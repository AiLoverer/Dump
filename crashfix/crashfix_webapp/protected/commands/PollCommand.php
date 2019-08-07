<?php

class PollCommand extends CConsoleCommand
{
	public function run($args)
	{
		Yii::log("Entering the method run", "info");
		
		if(0!=$this->checkDaemonStatus())
			return 1;
		
		// Delete old operations
		Operation::deleteOldOperations();
		
        // Look for started debug info deletion operations,check their statuses
		// and finalize completed operations.
        $this->checkDebugInfoDeletionOperations();
        
        // Delete debug info files marked for deletion
        $this->deletePendingDebugInfoFiles();                
        
		// Look for started debug info import operations, check their statuses
		// and finalize completed operations.
		$this->checkDebugInfoProcessingOperations();
		
		// Import new debug info files uploaded recently.
		$this->processNewDebugInfoFiles();
		
		// Look for started crash report processing operations, check their statuses
		// and finalize completed operations.
		$this->checkCrashReportProcessingOperations();
		
		// Process new crash report files uploaded recently.
		$this->processNewCrashReportFiles();
		
		// Send pending mail messages.
		MailQueue::sendMail();
		
        // Perform batch import of crash report files and PDB files
        $importDir = Yii::app()->getBasePath()."/import";
        $batchImporter = new BatchImporter();
        $importedCrashReportCount = 0;
        $importedDebugInfoCount = 0;
        $batchImporter->importFiles($importDir, $importedCrashReportCount,
                $importedDebugInfoCount);
        
        // Delete old temp files
        $this->deleteOldTempFiles();
        
		// Success
		Yii::log("Leaving the method run", "info");
		return 0;
	}
	
	/**
	 *  This method checks the daemon status.
	 */
	private function checkDaemonStatus()
	{
		Yii::log("Checking daemon status...", "info");
		
		$responce = "";
		$retCode = Yii::app()->daemon->getDaemonStatus($responce);
		if($retCode!=0)
		{
			Yii::log('Daemon status check returned an error: '.$retCode.' '.$responce.'\n', 'error');
			return 1;
		}
		
		// Success.
		Yii::log("Daemon status check succeeded.", "info");
		return 0;
	}
	
	/**
	 *  This method looks for debug info files uploaded recently and passes them
	 *  to daemon for processing. 
	 */
	private function processNewDebugInfoFiles()
	{
		Yii::log("Checking for new debug info files ready for import...", "info");
		
		// Get debug info files that have status 'Pending'
		$criteria=new CDbCriteria;
		$criteria->select='*';  
		$criteria->condition='status='.DebugInfo::STATUS_PENDING_PROCESSING;		
		$criteria->limit=10;
		$debugInfoFiles = DebugInfo::model()->findAll($criteria);
		if($debugInfoFiles==Null)		
			Yii::log('There are no debug info files ready for import', 'info');		
		else 
			Yii::log('Found '.count($debugInfoFiles).' debug info files ready for import', 'info');		
			
		foreach($debugInfoFiles as $debugInfo)
		{	
			// Format command parameters
			$fileName = $debugInfo->getLocalFilePath();
			$symDir = Yii::app()->getBasePath()."/data/debugInfo";
			$outFile = tempnam(Yii::app()->getRuntimePath(), "aop");				
			
			// Format command
			$command = 'assync dumper --import-pdb "'.$fileName.'" "'.$symDir.'" "'.$outFile.'"';
				
			// Execute the command
			$responce = "";
			$retCode = Yii::app()->daemon->execCommand($command, $responce);
				
			if($retCode!=0)
			{
				Yii::log('Error executing command '.$command.', responce = '.$responce, 'error');
				continue;
			}
			
			// Check responce and get command ID from server responce
			$matches = array();
			$check = preg_match(
					'#Assync command \{([0-9]{1,6}.[0-9]{1,9})\} has been added to the request queue.#', 
					$responce, $matches);			
			if(!$check || !isset($matches[1]))
			{
				Yii::log('Unexpected responce from command '.$command.', responce = '.$responce, 'error');
				continue;
			}			
			
			// Begin DB transaction
			$transaction = Yii::app()->db->beginTransaction();
						
			try
			{			
				// Create a new operation record in {{operation}} table.
				$op = new Operation;
				$op->status = Operation::STATUS_STARTED;
				$op->timestamp = time();
				$op->srcid = $debugInfo->id;
				$op->cmdid = $matches[1];
				$op->optype = Operation::OPTYPE_IMPORTPDB;
				$op->operand1 = $fileName;
				$op->operand2 = $outFile;
				if(!$op->save())
				{					
					throw new Exception('Could not save an operation record');
				}			
				
				// Update existing debug info record in {{debuginfo}} table.
				$debugInfo->status = DebugInfo::STATUS_PROCESSING_IN_PROGRESS;
				if(!$debugInfo->save())
				{					
					throw new Exception('Could not save a debug info record');										
				}
				
				// Commit transaction
				$transaction->commit();
			}
			catch(Exception $e)
			{
				// Roll back transaction
				$transaction->rollBack();
				
				Yii::log('An exception caught: '.$e->getMessage(), "error");				
			}
		}		
		
		Yii::log(
				"Finished checking for new debug info files ready for import.", 
				"info");
	}
	
