<?php

/**
 * This is the model class for table "{{crashreport}}".
 */
class CrashReport extends CActiveRecord
{
	// Crash report status codes
	const STATUS_PENDING_PROCESSING     = 1; // The crash report awaiting processing by daemon
	const STATUS_PROCESSING_IN_PROGRESS = 2; // The debug info file is currently being processed by daemon.
	const STATUS_PROCESSED              = 3; // The crash report was processed by daemon
	const STATUS_INVALID                = 4; // The crash report file is marked by daemon as an invalid PDB file.
	
	public $appversion; // Non-db field that stores appversion string for this report.
	public $fileAttachment; // File attachment CUploadedFile
    public $fileAttachmentIsUploaded = true;    // Set to TRUE if attachment was uploaded; or set to false if it resides in local filesystem.
	public $ignoreFileAttachmentErrors = false; // Used in tests
	public $cnt; // Count (used in stat query)
	
	// Search related attributes
	public $filter; // Simple search filter.
	public $isAdvancedSearch = false; // Is advanced search enabled?
	public $receivedFrom; // Received from time(used in advanced search).
	public $receivedTo;   // Received to time (used in advanced search).
	
	/**
	 * Returns the static model of the specified AR class.
	 * @return CrashReport the static model class
	 */
	public static function model($className=__CLASS__)
	{
		return parent::model($className);
	}

	/**
	 * @return string the associated database table name
	 */
	public function tableName()
	{
		return '{{crashreport}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		$rules = array(						
			array('md5', 'length', 'is'=>32),	// MD5 hash should be 32 characters in length
			array('crashguid', 'length', 'is'=>36), // Crash GUID length should be 36 characters in length			
			array('appversion', 'length', 'min'=>1, 'max'=>32),  // Application version string should be shorter than 32 characters
			array('description', 'length', 'max'=>256), // Problem description should not be longer than 256 characters 	
			array('crashrptver', 'numerical', 'integerOnly'=>true, 'min'=>1000, 'max'=>9999), // CrashRpt version should be in range [1000; 9999]
			// The following rules is used on search
			array('id, received, status, md5, crashguid, appversion, ipaddress, emailfrom, description', 'safe', 'on'=>'search'),
			array('receivedTo', 'compareFromToDates', 'on'=>'search'),
		);
		
		if($this->scenario!='search')
        {            
			$rules[] = array('emailfrom', 'email'); // Emailfrom should be a valid email address 	
        }
            
            		
		// For new record, add file attachment validation
		if($this->isNewRecord && $this->scenario!='search')			
		{
            $rules[] = array('crashguid', 'checkCrashGUIDExists'); // Crash GUID must be unique			
			$rules[] = array('fileAttachment', 'file', 'types' => 'zip', 'maxFiles'=>1, 'minSize'=>1);
			$rules[] = array('fileAttachment', 'checkFileAttachment');
		}
		
