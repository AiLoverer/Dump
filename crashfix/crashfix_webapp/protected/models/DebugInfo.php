<?php

/**
 * This is the model class for table "{{debuginfo}}".
 *
 */
class DebugInfo extends CActiveRecord
{	
	// Debug info file status
	const STATUS_PENDING_PROCESSING     = 1; // The debug info file is awaiting for processing by daemon.
	const STATUS_PROCESSING_IN_PROGRESS = 2; // The debug info file is currently being processed by daemon.
	const STATUS_PROCESSED              = 3; // The debug info file has been processed by daemon and everything is OK.
	const STATUS_INVALID                = 4; // The debug info file is marked by daemon as an invalid PDB file.
    const STATUS_PENDING_DELETE         = 5; // The debug info file is awaiting for deletion.
    const STATUS_DELETE_IN_PROGRESS     = 6; // The debug info file is being deleted.
			
	public $project_name; // Project name (used for external upload).
	public $fileAttachment; // File attachment CUploadedFile	
    public $fileAttachmentIsUploaded = true;    // Set to TRUE if attachment was uploaded; or set to false if it resides in local filesystem.
	public $ignoreFileAttachmentErrors = false; // Used for tests
	
	// Search-related attributes.
	public $filter; // Simple search filter
	public $isAdvancedSearch = false; // Advanced search flag
	public $dateFrom; // Date uploaded from
	public $dateTo; // Date uploaded to
	
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
		return '{{debuginfo}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(			
			array('guid', 'required', 'on'=>'create, update'),						
			array('guid', 'checkFileGUIDExists', 'on'=>'create'),
			array('project_name', 'length', 'max'=>64),									
			array('fileAttachment', 'file', 'types' => 'pdb', 'maxFiles'=>1, 'minSize'=>1, 'on'=>'create'),
			array('dateFrom', 'date', 'on'=>'search'),			
			array('dateTo', 'date', 'on'=>'search'),	
			array('dateTo', 'compareFromToDates', 'on'=>'search'),			
			array('guid, filename', 'safe', 'on'=>'search'),			
		);
	}
	
    /**
     * Returns the list of statuses.
     * @return array list of statuses. 
     */
    public function getStatusList() 
    {
        $list[self::STATUS_PENDING_PROCESSING] = 'Waiting';
        $list[self::STATUS_PROCESSING_IN_PROGRESS] = 'Processing';
        $list[self::STATUS_PROCESSED] = 'Ready';
        $list[self::STATUS_INVALID] = 'Invalid';
        $list[self::STATUS_PENDING_DELETE] = 'Pending Delete';                
        $list[self::STATUS_DELETE_IN_PROGRESS] = 'Deleting';                
        return $list;
    }

    /**
     * Returns status string.
     */
    public function getStatusStr() 
    {
        $statuses = $this->getStatusList();
        return $statuses[$this->status];
    }
    
	/**
	 * This validator method is used by rules method and checks if a file with 
	 * such a GUID exists in the database or not.
	 * If file already exits, it adds error message and returns false. If file 
	 * doesn't exist, it returns true.
	 */
	public function checkFileGUIDExists()
	{	
		if(!$this->isNewRecord)		
			return true;
		
        if(isset($this->project_name))
        {
            // Check if such a project name exists
            $project = Project::model()->findByAttributes(
                        array('name'=>$this->project_name));
            if($project===null)
            {
                $this->addError('project_id', 'Invalid project name.');
                return false;
            }
        }
        
		// Check if such a file already exists in the database
		$model = DebugInfo::model()->find(array(
										'condition'=>'guid=:guid',
										'params'=>array(':guid'=>$this->guid,),
										));
										
		if($model!==Null)
		{
			$this->addError('fileAttachment', 'Such a file is already in the database.');
			return false; // Not found such a GUID
		}
		
		// Found
		return true;
	}
	
	/**
	 *  This validator compares Date From and Date To to ensure Date To is the greater one.
	 */
	public function compareFromToDates()
	{		
		$date1 = $this->strToDate($this->dateFrom);
		$date2 = $this->strToDate($this->dateTo);
		
		if($date1==False || $date2==False)
			return true; // One of dates is invalid - nothing to compare
		
		if($date2>=$date1)
			return true; // Date To is greater than Date From
		
		$this->addError('dateTo', 'Date To must be greater than Date From.');
		return false;		
	}
	
	/**
	 *  Converts a date string to timestamp.
	 */
	public function strToDate($date)
	{
		$timestamp=CDateTimeParser::parse($date,'MM/dd/yyyy');
		return $timestamp;
	}
	
	/**
	 *  This method moves uploaded file to its persistent location.
	 *  @return On success, returns true. On failure, sets error message and returns false.
	 */
	public function saveFileAttachment()
	{
        // Validate file attachment
		if(!isset($this->fileAttachment) || $this->fileAttachment===Null)
		{
			$this->addError('fileAttachment', 'File attachment is missing.');			
			return false;
		}
		
		// Set filesize attribute
		$this->filesize = $this->fileAttachment->getSize();
		
		// Get project this debug info file belongs to.
		$project = Project::model()->findByPk($this->project_id);
		if($project===Null)
		{
			$this->addError('project_id', 'Invalid project ID.');			
			return false; 
		}
		
        // Check total debug info size disc quota for project.
        if($project->debug_info_files_disc_quota>0)
        {
            $totalFileSize = 0;
            $percentOfQuota = 0;
            $project->getDebugInfoCount($totalFileSize, $percentOfQuota);
            
            if($project->debug_info_files_disc_quota*1024*1024 < 
                    $totalFileSize+$this->filesize)
            {
                // Quota has exceeded
                $this->addError(
                        'fileAttachment', 
                        'Debug info disc quota for this project has exceeded.');			
                return false; 
            }
        }
		
		// Calc md5 hash and save it as model attribute
		$this->md5 = md5_file($this->fileAttachment->getTempName());

		// Set filename attribute
		$this->filename = $this->fileAttachment->getName();
								
		// Create the directory where we will place the uploaded file
		$dirName = Yii::app()->getRuntimePath()."/debugInfo/".$this->filename."/".$this->guid;
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
            $fileName = $dirName."/".$this->filename;
            if(!$this->fileAttachment->saveAs($fileName, true) && 
                !$this->ignoreFileAttachmentErrors)
            {
                $this->addError("fileAttachment", "Couldn't save data to local storage"); 
                return false;
            }
        }
        else
        {
            // Local file.
            $fileName = $dirName."/".$this->filename;
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
	 *  This method returns the absolute local path to debug info file.	 
	 */
	public function getLocalFilePath()
	{
		// Determine path to local file
		$fileName = $this->filename;
		if($this->status==self::STATUS_PENDING_PROCESSING ||
           $this->status==self::STATUS_PROCESSING_IN_PROGRESS)
            $dirName = Yii::app()->getBasePath()."/runtime/debugInfo/".$this->filename."/".$this->guid;		
        else
			$dirName = Yii::app()->getBasePath()."/data/debugInfo/".$this->filename."/".$this->guid;				
			
		$fileName = $dirName."/".$fileName;
		return $fileName;
	}
	
	/**
	 *  This method dumps the content of attachment file to stdout.
	 *  This method is used when downloading the debug info file.
	 */
	public function dumpFileAttachmentContent($outFile = null)
	{
		// Open out file
		if($outFile!=null)
			$fout = @fopen($outFile, 'w');
		
		// Determine path to local file for download
		$fileName = $this->getLocalFilePath();
		
		// Try to open file
		if ($fd = @fopen ($fileName, "r")) 
		{			
			$fsize = filesize($fileName);
			$path_parts = pathinfo($fileName);			
			
			// Write HTTP headers
			header("Content-type: application/octet-stream");
			header("Content-Disposition: filename=\"".$path_parts["basename"]."\"");
    		header("Content-length: $fsize");
			header("Cache-control: private"); //use this to open files directly
			
			// Write file content
			while(!feof($fd)) 
			{
				$buffer = fread($fd, 2048);
				if($outFile==null)
					echo $buffer;
				else
					fwrite($fout, $buffer);
			}
		}	
		
		if($outFile!=null)
			fclose($fout);
	}
			
	/**
	 *   This method is called before saving AR model to database table.
	 *   In this method, we set some required model attributes.
	 */
	protected function beforeSave()
	{
		// First call the base class
		if(!parent::beforeSave())
			return false;

		// Do nothing if this is not a new AR
		if(!$this->isNewRecord)
			return true;
		
		// 'project_name' field might be set on external upload
		if(isset($this->project_name))
		{
			$project = Project::model()->findByAttributes(
					array('name'=>$this->project_name));
			if($project===null)
			{
				$this->addError('project_id', 'Invalid project name.');
				return false;
			}
		
			$this->project_id = $project->id;
		}
		
		if(!isset($this->project_id))
		{
			$curProjectId = Yii::app()->user->getCurProjectId();
			if($curProjectId==false)
			{
				$this->addError('project_id', 'Current project is not set.');
				return false;
			}
			$this->project_id = $curProjectId;
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
			$this->addError('project_id', 'This project is disabled and not accepting debug info files.');
			return false;
		}
		
		// Save uploaded file to its persistent location
		if(!$this->saveFileAttachment())
			return false;
		
		// Set "dateuploaded" attribute with current time
		$this->dateuploaded = time();
			
		// Set "status" attribute with Pending
		$this->status = self::STATUS_PENDING_PROCESSING; 
				
		// OK.
		return true;
	}
	
    /**
     * Marks this record for deletion. 
     */
    public function markForDeletion()
    {        
        $this->status = self::STATUS_PENDING_DELETE;
        return $this->save();        
    }
    
	/**
	 *   This method is called before deleting AR model from database table.
	 *   In this method, we delete file attachment.
	 */
	protected function beforeDelete()
	{        
		// First call the base class
		if(!parent::beforeDelete())
			return false;
	
        
		// Delete all processing errors associated with this record.
		$processingErrors = $this->getProcessingErrors();
		foreach($processingErrors as $processingError)
		{
			$processingError->delete();
		}
		        
        /*
		// Determine path to local file 
		$fileName = $this->getLocalFilePath();
	
		// Delete file		
		if(!@unlink($fileName))
		{
			// Couldn't delete file
			$this->addError('fileAttachment', 'Could not unlink the file.');			
		}
				
		// Delete parent directory.		
        $parentDirName = dirname($fileName);
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
		*/
        
		// OK.
		return true;
	}

	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'dateuploaded' => 'Date Uploaded',
			'md5' => 'MD5 Hash',
			'filename' => 'File Name',	
			'filesize' => 'File Size',	
			'guid' => 'GUID+Age',	
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
		
		// Get current project ID 
		$curProjectId = Yii::app()->user->getCurProjectId();
		if($curProjectId==false)
			return Null;
		
		// Create empty search criteria
		$criteria=new CDbCriteria;

		$criteria = new CDbCriteria();							
		if($this->isAdvancedSearch==false)
		{
			// Simple search
			$criteria->compare('filename', $this->filter, true, 'OR');
			$criteria->compare('guid', $this->filter, true, 'OR');			
			
		}
		else
		{
			// Advanced search				
			if(trim($this->filename)!="")
				$criteria->compare('filename', $this->filename, true, 'AND');
			if(trim($this->guid)!="")
				$criteria->compare('guid', $this->guid, true, 'AND');			
			if($this->status!=-1)
				$criteria->compare('status', $this->status, false, 'AND');			
			if($this->dateFrom!="" & $this->dateTo!="")
				$criteria->addBetweenCondition('dateuploaded', $this->strToDate($this->dateFrom), $this->strToDate($this->dateTo), 'AND');										
		}
				
		$criteria->compare('project_id', $curProjectId, false, 'AND');
		        
		// Perform search
		$dataProvider = new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'dateuploaded DESC'
            ),
            'pagination'=>array(
                'pageSize'=>50
            )
		));
        
        return $dataProvider;
	}
		
	/**
	 * Generates a debug info upload statistics for current project and
	 * desired time period.
	 * @param type $w Image width.
	 * @param type $h Image height.
	 * @param type $period Time period (7, 30 or 365).
	 * @return void
	 */
	public static function generateDebugInfoUploadStat($w, $h, $period, $file=null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($period) || $period<=0 || $period>365)
			throw new CHttpException(403, 'Invalid parameter');
			
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
				
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
			$criteria->addBetweenCondition('dateuploaded', $dateFrom, $curDate);			
			$count = DebugInfo::model()->count($criteria);
			
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
	 * Returns the list of processing errors associated with this debug info. 
	 */
	public function getProcessingErrors()
	{
		$criteria = new CDbCriteria();
		$criteria->compare('type', ProcessingError::TYPE_DEBUG_INFO_ERROR);
		$criteria->compare('srcid', $this->id);
		$models = ProcessingError::model()->findAll($criteria);
		return $models;
	}
}