	/**
	 *  This method looks for debug info import operations being in progress,
     *  requests the daemon for their status, and finalizes the operations 	 
	 *  that have been completed by the daemon.
	 */
	private function checkDebugInfoProcessingOperations()
	{
		Yii::log('Checking debug info import operations in progress...', 'info');		
		
		// Get the list of debug info importing operations that were started recently
		$criteria=new CDbCriteria;
		$criteria->select='*';  		
		$criteria->condition='optype=:optype AND status=:status';		
		$criteria->params=array(
							':optype'=>Operation::OPTYPE_IMPORTPDB, 
							':status'=>Operation::STATUS_STARTED,
						  );
		$criteria->limit=500;
		$operations = Operation::model()->findAll($criteria);
		if($operations==Null)		
			Yii::log('There are no operations in progress', 'info');		
		else 
			Yii::log(
					'Found '.count($operations).' operations in progress', 
					'info');		
			
		foreach($operations as $op)
		{
			$cmdRetCode = -1;
			$cmdRetMsg = "";
			$check = $this->checkAssyncCommand($op->cmdid, $cmdRetCode, $cmdRetMsg);
			if($check==self::CAC_STILL_RUNNING)
				continue; // Command is still running
			
			$opStatus = Operation::STATUS_FAILED;
			
			if($check==self::CAC_COMPLETED)
			{	
				// The operation seems to be completed.	
				if($cmdRetCode==0)
				{				
					// Read crash report information from the XML file.
					$xmlFileName = $op->operand2;
					$debugInfoId = $op->srcid;
					$importResult = $this->importDebugInfoFromXml(
							$xmlFileName, $debugInfoId);
				
					if($importResult==true)
						$opStatus = Operation::STATUS_SUCCEEDED;
				}
				else
				{
					// Associate a processing error with debug info record
					$this->addProcessingError(
						ProcessingError::TYPE_DEBUG_INFO_ERROR, 
						$op->srcid,
						$cmdRetMsg);
				}
			}
			else if($check==self::CAC_ERROR)
			{
				// Set operation status to 'Failed'
				$opStatus = Operation::STATUS_FAILED;				
				
				// Associate a processing error with debug info record
				$this->addProcessingError(
						ProcessingError::TYPE_DEBUG_INFO_ERROR, 
						$op->srcid,
						$cmdRetMsg);
			}
			
			// Finish operation
			$this->finalizeOperation($op, $opStatus);							
		}

		Yii::log("Finished checking debug info import operations in progress.", "info");
	}
	
	/**
	 * This method reads debug info information from XML file and
	 * updates appropriate database tables. 
	 * @param string $xmlFileName XML file name.
	 * @param integer $debugInfoId Debug info ID in database.
	 * @return boolean true on success.
	 */
	public function importDebugInfoFromXml($xmlFileName, $debugInfoId)
	{
		$status = false;
		
		// Find debug info record in {{debuginfo}} table.
		$debugInfo = DebugInfo::model()->find('id='.$debugInfoId);
		if($debugInfo==Null)
		{
			Yii::log('Not found debug info id='.$debugInfoId);					
			return false;
		}
				
		// Begin DB transaction
		$transaction = Yii::app()->db->beginTransaction();
		
		try
		{		
			// Load XML file
			$doc = simplexml_load_file($xmlFileName);
			if($doc==Null)
				throw new Exception('Invalid XML document '.$xmlFileName);
	
			// Get Summary element
			$elemSummary = $doc->Summary;
			if($elemSummary==Null)
				throw new Exception('Not found Summary element in XML document '.$xmlFileName);
							
			// Get PDB GUID and Age				
			$guid = $elemSummary->GUID;
			$age = $elemSummary->Age;
			if($guid==Null || $age==Null)
				throw new Exception('Not found Summary->GUID or Summary->Age element in XML document '.$xmlFileName);
						
			// Set guid attribute.
			$debugInfo->guid = $guid.$age;	// Concatenate GUID and Age								
		
			// Set status attribute.
			$debugInfo->status = DebugInfo::STATUS_PROCESSED;
					
			// Update DebugInfo table record
			if(!$debugInfo->save())
				throw new Exception('Error saving debug info AR to database.');
			
			// Commit transaction.
			$transaction->commit();
			
			$status = true;
		}
		catch(Exception $e)
		{
			// Rollback transaction
			$transaction->rollback();
			
			// Add a message to log
			Yii::log($e->getMessage(), 'error');	
			
			// Associate a processing error with debug info record
			$this->addProcessingError(
				ProcessingError::TYPE_DEBUG_INFO_ERROR, 
				$debugInfoId,
				$e->getMessage());
			
			$status = false;
		}
						
		return $status;
	}	
	