		return $rules;
	}
	
    /**
	 *  This validator method is used by rules method and checks if a crash report with such a GUID exists in the database or not.
	 *  If file already exits, it adds error message and returns false. If file doesn't exist, it returns true.
	 */
	public function checkCrashGUIDExists()
	{		
		// If crash GUID is specified, check if it is unique within the database
		
		if(!isset($this->crashguid))
			return true; // Crash GUID is empty, skip this check
		
		// Check if such a crash GUID already exists in the database.
		$model = CrashReport::model()->find(array(
										'condition'=>'crashguid=:crashguid',
										'params'=>array(':crashguid'=>$this->crashguid,),
										));
										
		if($model!==Null)
		{
			// Found a db record
			$this->addError('crashguid', 'Such a crash GUID is already existing in the database.');
			return false; // Found such a GUID - check failed.
		}
		
		// Success - crash GUID is unique.
		return true;
	}

	/**
	 * This validator method is used to ensure that uploaded file attachment
	 * has the same MD5 hash as the 'md5' attribute of the model.
	 */
	public function checkFileAttachment()
	{
		if(!isset($this->md5))
			return true; // Nothing to check
				
		if($this->fileAttachment==Null)
		{
			$this->addError('fileAttachment', 'File attachment is missing.');
			return false; // Failed 
		}
				
		// Calculate file attachment MD5 hash
		$md5 = md5_file($this->fileAttachment->getTempName());
		
		// Compare both hashes
		if($md5!=$this->md5)
		{
			$this->addError('fileAttachment', 'File attachment MD5 hash mismatch.');
			return false; // Failed - hash mismatch
		}
		
		// Success - md5 is correct
		return true;		
	}
	
	/**
	 *  This validator compares Date From and Date To to ensure Date To is the greater one.
	 */
	public function compareFromToDates()
	{
		// Check scenarion
		if($this->scenario!='search')
			return true;
		
		// Check if both dates are set
		if(!isset($this->receivedFrom) || !isset($this->receivedTo))
			return true;
		
		// Convert date strings to timestamps
		$date1 = $this->strToDate($this->receivedFrom);
		$date2 = $this->strToDate($this->receivedTo);
		
		// Check timestamps
		if($date1==False || $date2==False)
			return true; // One of dates is invalid - nothing to compare
		
		// Compare dates
		if($date2>=$date1)
			return true; // Date To is greater than Date From
		
		// Error
		$this->addError('receivedTo', 'Received To must be greater or equal to Received From.');
		return false;		
	}
	
	/**
	 *  A helper method that converts a date string to timestamp.
	 */
	public function strToDate($date)
	{
		$timestamp=CDateTimeParser::parse($date,'MM/dd/yyyy');
		return $timestamp;
	}
		
	/**
	 * Checks if project quota allows to add this crash report to project.
	 * @return boolean true on success.
	 */
	public function checkQuota()
	{
		// Get project this crash report belongs to.
		$project = Project::model()->findByPk($this->project_id);
		if($project==Null)
		{
            // Project not found?
			$this->addError('project_id', 'Invalid project ID.');			
			return false; 
		}
        
        // Check if project has limited disk quota
        if($project->crash_report_files_disc_quota>0)
        {
            // Get total file size of crash reports that are already 
            // stored in project.
            $totalFileSize = 0;
            $percentOfQuota = 0;
            $project->getCrashReportCount($totalFileSize, $percentOfQuota);
            
            // Check if adding this crash report to project will 
            // exceed the quota.
            if($project->crash_report_files_disc_quota*1024*1024 < 
                    $totalFileSize+$this->filesize)
            {
                // Quota has exceeded
                $this->addError(
                        'fileAttachment', 
                        'Crash report disc quota for this project has exceeded.');			
                return false; 
            }
        }
		
        // Return true to indicate it is allowed to add this crash 
        // report to project.
		return true;
	}
	
	/**
	 *  This method moves uploaded file to its persistent location.
	 *  @return On success, returns true. On failure, sets error message and returns false.
	 */
	public function saveFileAttachment()
	{
		if($this->fileAttachment==null)
		{
			$this->addError('fileAttachment', 'File attachment is missing.');			
			return false; // File attachment missing
		}
		
		// Set srcfilename attribute
		$this->srcfilename = basename($this->fileAttachment->getName());
		
		// Set filesize attribute
		$this->filesize = $this->fileAttachment->getSize();
		
        // Check disc quota for this project
		if(!$this->checkQuota())
			return false;
		
		// Calc md5 hash and save it as model attribute
		$this->md5 = md5_file($this->fileAttachment->getTempName());
								
		// Create the directory where we will place the uploaded file
		$subDir1 = substr($this->md5, 0, 3);
		$subDir2 = substr($this->md5, 3, 3);		
		$dirName = Yii::app()->getBasePath()."/data/crashReports/".$subDir1."/".$subDir2;
		if(!@is_dir($dirName))
		{
			if(False==@mkdir($dirName, 0777, TRUE))
			{
				$error = error_get_last();
				$this->addError("fileAttachment", "Couldn't make directory: ". $error);
				return false;
			}
		}
		
        // Move attachment file to an appropriate directory and delete temp file.
        // Handle this differently for uploaded file and local file.
        if($this->fileAttachmentIsUploaded==true)
        {
            // Move uploaded file to an appropriate directory and delete temp file
            $fileName = $dirName."/".$this->md5.".zip";
            if(!$this->fileAttachment->saveAs($fileName, true) && !$this->ignoreFileAttachmentErrors)
            {
                $this->addError("fileAttachment", "Couldn't save data to local storage"); 
                return false;
            }
        }
        else
        {
            // Local file.
            $fileName = $dirName."/".$this->md5.".zip";
            if(!copy($this->fileAttachment->getTempName(), $fileName))
            {
                $this->addError("fileAttachment", "Couldn't copy data to local storage"); 
                return false;
            }
            
            // Remove temp file.
            if(!unlink($this->fileAttachment->getTempName()))
            {
                $this->addError("fileAttachment", "Couldn't remove temp attachment file."); 
                return false;
            }            
        }
		
		// Success.
		return true;
	}
	
	/**
	 *  This method returns the absolute local path to this crash report file.	 
	 */
	public function getLocalFilePath()
	{
		if(!isset($this->md5) || strlen($this->md5)!=32)
			return false;
		
		// Determine path to local file
		$subDir1 = substr($this->md5, 0, 3);
		$subDir2 = substr($this->md5, 3, 3);				
		$dirName = Yii::app()->getBasePath()."/data/crashReports/".$subDir1."/".$subDir2;	
		$fileName = $dirName."/".$this->md5.'.zip';
		return $fileName;
	}
	
	/**
	 * This method looks for an existing crash group for the given crash report
	 * and if the crash group doesn't exist, it creates a new one.
	 * @return CrashGroup crash group model, or Null on error.
	 */
	public function createCrashGroup()
	{
		// Get crash group title
		$crashGroupMD5 = "";
		$crashGroupTitle = $this->getCrashGroupTitle($crashGroupMD5);
		
		Yii::log('Crash group title = '.$crashGroupTitle, 'info');
		Yii::log('Crash group MD5 = '.$crashGroupMD5, 'info');
		
		$criteria = new CDbCriteria();		
		$criteria->compare('project_id', $this->project_id);
		$criteria->compare('appversion_id', $this->appversion_id);
        $criteria->compare('md5', $crashGroupMD5);
		
		// Look for existing crash group
		$crashGroup = CrashGroup::model()->find($criteria);
		if($crashGroup===Null)
		{
            // Not found, create new crash group			
            
            // First check if such a title already presents
            $criteria = new CDbCriteria();            
            $criteria->compare('project_id', $this->project_id);
            $criteria->compare('appversion_id', $this->appversion_id);
            $criteria->addSearchCondition('title', $crashGroupTitle);
            $count = CrashGroup::model()->count($criteria);
            if($count!=0)
            {
                // Add suffix to the title to avoid similar group names
                $postfix = ' ('. ($count+1) .')';
                $crashGroupTitle .= $postfix;
            }            
            
			// Create new crash group
			$crashGroup = new CrashGroup();
			$crashGroup->title = $crashGroupTitle;
			$crashGroup->md5 = $crashGroupMD5;
			$crashGroup->project_id = $this->project_id;
			$crashGroup->appversion_id = $this->appversion_id;
			
			if(!$crashGroup->save())
			{
				// Error saving crash group record				
				return Null;
			}
		}
		
		// Return crash group
		return $crashGroup;
	}
	
	/**
	 * This method is executed before AR is saved to database.
	 * @return boolean True on success.
	 */
	protected function beforeSave()
	{
		if(!parent::beforeSave())
			return false;

		// Skip the following code on update.
		if(!$this->isNewRecord)
			return true;
				
		// Set project name
		if(!isset($this->project_id))
		{
			// Get current project.
			$curProject = Yii::app()->user->getCurProject();
			if($curProject===Null)
				return false;
		
			$this->project_id = $curProject->id;
		}
		
		// Check if project is active or disabled. Do not allow to save
		// if project is disabled.
		$project = Project::model()->findByPk($this->project_id);
		if($project===Null) 
		{	
			$this->addError('project_id', 'No such a project name found.');
			return false;
		}
		
		if($project->status != Project::STATUS_ACTIVE)
		{
			$this->addError('project_id', 'This project is disabled and not accepting crash reports.');
			return false;
		}
		
		// Set 'appversion_id' attribute.					
		$ver = AppVersion::createIfNotExists(
				isset($this->appversion)?$this->appversion:'(not set)', 
				$this->project_id);
		if($ver===Null)
		{
			$this->addError('appversion', 'Invalid application version.');
			return false;
		}
		$this->appversion_id = $ver->id;
		
		// Set crash group ID.
		$crashGroup = $this->createCrashGroup();
		if($crashGroup===Null)
			return false;		
		$this->groupid = $crashGroup->id;
		
        // Check if project has quota of crash reports per group
        if($project->crash_reports_per_group_quota>0)
        {
            // Check count of crash reports in crash group and compare it with
            // quota.
            $reportCount = $crashGroup->crashReportCount;
            if($reportCount>=$project->crash_reports_per_group_quota)
            {
                $this->addError(
                        'fileAttachment', 
                        'Max crash report count per group reached.');
                return false;
            }
        }
		
		// Save uploaded file to its persistent location.
		if(!$this->saveFileAttachment())
			return false;
						
		// Set received timestamp.
		$this->received = time();
		
		// Set IP address
		$this->ipaddress = Yii::app()->request->userHostAddress;
		
		// Set crash report status.
		$this->status = self::STATUS_PENDING_PROCESSING; 
		
		// Success.
		return true;
	}
	
	/**
	 * This method removes all threads, modules, fileItems, customProps,
	 * processingErrors associated with this crash report.
	 * @return boolean true on success; false on failure.
	 */
	public function deleteAssociatedRecords()
	{
		// Begin DB transaction
		$transaction = Yii::app()->db->beginTransaction();		
		
		try 
		{
			// Remove all related threads
			foreach($this->threads as $thread)
			{
				$thread->delete();
			}
			
			// Remove all related modules
			foreach($this->modules as $module)
			{
				$module->delete();
			}
			
			// Remove all related file items
			foreach($this->fileItems as $fileItem)
			{
				$fileItem->delete();
			}
			
			// Remove all related custom props
			foreach($this->customProps as $customProp)
			{
				$customProp->delete();
			}
			
			// Remove all related processing errors
			foreach($this->processingErrors as $processingError)
			{
				$processingError->delete();
			}			
			
			// Commit transaction
			$transaction->commit();
		}
		catch(Exception $e)
		{
			// Rollback transaction
			$transaction->rollback();
			
			// Add a message to log
			Yii::log($e->getMessage(), 'error');
			
			return false;
		}
		
		return true;
	}
	
	/**
	 * This method is executed before AR is deleted from database.
	 * @return boolean True on success.
	 */
	protected function beforeDelete()
	{
		if(!parent::beforeDelete())	
			return false;
		
		if(!$this->deleteAssociatedRecords())
			return false;
		
		// Get local file path to crash report file
		$fileName = $this->getLocalFilePath();
		
		// Get parent directory names
		$dirName = dirname($fileName);
		$outerDirName = dirname($dirName);
		
		// Remove crash report file
		@unlink($fileName);
		
		// Try to delete file owning directory (if it is empty)
		@rmdir($dirName);
		
		// Try to delete outer directory (if it is empty)
		@rmdir($outerDirName);
		
		return true;
		
	}
	
	/**
	 * Returns crash group title for this crash report.
     * @param string $md5 On output, returns MD5 hash for exception stack trace.
	 * @return string Crash group title string.  
	 */
	public function getCrashGroupTitle(&$md5)
	{
		$title = "";
		$hash = "";
				
		// Get project name
		$proj = Project::model()->findByPk($this->project_id);
		if($proj==null)
		{
			$title = 'Unknown Project';
		}
		else if($this->status==self::STATUS_INVALID)
		{
			$title = 'Invalid Reports';
		}
		else if($this->status!=self::STATUS_PROCESSED)
		{
			$title = 'Unsorted Reports';
		}
        else // Processed
        {
            // Check if there is CollectionId property
            if(isset($this->customProps))
            {
                foreach($this->customProps as $prop)
                {
                    if($prop->name=='CollectionId')
                    {
                        // Use property value as collection name
                        $title = $prop->value;
                    }
                }
            }
            
            // If title is still empty, and there is no exception info...
            if(strlen($title)==0 && $this->exception_thread_id==0)
            {
                $title = 'Reports without Exception Info';
            }
                        
            // If title is still empty...
            if(strlen($title)==0)
            {
                // Find exception thread
                $exceptionThread = null;
                foreach($this->threads as $thread)
                {
                    if($thread->thread_id==$this->exception_thread_id)
                    {
                        $exceptionThread = $thread;
                        Yii::log('Exception thread found '.$thread->thread_id, 'info');
                        break;
                    }
                }

                // If threre is an exception thread and it has stack frames
                if($exceptionThread!=null && isset($exceptionThread->stack_trace_md5))
                {
                    // Use the topmost stack frame as title
                    $title = $exceptionThread->getExceptionStackFrameTitle();
                    Yii::log('Exception stack frame title = '.$title, 'info');
                    if(strlen($title)!=0)
                        $hash = $exceptionThread->stack_trace_md5;
                }		               		
            }
            
            // If title is still not determined...
            if(strlen($title)==0)
            {
                // If there is an exception module...
                if(isset($this->exceptionmodule) && isset($this->exceptionmodulebase))		
                {
                    // Get module name
                    $shortModuleName = $this->exceptionmodule;
                    // Find last back-slash in path
                    $pos = strrchr($this->exceptionmodule, "\\");
                    if($pos!=false)
                        $shortModuleName = substr($pos, 1);
                    // Get offset in module
                    $offsetInModule = $this->exceptionaddress-$this->exceptionmodulebase;					
                    // Format title as module!+0xN
                    $title = sprintf("%s!+0x%x", $shortModuleName, $offsetInModule);
                }
                else
                {
                    // In other cases, move this to 'Reports without Exception Info' collection
                    $title = 'Reports without Exception Info';
                }
            }
        }        
		
        // Ensure title is not too long
        $title = MiscHelpers::addEllipsis($title, 200);
        
		if(strlen($hash)==0)
			$hash = md5($title);

		$md5 = $hash;

		return $title;
	}
    
    /**
     * Looks for exception thread model for this crash report. 
     */
    public function getExceptionThread()
    {
        // Find exception thread
        $exceptionThread = null;
        foreach($this->threads as $thread)
        {
            if($thread->thread_id==$this->exception_thread_id)
            {
                $exceptionThread = $thread;                
                break;
            }
        }
        
        return $exceptionThread;
    }
	
	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
			'project'=>array(self::BELONGS_TO, 'Project', 'project_id'),			
			'fileItems'=>array(self::HAS_MANY, 'FileItem', 'crashreport_id'),
			'threads'=>array(self::HAS_MANY, 'Thread', 'crashreport_id'),
			'modules'=>array(self::HAS_MANY, 'Module', 'crashreport_id'),
			'customProps'=>array(self::HAS_MANY, 'CustomProp', 'crashreport_id'),
			'collection'=>array(self::BELONGS_TO, 'CrashGroup', 'groupid'),
			'bugs'=>array(self::HAS_MANY, 'BugCrashReportMapping', 'crashreport_id'),			
			'appVersion'=>array(self::BELONGS_TO, 'AppVersion', 'appversion_id'),			
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'fileAttachment'=>'File Attachment',
			'id' => 'ID',
			'srcfilename'=>'Report File Name',			
			'filesize'=>'Size',
			'date_created'=>'Date Created',
			'received' => 'Date Received',
			'status'=>'Status',
			'ipaddress'=>'IP Address',
			'md5'=>'MD5 Hash',
			'groupid'=>'Collection',
			'crashguid'=>'Crash GUID',			
			'project_id'=>'Project',
			'appversion_id'=>'Project Version',
			'emailfrom' =>'E-mail',
			'description'=>'Description',						
			'crashrptver'=>'Generator Version',
			'exception_type'=>'Exception Type',
			'exception_code'=>'SEH Exception Code',
			'exceptionaddress'=>'Exception Address',
			'exceptionmodule'=>'Exception Module',
			'exceptionmodulebase'=>'Exception Module Base',
			'exe_image'=>'Image Path',
			'os_name_reg'=>'Operating System',
			'os_ver_mdmp'=>'OS Version',
			'os_is_64bit'=>'OS Bitness',
			'geo_location'=>'Geographic Location',
			'product_type'=>'Machine Type',
			'cpu_architecture'=>'CPU Architecture',
			'cpu_count'=>'CPU Count',
			'gui_resource_count'=>'GUI Resource Count',			
			'open_handle_count'=>'Open Handle Count',
			'memory_usage_kbytes'=>'Memory Usage',
		);
	}

	/**
	 * Retrieves a list of models based on the current search/filter conditions.
	 * @return CActiveDataProvider the data provider that can return the models based on the search/filter conditions.
	 */
	public function search()
	{
		// Validate model
		$this->scenario = 'search';
		if(!$this->validate())
			return Null;
		
		// Check that current project is selected.
		$curProjectId = Yii::app()->user->getCurProjectId();
		if($curProjectId==false)
			return Null;
		
		// Create empty search criteria
		$criteria=new CDbCriteria;        

        // Get current project ID and version
		$curProjectId = Yii::app()->user->getCurProjectId();
		$criteria->compare('project_id', $curProjectId, false, 'AND');
		$curProjectVer = false;
		Yii::app()->user->getCurProjectVersions($curProjectVer);
		if($curProjectVer!=false && $curProjectVer!=-1)
		{
			$criteria->compare('appversion_id', $curProjectVer, false, 'AND');
		}		
        
        if(isset($this->groupid))
		{
			$criteria->compare('groupid', $this->groupid, false, 'AND');
		}
		        
		// Simple search filter
		if(!$this->isAdvancedSearch && isset($this->filter))
		{
            $criteria2 = new CDbCriteria;
			$criteria2->compare('ipaddress', $this->filter, true, 'OR');
			$criteria2->compare('emailfrom', $this->filter, true, 'OR');
			$criteria2->compare('description', $this->filter, true, 'OR');
            $criteria->mergeWith($criteria2);
		}
		else if($this->isAdvancedSearch) // Advanced search
		{				
			// Fill search criteria
			if(isset($this->ipaddress))
				$criteria->compare('ipaddress', $this->ipaddress, true, 'AND');
			if(isset($this->emailfrom))
				$criteria->compare('emailfrom', $this->emailfrom, true, 'AND');			
			if($this->status!=-1)
				$criteria->compare('status', $this->status, false, 'AND');			
			if($this->receivedFrom!="" & $this->receivedTo!="")
				$criteria->addBetweenCondition('received', 
						$this->strToDate($this->receivedFrom), 
						$this->strToDate($this->receivedTo), 'AND');						
		}
						
		// Perform search
		$dataProvider = new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
            'pagination'=>array(
                'pageSize'=>50
            ),
            'sort'=>array(
                // Sort crash reports by receive date in descendent order
                'defaultOrder'=>'received DESC'
            )
		));
				
		return $dataProvider;
	}
	
	/**
	 * This method looks in {{fileitem}} table for file items belonging this
	 * crash report and returns the list of models.
	 * @return \CActiveDataProvider 
	 */
	public function searchFileItems()
	{
		$criteria=new CDbCriteria;
		$criteria->compare('crashreport_id', $this->id);								
		
		$dataProvider = new CActiveDataProvider('FileItem', array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'filename ASC'
            ),
            'pagination'=>false
		));
                
        return $dataProvider;
	}	
    
    /**
	 * This method looks in {{fileitem}} table for screenshot file items 
     * (screenshot%.jpg, screenshot%.png) belonging this
	 * crash report and returns the list of models.
	 * @return \CActiveDataProvider 
	 */
    public function searchScreenshots()
    {
        $criteria=new CDbCriteria;
        $criteria->compare('crashreport_id', $this->id);										
        $criteria->condition .= ' AND (filename LIKE \'screenshot%.jpg\' OR filename LIKE \'screenshot%.png\')';        
        
		$dataProvider = new CActiveDataProvider('FileItem', array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'filename ASC'
            ),
            'pagination'=>false
		));
                
        return $dataProvider;
    }
    
    /**
	 * This method looks in {{fileitem}} table for screenshot file items 
     * (video%.ogg) belonging this
	 * crash report and returns the list of models.
	 * @return \CActiveDataProvider 
	 */
    public function searchVideos()
    {
        $criteria=new CDbCriteria;        
        $criteria->compare('crashreport_id', $this->id);										
		$criteria->addSearchCondition('filename', 'video%.ogg', false);        
        
		$dataProvider = new CActiveDataProvider('FileItem', array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'filename ASC'
            ),
            'pagination'=>false
		));
                
        return $dataProvider;
    }
	
	/**
	 * This method looks in {{customprop}} table for custom props belonging this
	 * crash report and returns the list of models.
	 * @return \CActiveDataProvider 
	 */
	public function searchCustomProps()
	{
		$criteria=new CDbCriteria;
		$criteria->compare('crashreport_id', $this->id);								
		
		$dataProvider = new CActiveDataProvider('CustomProp', array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'name ASC'
            ),
            'pagination'=>false
		));
                
        return $dataProvider;
	}	
	
	/**
	 * This method looks in {{module}} table for modules belonging this
	 * crash report and returns the list of models.
	 * @return \CActiveDataProvider 
	 */
	public function searchModules()
	{
		$criteria=new CDbCriteria;
		$criteria->compare('crashreport_id', $this->id);								
		
		$dataProvider =  new CActiveDataProvider('Module', array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'sym_load_status DESC, name ASC'
            ),
            'pagination'=>false
		));
		        
		return $dataProvider;
	}
	
	/**
	 * This method looks in {{thread}} table for threads belonging this
	 * crash report and returns the list of models.
	 * @return \CActiveDataProvider 
	 */
	public function searchThreads()
	{
		$criteria=new CDbCriteria;
		$criteria->compare('crashreport_id', $this->id);								
		$criteria->order = 'thread_id ASC';        
        
		$dataProvider = new CActiveDataProvider('Thread', array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'thread_id ASC'
            ),
            'pagination'=>false
		));
		
		return $dataProvider;
	}	
	
	/**
	 *  This method dumps the content of attachment file to stdout.
	 *  This method is used when downloading the debug info file.
	 */
	public function dumpFileAttachmentContent($file=null)
	{
		// Determine path to local file for download
		$fileName = $this->getLocalFilePath();
		
		if($file!=null)
		{
			$fout = @fopen($file, 'w');
			if($fout==false)
				throw new CHttpException(403, 'Invalid file');
		}
		
		// Try to open file
		if ($fd = fopen ($fileName, "r")) 
		{			
			$fsize = filesize($fileName);			
			
			// Write HTTP headers
			header("Content-type: application/octet-stream");
			header("Content-Disposition: filename=\"".$this->srcfilename."\"");
    		header("Content-length: $fsize");
			header("Cache-control: private"); //use this to open files directly
			
			// Write file content
			while(!feof($fd)) 
			{
				$buffer = fread($fd, 2048);
				if($file==null)
					echo $buffer;
				else
					fwrite($fout, $buffer);
			}			
		}	
		
		fclose($fd);
		
		// Close out file
		if($file!=null)
			fclose($fout);
	}
	
	/**
	 * Extracts a file item from crash report to a temporary location and 
	 * returns file name, or False on error.
	 * @param string $itemName File item name.
	 * @return string file name.
	 */
	public function extractFileItem($itemName)
	{
		// Determine path to local crash report file
		$crashReportFileName = $this->getLocalFilePath();
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "ext");				
			
		// Format daemon command 
		$command = 'dumper --extract-file "'.$crashReportFileName.'" "'.$itemName.'" "'.$outFile.'"';
		
		// Execute the command
		$responce = "";
		$retCode = Yii::app()->daemon->execCommand($command, $responce);
				
		if($retCode!=0)
		{
			Yii::log('Error executing command '.$command.', responce = '.$responce, 'error');
			
			// Remove temp file
			@unlink($outFile);
			return False;
		}
		
		return $outFile;
	}
	
	/**
	 * This method dumps the content of a given zipped file to stdout or to a file.
	 * This method is used when downloading the crash report file contents.
	 * @param $itemName Name of file item.
	 * @param $asOctetStream If true, dumps as application/octet stream.
	 * @param $file File name; can be Null.
	 * @throw CHttpException
	 * @return void
	 */
	public function dumpFileItemContent($itemName, $asOctetStream=true, $file=null)
	{
		if($file!=null)
		{
			$fout = @fopen($file, 'w');
			if($fout==false)
				throw new CHttpException(403, 'Invalid output file.');
		}
		
		// Extract file item to a temp file
		$tmpfile = $this->extractFileItem($itemName);
		if($tmpfile==False)
			throw new CHttpException(403, 'File does not exists.');
				
		// Try to open file
		if ($fd = fopen ($tmpfile, "r")) 
		{			
			$fsize = filesize($tmpfile);			
			
			$mimeType='application/octet-stream';
			if(!$asOctetStream)
			{
				$path_parts = pathinfo($itemName);
				$ext = $path_parts['extension'];
				if(strcasecmp($ext,'jpg')==0)
					$mimeType = 'image/jpg';
				else if(strcasecmp($ext,'png')==0)
					$mimeType = 'image/png';
                else if(strcasecmp($ext,'ogg')==0)
					$mimeType = 'video/ogg';
			}
			
			// Write HTTP headers
			header("Content-type: $mimeType");
			header("Content-Disposition: filename=\"".$itemName."\"");
    		header("Content-length: $fsize");
			header("Cache-control: private"); //use this to open files directly
			
			// Write file content
			while(!feof($fd)) 
			{
				$buffer = fread($fd, 2048);
				if($file==null)
					echo $buffer;
				else
					fwrite($fout, $buffer);
			}
			
			// Close file
			fclose($fd);
		}	
		else
			throw new CHttpException(403, 'File does not exist.');
		
		if($file!=null)
			fclose($fout);
		
		// Remove temp file
		@unlink($tmpfile);
	}
	
	/**
	 * This method dumps a screenshot thumbnail image to stdout or to a file.
	 * @param string $itemName File item name.
	 */
	public function dumpScreenshotThumbnail($itemName, $file=null)
	{
		// Extract file item to a temp file
		$tmpfile = $this->extractFileItem($itemName);
		if($tmpfile==False)
			throw new CHttpException(403, 'File does not exists.');
				
		$src_img=null;
		$path_parts = pathinfo($itemName);
		$ext = $path_parts['extension'];
		if(strcasecmp($ext,'jpg')==0)
			$src_img = imagecreatefromjpeg($tmpfile);
		else if(strcasecmp($ext,'png')==0)
			$src_img = imagecreatefrompng($tmpfile);
		
		if($src_img===null)
			throw new CHttpException(403, 'Invalid file.');

		$thumb_w = 220;
		$thumb_h = 190;
		$dst_w = 0;
		$dst_h = 0;
		$dst_x = 0;
		$dst_y = 0;
		$src_w=imageSX($src_img);
		$src_h=imageSY($src_img);
		$src_ratio = $src_w/$src_h;
		$dst_ratio = $thumb_w/$thumb_h;
		if($dst_ratio > $src_ratio) 
		{
			$dst_h=$thumb_h;
			$dst_w=$dst_h*$old_ratio;
			$dst_x = $thumb_w/2-$dst_w/2;
		}
		else 
		{
			$dst_w=$thumb_w;
			$dst_h=$dst_w/$src_ratio;
			$dst_y = $thumb_h/2-$dst_h/2;
		}
		
		$dst_img=ImageCreateTrueColor($thumb_w,$thumb_h);
		imagecopyresampled($dst_img,$src_img,$dst_x,$dst_y,0,0,$dst_w,$dst_h,$src_w,$src_h); 

		imagejpeg($dst_img, $tmpfile); 
		imagedestroy($dst_img); 
		imagedestroy($src_img); 
		
		// Open out file
		if($file!=null)
		{
			$fout = @fopen($file, 'w');
			if($fout==false)
			{
				@unlink($tmpfile);
				throw new CHttpException(403, 'Invalid file.');
			}
		}
		
		// Try to open file
		if ($fd = fopen ($tmpfile, "r")) 
		{			
			$fsize = filesize($tmpfile);			
			
			$path_parts = pathinfo($itemName);
			$ext = $path_parts['extension'];
			if(strcasecmp($ext,'jpg')==0)
				$mimeType = 'image/jpg';
			else if(strcasecmp($ext,'png')==0)
				$mimeType = 'image/png';
			
			// Write HTTP headers
			header("Content-type: $mimeType");
			header("Content-Disposition: filename=\"".$itemName."\"");
    		header("Content-length: $fsize");
			header("Cache-control: private"); //use this to open files directly
						
			// Write file content
			while(!feof($fd)) 
			{
				$buffer = fread($fd, 2048);
				
				if($file!=null)
					fwrite($fout, $buffer);
				else
					echo $buffer;
			}
			
			// Close file
			fclose($fd);
		}	
		else
			throw new CHttpException(403, 'File does not exist.');
		
		if($file!=null)
			fclose($fout);
		
		// Remove temp file
		@unlink($tmpfile);
	}
	
	/**
	 * Converts CrashRpt version (e.g. 1300) to string (e.g. 1.3.0).
	 * @param type $ver 
	 */
	public static function generatorVersionToStr($ver)
	{
		if(strlen($ver)!=4)
			return null;
		
		$verMajor = substr($ver, 0, 1);
		$verMinor = substr($ver, 1, 1);
		$verBuild = (int)substr($ver, 2, 2);
		return $verMajor.'.'.$verMinor.'.'.$verBuild;
	}
	
	/**
	 * Returns OS bittness tring, e.g. '64-bit'
	 */
	public function getOsBittnessStr()
	{
		if(!isset($this->os_is_64bit))
			return null;
		else if($this->os_is_64bit==1)
			return '64-bit';
		else
			return '32-bit';
	}
	
	public static function geoIdToCountryName($geoId)
	{
		if(strlen($geoId)==0)
			return null;
		
		$lang = substr($geoId, 0, 2);
		$country = substr($geoId, 3, 2);
		
		$countryName = MiscHelpers::countryCodeToCountryName($country);
		return "$countryName ($geoId)";
	}
	
	/**
	 * Generates a crash report upload statistics graph for currently 
	 * selected project and dumps it to stdout.
	 * @param integer $w Desired image width.
	 * @param integer $h Desired image height.
	 * @param integer $period Desired time period (7, 30 or 365).
	 * @throws CHttpException
	 * @return void
	 */
	public static function generateUploadStatisticsGraph($w, $h, $period, $file = null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($period) || $period<=0 || $period>365)
			throw new CHttpException(403, 'Invalid parameter');
			
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
		$curVer = Project::PROJ_VER_NOT_SET;
		$versions = Yii::app()->user->getCurProjectVersions($curVer);
				
		// Prepare data
		$data = array();
		
		$tomorrow  = mktime(0, 0, 0, date("m") , date("d")+1, date("Y"));		
		
		$finishDate = $tomorrow-1;		
						
		$curDate = $finishDate;
		$dateFrom = $curDate;
		while($finishDate-$curDate<$period*24*60*60)
		{			
			// Calc the beginning of time interval
			if($period>30)
				$dateFrom  = mktime(0, 0, 0, date("m", $curDate)-1 , date("d", $curDate), date("Y", $curDate));
			else if($period>7)
				$dateFrom  = mktime(0, 0, 0, date("m", $curDate) , date("d", $curDate)-6, date("Y", $curDate));
			else
				$dateFrom  = mktime(0, 0, 0, date("m", $curDate) , date("d", $curDate), date("Y", $curDate));
						
			// Get count of crash reports received within the period
			$criteria = new CDbCriteria();			
			$criteria->compare('project_id', $curProjectId);
			if($curVer!=Project::PROJ_VER_ALL) // not '(all)'
			{
				$criteria->compare('appversion_id', $curVer);		
			}
			$criteria->addBetweenCondition('received', $dateFrom, $curDate);			
			$count = CrashReport::model()->count($criteria);
			
			// Add an item to data
			$item = array($period>30?date('M y', $curDate):date('j M', $curDate)=>$count);
			$data = $item + $data;
			
			// Next time interval
			$curDate = $dateFrom-1;
		}
				
		$graph = new ezcGraphLineChart();		
		$graph->palette = new ezcGraphPaletteEzBlue();
		
		$graph->data['Versions'] = new ezcGraphArrayDataSet($data);
		
        $majorStep = round(max($data));
        if($majorStep==0)
            $majorStep = 1;
		$graph->yAxis->majorStep = $majorStep;
		$graph->yAxis->minorStep = $graph->yAxis->majorStep/5;		
		$graph->xAxis->labelCount = 30;
		/*$graph->xAxis = new ezcGraphChartElementDateAxis();
		$graph->xAxis->dateFormat = 'M';
		$graph->xAxis->endDate = $finishDate;
		$graph->xAxis->startDate = $dateFrom;
		$graph->xAxis->interval = ezcGraphChartElementDateAxis::MONTH;*/
				
		//$graph->data['Versions']->highlight = true;
		$graph->options->fillLines = 210;
		
		$graph->legend = false;
		$graph->legend->position = ezcGraph::RIGHT;		
		$graph->options->font->name = 'Tahoma';
        $graph->options->font->maxFontSize = 12;
        $graph->options->font->minFontSize = 1;
				
		if($file===null)
			$graph->renderToOutput($w, $h);
		else
			$graph->render($w, $h, $file);
	}
	
	/**
	 * Generates a crash report version distribution graph for currently 
	 * selected project and dumps it to stdout.
	 * @param type $w Desired image width.
	 * @param type $h Desired image height.
	 * @throws CHttpException
	 * @return void
	 */
	public static function generateVersionDistributionGraph($w, $h, $file=null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
		if($curProjectId==false)
			throw new CHttpException(403, 'Invalid parameter');
		
		// Prepare data
		$criteria = new CDbCriteria();
		$criteria->select = 'appversion_id, COUNT(*) as cnt';
		$criteria->group = 'appversion_id';
		$criteria->compare('t.project_id', $curProjectId);		
		$criteria->order = 'cnt DESC';		
		$criteria->with = 'appVersion';
		
		$curVer = '(not set)';
		$versions = Yii::app()->user->getCurProjectVersions($curVer);
		
		$data = array();
		$models = CrashReport::model()->findAll($criteria);
                
		$totalReportCount = 0;			
		$curVerStr = '';
		foreach($models as $model)
		{
			$totalReportCount += $model->cnt;			            
		}
        
        $itemCount=0;
        $others=0;
        foreach($models as $model)
		{			
            $itemCount++;
            if($itemCount>10)
            {
                $others += $model->cnt;
                continue;
            }
            
            $percent = 100*$model->cnt/$totalReportCount;
            $label = sprintf('(%0.1f%%) %s', $percent, $model->appVersion->version);
            $label = MiscHelpers::addEllipsis($label, 60);
            
            if(array_key_exists($label, $data))
                $data[$label] += $model->cnt;
            else
                $data[$label] = $model->cnt;					
			if($curVer==$model->appversion_id)
				$curVerStr = $label;
		}
        
        if($others>0)
        {
            // Add 'Others' 
            $percent = 100*$others/$totalReportCount;
            $label = sprintf('(%0.1f%%) Others', $percent);                       
            $data[$label] = $others;            
        }
		        
		// Check the case when $data is empty.
		if(count($data)==0)
		{			
			// Open out file
			if($file!=null)
			{
				$fout = @fopen($file, 'w');
				if($fout==false)
				{
					@unlink($tmpfile);
					throw new CHttpException(403, 'Invalid file.');
				}
			}
			
			// No data available
			$fileName = Yii::app()->basePath.'/../images/no_data_available.png';
			if ($fd = @fopen ($fileName, "r")) 
			{			
				$fsize = filesize($fileName);			

				// Write HTTP headers
				header("Content-type: image/png");
				//header("Content-Disposition: filename=\"".$fileName."\"");
				header("Content-length: $fsize");
				header("Cache-control: private"); //use this to open files directly

				// Write file content
				while(!feof($fd)) 
				{
					$buffer = fread($fd, 2048);
					if($file==null)
						echo $buffer;
					else
						fwrite($fout, $buffer);
				}	
				
				if($file!=null)
					fclose($fout);
				
				fclose($fd);
			}				
			return;
			
		}
		
		$graph = new ezcGraphPieChart();
		$graph->palette = new ezcGraphPaletteEzBlue();	

		$graph->data['Versions'] = new ezcGraphArrayDataSet($data);
		
		$graph->legend = true;
		$graph->legend->position = ezcGraph::RIGHT;
        $graph->legend->landscapeSize = .5;
        $graph->legend->portraitSize = .5;		
		if($curVerStr!='')
			$graph->data['Versions']->highlight[$curVerStr] = true;		
		$graph->options->font->name = 'Tahoma';		
		$graph->options->sum = $totalReportCount;		
		//$graph->options->summarizeCaption = 'Others';		
        $graph->options->label = '%3$.1f%%';
        $graph->options->font->name = 'Tahoma';		
        $graph->options->font->maxFontSize = 14;
        $graph->options->font->minFontSize = 1;
        		
		$graph->renderer = new ezcGraphRenderer3d();
		$graph->renderer->options->pieChartRotation = .8;
		$graph->renderer->options->pieChartShadowSize = 10;
        //$graph->renderer->options->pieChartGleam = .3;
		//$graph->renderer->options->pieChartGleamColor = '#FFFFFF';
		//$graph->renderer->options->pieChartGleamBorder = 2;
		

		if($file===null)
			$graph->renderToOutput($w, $h);
		else
			$graph->render($w, $h, $file);
	}
    
    /**
	 * Generates a crash report OS version distribution graph for currently 
	 * selected project and dumps it to stdout.
	 * @param type $w Desired image width.
	 * @param type $h Desired image height.
	 * @throws CHttpException
	 * @return void
	 */
	public static function generateOSVersionDistributionGraph($w, $h, $file=null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
		if($curProjectId==false)
			throw new CHttpException(403, 'Invalid parameter');
                
		// Prepare data
		$criteria = new CDbCriteria();
		$criteria->select = 'os_name_reg, COUNT(*) as cnt';
        $criteria->order = 'cnt DESC';
		$criteria->group = 'os_name_reg';        
		$criteria->compare('t.project_id', $curProjectId);		
        
		$curVer = '(not set)';
		$versions = Yii::app()->user->getCurProjectVersions($curVer);
		
        if($curVer!=-1) // -1 means '<all>'
        {
            $criteria->compare('appversion_id', $curVer==0?null:$curVer);		
        }		
		
		$data = array();
		$models = CrashReport::model()->findAll($criteria);
		$totalCount = 0;					
		foreach($models as $model)
		{
			$totalCount += $model->cnt;			            
		}
        
        $itemCount=0;
        $others=0;
        foreach($models as $model)
		{			
            $itemCount++;
            if($itemCount>10)
            {
                $others += $model->cnt;
                continue;
            }
            
            $osName = strlen($model->os_name_reg)!=0?$model->os_name_reg:"(not set)";
            $percent = 100*$model->cnt/$totalCount;
            $label = sprintf('(%0.1f%%) %s', $percent, $osName);
            $label = MiscHelpers::addEllipsis($label, 60);
            if(array_key_exists($label, $data))
                $data[$label] += $model->cnt;
            else
                $data[$label] = $model->cnt;			
		}
        
        if($others>0)
        {
            // Add 'Others' 
            $percent = 100*$others/$totalCount;
            $label = sprintf('(%0.1f%%) Others', $percent);                       
            $data[$label] = $others;            
        }
        
        // Check the case when $data is empty.
		if(count($data)==0)
		{			
			// Open out file
			if($file!=null)
			{
				$fout = @fopen($file, 'w');
				if($fout==false)
				{
					@unlink($tmpfile);
					throw new CHttpException(403, 'Invalid file.');
				}
			}
			
			// No data available
			$fileName = Yii::app()->basePath.'/../images/no_data_available.png';
			if ($fd = @fopen ($fileName, "r")) 
			{			
				$fsize = filesize($fileName);			

				// Write HTTP headers
				header("Content-type: image/png");
				//header("Content-Disposition: filename=\"".$fileName."\"");
				header("Content-length: $fsize");
				header("Cache-control: private"); //use this to open files directly

				// Write file content
				while(!feof($fd)) 
				{
					$buffer = fread($fd, 2048);
					if($file==null)
						echo $buffer;
					else
						fwrite($fout, $buffer);
				}	
				
				if($file!=null)
					fclose($fout);
				
				fclose($fd);
			}				
			return;
			
		}
		
		$graph = new ezcGraphPieChart();
		$graph->palette = new ezcGraphPaletteEzGreen();	

		$graph->data['Versions'] = new ezcGraphArrayDataSet($data);
		
		$graph->legend = true;
		$graph->legend->position = ezcGraph::RIGHT;
        $graph->legend->landscapeSize = .5;
        $graph->legend->portraitSize = .5;
		$graph->options->sum = $totalCount;		
		//$graph->options->summarizeCaption = 'Others';
        $graph->options->label = '%3$.1f%%';
        $graph->options->font->name = 'Tahoma';		
        $graph->options->font->maxFontSize = 14;
        $graph->options->font->minFontSize = 1;
				
		$graph->renderer = new ezcGraphRenderer3d();
		$graph->renderer->options->pieChartRotation = .8;
		$graph->renderer->options->pieChartShadowSize = 10;
        //$graph->renderer->options->pieChartGleam = .3;
		//$graph->renderer->options->pieChartGleamColor = '#FFFFFF';
		//$graph->renderer->options->pieChartGleamBorder = 2;
        
		if($file===null)
			$graph->renderToOutput($w, $h);
		else
			$graph->render($w, $h, $file);
	}
    
    
    /**
	 * Generates a crash report geographic location distribution graph for currently 
	 * selected project and dumps it to stdout.
	 * @param type $w Desired image width.
	 * @param type $h Desired image height.
	 * @throws CHttpException
	 * @return void
	 */
	public static function generateGeoLocationDistributionGraph($w, $h, $file=null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
		if($curProjectId==false)
			throw new CHttpException(403, 'Invalid parameter');
                
		// Prepare data
		$criteria = new CDbCriteria();
		$criteria->select = 'geo_location, COUNT(*) as cnt';
		$criteria->group = 'geo_location';
		$criteria->compare('t.project_id', $curProjectId);		
        $criteria->order = 'cnt DESC';
		
        $curVer = '(not set)';
		$versions = Yii::app()->user->getCurProjectVersions($curVer);
                
        if($curVer!=-1) // -1 means '<all>'
        {
            $criteria->compare('appversion_id', $curVer==0?null:$curVer);		
        }		
		
		$data = array();
		$models = CrashReport::model()->findAll($criteria);
		$totalCount = 0;					
		foreach($models as $model)
		{
			$totalCount += $model->cnt;		
            if($model->geo_location===null)
            {
                if(isset($data['']))
                    $data[''] += $model->cnt;
                else
                    $data[''] = $model->cnt;
            }
            else
            {
                $data[$model->geo_location] = $model->cnt;			            
            }
		}
		        
        if($file==null)
            $fout = fopen('php://output', 'w');
        else
            $fout = fopen($file, 'wt');
                
        fprintf($fout, '<div class="digest-pane-image" style="width:'.$w.'px;min-height:'.$h.'px;max-height:'.$h.'px;overflow:hidden"><table id="geo-locations">');
        // Check the case when $data is empty.
		if(count($data)==0)
		{			
            fprintf($fout, '<tr><td><i>No data available</i></td></tr>');          
        }
        else
        {		
            foreach($data as $name=>$val)
            {
                $percent = 100*$val/$totalCount;
                $percentStr = sprintf('%0.1f', $percent);
                $nameStr = '(not set)';
                if($name!='')
                    $nameStr = MiscHelpers::addEllipsis(CrashReport::geoIdToCountryName($name), 30);
                fprintf($fout, '<tr><td>'.$nameStr.'</td><td>'.$percentStr.'%%</td></tr>');
            }	
            
        }
        fprintf($fout, '</table></div>');
        if($file!=null)
            fclose($fout);
	}
	
	/**
	 * Determines if a bug can be opened for this crash report.
	 * @return boolean true if a bug can be opened.
	 */
	public function canOpenNewBug()
	{		
		if($this->status!=CrashReport::STATUS_PROCESSED)
			return false;
				
		return true;
	}
	
	/**
	 * Returns the list of processing errors associated with this crash report. 
	 */
	public function getProcessingErrors()
	{
		$criteria = new CDbCriteria();
        $criteria->compare('srcid', $this->id);
		$criteria->compare('type', ProcessingError::TYPE_CRASH_REPORT_ERROR);		
		$models = ProcessingError::model()->findAll($criteria);
		return $models;
	}
	
	/**
	 * This method determines if the status of crash report can be resetted to
	 * Waiting.
	 */
	public function canResetStatus()
	{
		if($this->status==self::STATUS_PROCESSED || 
		   $this->status==self::STATUS_INVALID)
		{
			// OK
			return true;
		}
		
		// Can't reset status
		return false;
	}
	
	/**
	 * This function resets crash report's status to Waiting.     
	 */
	public function resetStatus()
	{
		if(!$this->canResetStatus())
			throw new CHttpException('Unexpected error.');
		
		$this->status = self::STATUS_PENDING_PROCESSING;
		
		if(!$this->save())
			throw new CHttpException('Unexpected error.');
	}
}