	/**
	 *  This method looks for crash report files uploaded recently and passes them
	 *  to daemon for processing. 
	 */
	private function processNewCrashReportFiles()
	{
		// Add a message to log
		Yii::log(
				"Checking for new crash report files ready for processing...", 
				"info");
		
		// Get crash report files that have status 'Pending'
		$criteria=new CDbCriteria;
		$criteria->select='*';  
		$criteria->condition='status='.CrashReport::STATUS_PENDING_PROCESSING;		
		$criteria->limit=10;
		$crashReportFiles = CrashReport::model()->findAll($criteria);
		if($crashReportFiles==Null)		
		{
			Yii::log(
					'There are no crash report files ready for processing', 
					'info');		
		}
		else 
		{
			Yii::log(
					'Found '.count($crashReportFiles).' crash report file(s) ready for processing', 
					'info');		
		}
			
		foreach($crashReportFiles as $crashReport)
		{	
			// Determine path to crash report file
			$fileName = $crashReport->getLocalFilePath();			
			// Create a temporary file for outputting results
			$outFile = tempnam(Yii::app()->getRuntimePath(), "aop");				
			
			// Format daemon command 
			$command = 'assync dumper --dump-crash-report "'.$fileName.'" "'.$outFile.'"';
			
            // Check if project allows to load PDB files without checking for matching build age
            if(isset($crashReport->project) && $crashReport->project->require_exact_build_age==false )
            {
                $command .= ' --relax-build-age';
            }
            
			// Execute the command
			$responce = "";
			$retCode = Yii::app()->daemon->execCommand($command, $responce);
				
			if($retCode!=0)
			{
				Yii::log('Error executing command '.$command.', responce = '.$responce, 'error');
				continue;
			}
			
			// Check response and get command ID from server responce
			$matches = array();
			$check = preg_match(
					'#Assync command \{([0-9]{1,6}.[0-9]{1,9})\} has been added to the request queue.#', 
					$responce, $matches);			
			if(!$check || !isset($matches[1]))
			{
				Yii::log('Unexpected responce from command '.$command.', responce = '.$responce, 'error');
				continue;
			}			
			
			// Begin DB transaction
			$transaction = Yii::app()->db->beginTransaction();
						
			try
			{			
				// Create a new operation record in {{operation}} table.
				$op = new Operation;
				$op->status = Operation::STATUS_STARTED;
				$op->timestamp = time();
				$op->srcid = $crashReport->id;
				$op->cmdid = $matches[1];
				$op->optype = Operation::OPTYPE_PROCESS_CRASH_REPORT;
				$op->operand1 = $fileName;
				$op->operand2 = $outFile;
				$op->operand3 = $crashReport->srcfilename;
				if(!$op->save())
				{					
					throw new Exception('Could not save an operation record');
				}			
				
				// Update existing crash report record in {{crashreport}} table.
				$crashReport->status = CrashReport::STATUS_PROCESSING_IN_PROGRESS;
				if(!$crashReport->save())
				{	
					//$errors = $crashReport->getErrors();
					//print_r($errors);
					throw new Exception('Could not save a crash report record ');										
				}
				
				// Commit transaction
				$transaction->commit();
			}
			catch(Exception $e)
			{
				// Roll back transaction
				$transaction->rollBack();
				// Remove temp file
				@unlink($outFile);
				// Add an error message to log
				Yii::log('An exception caught: '.$e->getMessage(), "error");				
			}
		}		
		
		Yii::log(
				"Finished checking for new crash report files ready for processing.", 
				"info");
	}
		
	/**
	 *  This method looks for crash report processing operations being in progress,
     *  requests the daemon for their status, and finalizes the operations 	 
	 *  that have been completed by the daemon.
	 */
	private function checkCrashReportProcessingOperations()
	{
		// Add a message to log file
		Yii::log(
				'Checking crash report processing operations in progress...', 
				'info');		
		
		// Get the list of crash report processing operations 
		// that were started recently
		$criteria=new CDbCriteria;
		$criteria->select='*';  		
		$criteria->condition='optype=:optype AND status=:status';		
		$criteria->params=array(
							':optype'=>Operation::OPTYPE_PROCESS_CRASH_REPORT, 
							':status'=>Operation::STATUS_STARTED,
						  );
		$criteria->limit=500;
		$operations = Operation::model()->findAll($criteria);
		if($operations==Null)		
		{
			Yii::log('There are no operations in progress', 'info');		
		}
		else 
		{
			Yii::log(
					'Found '.count($operations).' operations in progress', 
					'info');		
		}
			
		foreach($operations as $op)
		{			
			$cmdRetCode = -1;
			$cmdRetMsg = "";
			$check = $this->checkAssyncCommand($op->cmdid, $cmdRetCode, $cmdRetMsg);
			if($check==self::CAC_STILL_RUNNING)
				continue; // Command is still running
			
			$opStatus = Operation::STATUS_FAILED;
			
			// Delete records associated with this report.
			$crashReport = CrashReport::model()->findByPk($op->srcid);
			if($crashReport!=null)
			{
				if($crashReport->deleteAssociatedRecords())
				{			
					if($check==self::CAC_COMPLETED)
					{	
						// The operation seems to be completed.				
						if($cmdRetCode!=0)
						{
							// Associate a processing error with crash report record
							$this->addProcessingError(
								ProcessingError::TYPE_CRASH_REPORT_ERROR, 
								$op->srcid,
								$cmdRetMsg);
						}

						// Read crash report information from the XML file.
						$xmlFileName = $op->operand2;
						$crashReportId = $op->srcid;
						$importResult = $this->importCrashReportFromXml($xmlFileName, $crashReportId);
						if($importResult==true)
							$opStatus = Operation::STATUS_SUCCEEDED;
					}
					else if($check==self::CAC_ERROR)
					{
						// Set operation status to 'Failed'
						$opStatus = Operation::STATUS_FAILED;				

						// Associate a processing error with crash report record
						$this->addProcessingError(
							ProcessingError::TYPE_CRASH_REPORT_ERROR, 
							$op->srcid,
							$cmdRetMsg);
					}
				}
			}
			
			// Finish operation
			$this->finalizeOperation($op, $opStatus);			
		}

		Yii::log("Finished checking crash report processing operations in progress.", "info");
	}
		
	// Constants returned by checkAssyncCommand method.
	const CAC_ERROR          = -1; // An error occurred while checking command.
	const CAC_STILL_RUNNING  =  0; // The command is still executing.
	const CAC_COMPLETED      =  1; // The command has completed.
	
	/**
	 * This method checks if an assync daemon command has finished.
	 * @param type $cmdId 
	 */
	public function checkAssyncCommand($cmdId, &$cmdRetCode, &$cmdRetMsg)
	{
		// Format command to daemon
		$command = 'daemon get-assync-info -erase-completed '.$cmdId;				
			
		// Execute the command
		$responce = "";
		$retCode = Yii::app()->daemon->execCommand($command, $responce);
		
		// Add a message to log
		Yii::log('Command returned: '.$responce, 'info');
		
		// Check responce
		if(preg_match('/still executing/', $responce))
		{
			// This operation is still executing
			// TODO: check time of execution to notify about frozen operations
			Yii::log(
					'The operation'.$cmdId.' is still in progress', 
					'info');
			
			return self::CAC_STILL_RUNNING;
		}
		
		// Get command ID and command's return message.
		$matches = array();
		$check = preg_match(
				'#Command \{([0-9]{1,6}.[0-9]{1,9})\} returned \{(.+)\}#', 
				$responce, $matches);				
		if(!$check || count($matches)!=3)
		{
			Yii::log(
				'Unexpected response from command '.$command.', response = '.$responce, 
				'error');
			
            $cmdRetMsg = 'CrashFix service has encountered an unexpected internal error during processing this file.';
            
			return self::CAC_ERROR;
		}				

		// Check what command returned				
		$cmdRetMsg = $matches[2];
		$cmdRetCode = trim(strstr($cmdRetMsg, " ", true)); 							
		if(strlen($cmdRetCode)<=0)
			$cmdRetCode = -1;			
		if($cmdRetCode!=0)
		{
			Yii::log('Invalid command return code '.$cmdRetMsg, 'error');			
		}
        
        // Remove beginning code number from message
        $cmdRetMsg = trim(strstr($cmdRetMsg, " ", false)); 					
		
		return self::CAC_COMPLETED;
	}
	
	/**
	 * This method reads crash report information from XML file and
	 * updates appropriate database tables. 
	 * @param string $xmlFileName XML file name.
	 * @param integer $crashReportId Crash report ID in database.
	 * @return boolean true on success.
	 */
	public function importCrashReportFromXml($xmlFileName, $crashReportId)
	{
		$status = false;
		
		// Find appropriate {{crashreport}} table record
		$criteria=new CDbCriteria;
		$criteria->select='*';  
		$criteria->condition='id='.$crashReportId;						
		$crashReport = CrashReport::model()->find($criteria);
		if($crashReport==Null)
		{
			Yii::log('Not found crash report id='.$crashReportId);
			return $status;
		}
				
		$crashReport->status = CrashReport::STATUS_PROCESSED;
		
		// Begin DB transaction
		$transaction = Yii::app()->db->beginTransaction();
		
		try{
					
			// Load XML file
			$doc = @simplexml_load_file($xmlFileName);
			if($doc==Null)
				throw new Exception('CrashFix service has encountered an error when retrieving information from crash report file');
			
			// Get command return status message from XML
			$elemSummary = $doc->Summary;
			if($elemSummary==Null)
				throw new Exception('Internal error: not found Summary element in XML document '.$xmlFileName);

			// Extract crash report info
			$generatorVersion = (int)$elemSummary->GeneratorVersion;
			$crashGuid = (string)$elemSummary->CrashGUID;
			$appName = (string)$elemSummary->ApplicationName;
			$appVersion = (string)$elemSummary->ApplicationVersion;
			$exeImage = (string)$elemSummary->ExecutableImage;
			$dateCreated = (string)$elemSummary->DateCreatedUTC;
			$osNameReg = (string)$elemSummary->OSNameReg;
			$osVersionMinidump = (string)$elemSummary->OSVersionMinidump;
			$osIs64Bit = (int)$elemSummary->OSIs64Bit;
			$geoLocation = (string)$elemSummary->GeographicLocation;
			$productType = (string)$elemSummary->ProductType;
			$cpuArchitecture = (string)$elemSummary->CPUArchitecture;
			$cpuCount = (int)$elemSummary->CPUCount;
			$guiResourceCount = (int)$elemSummary->GUIResourceCount;
			$openHandleCount = $elemSummary->OpenHandleCount;
			$memoryUsageKbytes = $elemSummary->MemoryUsageKbytes;
			$exceptionType = (string)$elemSummary->ExceptionType;
			$exceptionAddress = $elemSummary->ExceptionAddress;
			$sehExceptionCode = $elemSummary->SEHExceptionCode;			
			$exceptionThreadID = $elemSummary->ExceptionThreadID;
			$exceptionModuleName = (string)$elemSummary->ExceptionModuleName;
            
            $exceptionModuleBase = (string)$elemSummary->ExceptionModuleBase;
            if(strlen($exceptionModuleBase)==0)
                $exceptionModuleBase = 0;
                
			$crashReport->exceptionmodulebase = $exceptionModuleBase;
			$userEmail = (string)$elemSummary->UserEmail;
			$problemDescription = (string)$elemSummary->ProblemDescription;

			// Set crash report fields
			$crashReport->status = CrashReport::STATUS_PROCESSED;				
			$crashReport->crashrptver = $generatorVersion;
			$crashReport->crashguid = $crashGuid;				
						
			$ver = AppVersion::createIfNotExists($appVersion, $crashReport->project_id);
			$crashReport->appversion_id = $ver->id;
			
			if(strlen($userEmail)!=0)
				$crashReport->emailfrom = $userEmail;
			
			if(strlen($problemDescription)!=0)
				$crashReport->description = $problemDescription;

			if(strlen($dateCreated)!=0)
				$crashReport->date_created = strtotime($dateCreated);

			$crashReport->os_name_reg = $osNameReg;
			$crashReport->os_ver_mdmp = $osVersionMinidump;
			$crashReport->os_is_64bit = $osIs64Bit;
			$crashReport->geo_location = $geoLocation;
			$crashReport->product_type = $productType;
			$crashReport->cpu_architecture = $cpuArchitecture;
			$crashReport->cpu_count = $cpuCount;
			$crashReport->gui_resource_count = $guiResourceCount;
			$crashReport->memory_usage_kbytes = $memoryUsageKbytes;
			$crashReport->open_handle_count = $openHandleCount;
			$crashReport->exception_type = $exceptionType;			
            
            if(strlen($sehExceptionCode)!=0)
                $crashReport->exception_code = $sehExceptionCode;                        
            
            if(strlen($exceptionThreadID)!=0)
                $crashReport->exception_thread_id = $exceptionThreadID;
            
            if(strlen($exceptionAddress)!=0)
                $crashReport->exceptionaddress = $exceptionAddress;
            
            if(strlen($exceptionModuleName)!=0)
                $crashReport->exceptionmodule = $exceptionModuleName;
            
            if(strlen($exceptionModuleBase)!=0)
                $crashReport->exceptionmodulebase = $exceptionModuleBase;
            
			$crashReport->exe_image = $exeImage;

			// Validate crash report fields
			if(!$crashReport->validate())
			{
				// There are some errors
				$errors = $crashReport->getErrors();
				foreach($errors as $fieldName=>$fieldErrors)
				{
					foreach($fieldErrors as $errorMsg)
					{
						// Add an error message to log
						Yii::log(
								'Error in crashreport data ('.$crashReport->$fieldName.'): '.$errorMsg, 
								'error');					
						
						// Associate a processing error with crash report record
						$this->addProcessingError(
							ProcessingError::TYPE_CRASH_REPORT_ERROR, 
							$crashReport->id,
							$errorMsg.' ('.$crashReport->$fieldName.')');						
                        
                        // Clear field - this should fix the error
						unset($crashReport->$fieldName);
					}
				}

				// Clear validation errors
				$crashReport->clearErrors();
			}
			
			// Extract file items
			$elemFileList = $doc->FileList;
			if($elemFileList!=Null)
			{
				$i=0;
				foreach($elemFileList->Row as $elemRow)
				{	
					$i++;
					if($i==1)
						continue; // Skip header row

					$itemNo = $elemRow->Cell[0]['val'];
					$itemName = $elemRow->Cell[1]['val'];
					$itemDesc = $elemRow->Cell[2]['val'];

					$fileItem = new FileItem;
					$fileItem->filename = $itemName;
					$fileItem->description = $itemDesc;
					$fileItem->crashreport_id = $crashReportId;

					if(!$fileItem->save())
						throw new Exception('Could not save file item record');					
				}					
			}			
			
			// Extract custom props
			$elemAppDefinedProps = $doc->ApplicationDefinedProperties;
			if($elemAppDefinedProps!=Null)
			{
				$i=0;
				foreach($elemAppDefinedProps->Row as $elemRow)
				{	
					$i++;
					if($i==1)
						continue; // Skip header row

					$itemNo = $elemRow->Cell[0]['val'];
					$name = $elemRow->Cell[1]['val'];
					$val = $elemRow->Cell[2]['val'];

					$customProp = new CustomProp;
					$customProp->name = $name;
					$customProp->value = $val;
					$customProp->crashreport_id = $crashReportId;

					if(!$customProp->save())
						throw new Exception('Could not save custom property record');					
				}					
			}
			
			// Extract the list of modules
			$elemModuleList = $doc->ModuleList;            
			if($elemModuleList!=Null && $elemModuleList->count()!=0)
			{
				$i=0;
				foreach($elemModuleList->Row as $elemRow)
				{	
					$i++;
					if($i==1)
						continue; // Skip header row

					$itemNo = $elemRow->Cell[0]['val'];
					$name = $elemRow->Cell[1]['val'];
					$symLoadStatus = $elemRow->Cell[2]['val'];
					$loadedPdbName = $elemRow->Cell[3]['val'];
					$loadedPdbGUID = $elemRow->Cell[4]['val'];
					$fileVersion = $elemRow->Cell[5]['val'];
					$timeStamp = $elemRow->Cell[6]['val'];
                    $guidnAge = $elemRow->Cell[7]['val'];
						
					$module = new Module;
					$module->crashreport_id = $crashReportId;
					$module->name = $name;
					$module->sym_load_status = $symLoadStatus;					
					$module->file_version = $fileVersion;					
					$module->timestamp = $timeStamp;					
					$module->matching_pdb_guid = $guidnAge;					
                    
					$debugInfo = DebugInfo::model()->findByAttributes(array('guid'=>$loadedPdbGUID));
					if($debugInfo!=null)
						$module->loaded_debug_info_id = $debugInfo->id;
										
					if(!$module->save())
						throw new Exception('Could not save module record');					
				}					
			}
			
			// Extract the list of stack traces
			foreach($doc->StackTrace as $elemStackTrace)			
			{
				$threadId = $elemStackTrace->ThreadID;
				$stackTraceMD5 = $elemStackTrace->StackTraceMD5;
				
				$thread = new Thread;				
				$thread->thread_id = $threadId;
				$thread->crashreport_id = $crashReportId;
				
				if(strlen($stackTraceMD5)!=0)
					$thread->stack_trace_md5 = $stackTraceMD5;
				
				if(!$thread->save())
						throw new Exception('Could not save thread record');					
				
				$i=0;
				foreach($elemStackTrace->Row as $elemRow)
				{	
					$i++;
					if($i==1)
						continue; // Skip header row

					$title = $elemRow->Cell[0]['val'];
					$addrPC = $elemRow->Cell[1]['val'];
					$moduleName = $elemRow->Cell[2]['val'];
					$offsInModule = $elemRow->Cell[3]['val'];
					$symName = $elemRow->Cell[4]['val'];
					$undSymName = $elemRow->Cell[5]['val'];
					$offsInSym = $elemRow->Cell[6]['val'];
					$srcFile = $elemRow->Cell[7]['val'];
					$srcLine = $elemRow->Cell[8]['val'];
					$offsInLine = $elemRow->Cell[9]['val'];
					
					$stackFrame = new StackFrame;					
					$stackFrame->thread_id = $thread->id;
					$stackFrame->addr_pc = $addrPC;					
					
					if(strlen($moduleName)!=0)
					{
						$module = Module::model()->findByAttributes(
                                array(
                                    'name'=>$moduleName, 
                                    'crashreport_id'=>$crashReportId
                                    )
                                );
						if($module!=null)
							$stackFrame->module_id = $module->id;
						
						$stackFrame->offs_in_module = $offsInModule;
					}
					
					if(strlen($symName)!=0)
						$stackFrame->symbol_name = $symName;
					
					if(strlen($undSymName)!=0)
						$stackFrame->und_symbol_name = $undSymName;
					
					if(strlen($offsInSym)!=0)
						$stackFrame->offs_in_symbol = $offsInSym;
										
					if(strlen($srcFile)!=0)
						$stackFrame->src_file_name = $srcFile;	
					
					if(strlen($srcLine)!=0)
						$stackFrame->src_line = $srcLine;	
					
					if(strlen($offsInLine)!=0)
						$stackFrame->offs_in_line = $offsInLine;	
					
					if(!$stackFrame->save())
						throw new Exception('Could not save stack frame record');					
				}					
			}
						
			// Commit transaction
			$transaction->commit();
			
			// Success.
			$status = true;
		}
		catch(Exception $e)
		{
			// Rollback transaction
			$transaction->rollback();
			
			// Add a message to log
			Yii::log($e->getMessage(), 'error');	
			
			$crashReport->status = CrashReport::STATUS_INVALID;
			
			// Associate a processing error with crash report record
			$this->addProcessingError(
				ProcessingError::TYPE_CRASH_REPORT_ERROR, 
				$crashReport->id,
				$e->getMessage());						
			
			$status = false;
		}	
		
		// Update crash group based on new data
		$crashGroup = $crashReport->createCrashGroup();
		if($crashGroup==Null)
		{
			Yii::log('Error creating crash group', 'error');					
			$status = false;			
		}
				
		$crashReport->groupid = $crashGroup->id;
						
		// Update crash report
		$saved = $crashReport->save();
		if(!$saved)
		{
			Yii::log('Error saving AR crashReport', 'error');					
			$status = false;			
		}
				
		if(!$saved || !$crashReport->checkQuota())
		{					
			Yii::log('Error checking crash report quota', 'error');					
			$status = false;			
			
			// Delete crash report
			$crashReport = $crashReport = CrashReport::model()->find('id='.$crashReport->id);
			$crashReport->delete();
		}
		
		// Return status
		return $status;			
	}
	
	/**
	 * This method finalizes the given operation, updates appropriate db tables
	 * and removes temporary files.	 
	 */
	public function finalizeOperation($op, $opStatus)
	{
		// Update operation db record.
		$op->status = $opStatus;
		$op->save();

		if($op->optype==Operation::OPTYPE_PROCESS_CRASH_REPORT)
		{			
			// Delete temporary files and directories								
			$crashReportFileName = $op->operand1;
			$xmlFileName = $op->operand2;

			if($opStatus!=Operation::STATUS_SUCCEEDED)
			{
				// Set crash report record status to Invalid
				Yii::log('Setting crash report #'.$op->srcid.' status to invalid ', 'error');					
				$crashReport = CrashReport::model()->find('id='.$op->srcid);
				if($crashReport!=null)
				{
					$crashReport->status = DebugInfo::STATUS_INVALID;
					$crashReport->save();		
				}
			}
			
			// Delete temp XML file		
			Yii::log('Deleting file '.$xmlFileName, 'info');
			if(!@unlink($xmlFileName))
			{
				// Couldn't delete file
				Yii::log('Error deleting file '.$xmlFileName, 'error');					
			}				
		}
		else if($op->optype==Operation::OPTYPE_IMPORTPDB)
		{
			// Delete temporary files and directories										
			$pdbFileName = $op->operand1;
			$xmlFileName = $op->operand2;
		
			if($opStatus!=Operation::STATUS_SUCCEEDED)
			{
				// Set debug info record status to Invalid
				Yii::log('Setting debug info #'.$op->srcid.' status to invalid ', 'error');					
				$debugInfo = DebugInfo::model()->find('id='.$op->srcid);
				if($debugInfo!=null)
				{
					$debugInfo->status = DebugInfo::STATUS_INVALID;
					$debugInfo->save();		
				}
			}
			
			// Delete file		
			Yii::log('Deleting file '.$xmlFileName, 'info');					
			if(!@unlink($xmlFileName))
			{
				// Couldn't delete file
				Yii::log('Error deleting file '.$xmlFileName, 'error');					
			}
			
			// If operation has succeeded, we can remove original PDB file and 
			// its parent directories, because the file was copied by daemon
			// to its persistent location.
			if($op->status==Operation::STATUS_SUCCEEDED)
			{
				// Delete file		
				Yii::log('Deleting file '.$pdbFileName, 'info');					
				if(!@unlink($pdbFileName))
				{
					// Couldn't delete file
					Yii::log('Error deleting file '.$pdbFileName, 'error');					
				}
				
				// Delete parent directory.		
				$parentDirName = dirname($pdbFileName);
				Yii::log('Deleting directory '.$parentDirName, 'info');					
				if(!@rmdir($parentDirName))
				{
					// Couldn't delete directory.
					Yii::log('Error deleting directory '.$parentDirName, 'error');					
				}

				// Delete first-level parent directory.		
				$parentDirName = dirname($parentDirName);
				Yii::log('Deleting directory '.$parentDirName, 'info');					
				if(!@rmdir($parentDirName))
				{
					// Couldn't delete dir
					Yii::log('Error deleting directory '.$parentDirName, 'error');					
				}
			}
		}
	}
	
	/**
	 * Creates a processing error record and associates it with a crash report or
	 * with a debug info
	 * @param integer $type Error type.
	 * @param integer $srcid Crash report ID or debug info ID.
	 * @param string $message Error message.
	 * @return void
	 */
	private function addProcessingError($type, $srcid, $message)
	{
		$processingError = new ProcessingError;
		$processingError->type = $type;
		$processingError->srcid = $srcid;
		$processingError->message = $message;
		
		if(!$processingError->save())
		{
			Yii::log(
					'Couldnot save processing error record', 
					'error');						
		}
	}
    
    /**
     * This method removes old temporarily created files to keep the temp folder
     * clear.
     * @returns integer Count of files deleted.
     */
    private function deleteOldTempFiles()
    {
        $countDeleted = 0;
        
        $dirName = Yii::app()->getRuntimePath();
        
        // Check the directory exists
        if(!is_dir($dirName))
        {
            Yii::log('BatchImporter: not a directory: '.$dirName, 'error');
            return -1;
        }
        
        // Get file list in the directory
        $fileList = scandir($dirName);
        if($fileList==false)
        {
            Yii::log('Directory name is invalid: '.$dirName, 'error');
            return false;
        }
        
        // Walk through files
        foreach($fileList as $index=>$file)
        {
            // Get abs path
            $fileName = $dirName.'/'.$file;
            
            // Strip file parts
            $path_parts = pathinfo($fileName);             
            
            // Delete only old .tmp files
            if($file!='.' && $file!='..' && is_file($fileName))
            {
                if(!isset($path_parts['extension']) || strtolower($path_parts['extension'])!='log')
                {
                    // Check file modification date
                    $modificationTime = filemtime($fileName);

                    // If it was modified more than a day ago
                    if($modificationTime < (time()-(60*60*24)))
                    {
                        // Remove the file, use @ to avoid PHP warnings if 
                        // file cannot be removed by some reason
                        if(false==@unlink($fileName))
                        {
                            Yii::log('Old temporary file cannot be deleted by some reason: '.$fileName, 'info');
                        }

                        // Increment counter
                        $countDeleted++;
                    }
                }
            }
        }
        
        return $countDeleted;
    }
    
    private function checkDebugInfoDeletionOperations()
    {
        Yii::log('Checking debug info deletion operations in progress...', 'info');		
		
		// Get the list of debug info deletion operations that were started recently
		$criteria=new CDbCriteria;
		$criteria->select='*';  		
		$criteria->condition='optype=:optype AND status=:status';		
		$criteria->params=array(
							':optype'=>Operation::OPTYPE_DELETE_DEBUG_INFO, 
							':status'=>Operation::STATUS_STARTED,
						  );
		$criteria->limit=500;
		$operations = Operation::model()->findAll($criteria);
		if($operations==Null)		
			Yii::log('There are no operations in progress', 'info');		
		else 
			Yii::log(
					'Found '.count($operations).' operations in progress', 
					'info');		
			
		foreach($operations as $op)
		{
			$cmdRetCode = -1;
			$cmdRetMsg = "";
			$check = $this->checkAssyncCommand($op->cmdid, $cmdRetCode, $cmdRetMsg);
			if($check==self::CAC_STILL_RUNNING)
				continue; // Command is still running
			
			$opStatus = Operation::STATUS_FAILED;
			
			if($check==self::CAC_COMPLETED)
			{	               
				// The operation seems to be completed.	
				$debugInfoId = $op->srcid;
                
				$debugInfo = DebugInfo::model()->findByPk($debugInfoId);
                if($debugInfo!=null)
                {
                    // Delete debug info record
                    $debugInfo->delete();
                    
                    // Set operation status to 'Succeeded'
                    $opStatus = Operation::STATUS_SUCCEEDED;
                }
			}
			else if($check==self::CAC_ERROR)
			{
                // Delete debug info record
                $debugInfoId = $op->srcid;
                $debugInfo = DebugInfo::model()->findByPk($debugInfoId);
                if($debugInfo!=null)
                    $debugInfo->delete();
                    
				// Set operation status to 'Failed'
				$opStatus = Operation::STATUS_FAILED;				
			}
			
			// Finish operation
			$this->finalizeOperation($op, $opStatus);							
		}

		Yii::log("Finished checking debug info deletion operations in progress.", "info");
    }    
        
    private function deletePendingDebugInfoFiles()
    {
        // Add a message to log
		Yii::log(
				"Checking for debug info records marked for deletion...", 
				"info");
		
		// Get debug info files that have status 'Pending Delete'
		$criteria=new CDbCriteria;
		$criteria->select='*';  
		$criteria->condition='status='.DebugInfo::STATUS_PENDING_DELETE;
		$criteria->limit=20;
		$debugInfoFiles = DebugInfo::model()->findAll($criteria);
		if($debugInfoFiles==Null)		
		{
			Yii::log(
					'There are no debug info files waiting for deletion', 
					'info');		
		}
		else 
		{
			Yii::log(
					'Found '.count($debugInfoFiles).' debug info file(s) ready for deletion', 
					'info');		
		}
			
		foreach($debugInfoFiles as $debugInfo)
		{	
			// Determine path to debug info file
			$fileName = $debugInfo->getLocalFilePath();			
			
			// Format daemon command 
			$command = 'assync dumper --delete-debug-info "'.$fileName.'"';
				
			// Execute the command
			$responce = "";
			$retCode = Yii::app()->daemon->execCommand($command, $responce);
				
			if($retCode!=0)
			{
				Yii::log('Error executing command '.$command.', response = '.$responce, 'error');
				continue;
			}
			
			// Check responce and get command ID from server responce
			$matches = array();
			$check = preg_match(
					'#Assync command \{([0-9]{1,6}.[0-9]{1,9})\} has been added to the request queue.#', 
					$responce, $matches);			
			if(!$check || !isset($matches[1]))
			{
				Yii::log('Unexpected response from command '.$command.', responce = '.$responce, 'error');
				continue;
			}			
			
			// Begin DB transaction
			$transaction = Yii::app()->db->beginTransaction();
						
			try
			{			
				// Create a new operation record in {{operation}} table.
				$op = new Operation;
				$op->status = Operation::STATUS_STARTED;
				$op->timestamp = time();
				$op->srcid = $debugInfo->id;
				$op->cmdid = $matches[1];
				$op->optype = Operation::OPTYPE_DELETE_DEBUG_INFO;
				$op->operand1 = $fileName;				
				if(!$op->save())
				{					
					throw new Exception('Could not save an operation record');
				}			
				
				// Update existing debug info record in {{debuginfo}} table.
				$debugInfo->status = DebugInfo::STATUS_DELETE_IN_PROGRESS;
				if(!$debugInfo->save())
				{	
					//$errors = $crashReport->getErrors();
					//print_r($errors);
					throw new Exception('Could not save a debug info record ');										
				}
				
				// Commit transaction
				$transaction->commit();
			}
			catch(Exception $e)
			{
				// Roll back transaction
				$transaction->rollBack();
				// Add an error message to log
				Yii::log('An exception caught: '.$e->getMessage(), "error");				
			}
		}		
		
		Yii::log(
				"Finished checking for debug info files ready for deletion.", 
				"info");
    }
};


