<?php

// The following is needed to use ezComponents Graph
Yii::import('application.vendors.ezcomponents.*');
require_once "Base/src/base.php"; 
Yii::registerAutoloader(array('ezcBase', 'autoload'), true);

/**
 * This is the model class for table "{{bug}}".
 */
class Bug extends CActiveRecord
{	
	// Open statuses.
	const STATUS_NEW      = 1;   // New bug.
	const STATUS_REVIEWED = 2;   // Bug is being reviewed.
	const STATUS_ACCEPTED = 3;   // Bug accepted.
	const STATUS_STARTED  = 4;   // Bug is being fixed.	
	const STATUS_OPEN_MAX = 100; // Maximum available open status.
	// Closed statuses.
	const STATUS_FIXED     = 101; // Bug has been fixed.
	const STATUS_VERIFIED  = 102; // Bug has been verified by QA
	const STATUS_DUPLICATE = 103; // Bug is a duplicate of another bug
	const STATUS_WONTFIX   = 104; // We have decided not to take an action on this bug.
	
	// Bug priorities
	const PRIORITY_LOW      = 1; // Low priority
	const PRIORITY_MEDIUM   = 2; // Medium priority
	const PRIORITY_HIGH     = 3; // High priority
	const PRIORITY_CRITICAL = 4; // Critical priority
	
	// Bug reproducibility constants
	const REPRO_NOT_TRIED   = 1; // Not tried to reproduce this bug.
	const REPRO_NEVER       = 2; // Bug can never be reproduced.  
	const REPRO_SOMETIMES   = 3; // Bug can be sometimes reproduced.
	const REPRO_ALWAYS      = 4; // Bug can always be reproduced.
	
	// Non-db attributes
	public $crashreports;   // Space-separated list of crash reports associated with this bug.
	public $crashgroups;    // Space-separated list of crash groups associated with this bug.	
	public $comment;        // Comment (used on change).
	public $fileAttachment; // File attachment (used on change).	
	public $cnt; // Count (used in stat query)
    
	// Search related variables
	public $filter; // Simple search filter.
	public $bugStatusFilter; // Simple search bug status filter;
	public $isAdvancedSearch = false; // Is advanced search enabled?
	public $createdFrom; // Received from (used in advanced search).
	public $createdTo;   // Received to (used in advanced search).
    	
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return Bug the static model class
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
		return '{{bug}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('summary, assigned_to', 'required', 'on'=>'create, change'), // Summary required always
			array('status, priority, reproducability, description, assigned_to', 'required', 'on'=>'create'),
			array('status, priority, reproducability, assigned_to', 'numerical', 'integerOnly'=>true),
			array('summary', 'length', 'min'=>1, 'max'=>256),			
			array('description', 'length', 'min'=>1, 'max'=>2048),					
			array('crashgroups', 'numberList', 'CrashGroup'),
			array('crashreports', 'numberList', 'CrashReport'),
			array('crashgroups', 'atLeastRequired', 'crashgroups, crashreports', 'count'=>1, 'on'=>'create, change'),
			// The following rules are used on change
			array('fileAttachment', 'file', 'maxFiles'=>1, 'minSize'=>1, 'maxSize'=>20*1024*1024, 'allowEmpty'=>true, 'on'=>'change'),
			array('comment', 'length', 'max'=>2048, 'on'=>'change'),					
			array('reporter, description', 'unsafe', 'on'=>'change'),								
			array('merged_into', 'checkMergedInto', 'on'=>'change'),								
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, date_created, status, priority, summary, reported_by, assigned_to', 'safe', 'on'=>'search'),
			array('createdFrom', 'compareDates', 'on'=>'search'),
		);
	}

	/**
	 * This validator method checks 'merged_into' attribute.
	 * @param string $attribute Attribute name.
	 * @param array $params Parameters.
	 * @return boolean true on success.
	 */
	public function checkMergedInto($attribute, $params)
	{
		if($this->status==self::STATUS_DUPLICATE)
		{
			if(isset($this->merged_into) &&
				is_numeric($this->merged_into))
			{
				// Search for a bug with such an ID.
				$bug = Bug::findByPk($this->merged_into);
				if($bug!=null && $bug->id!=$this->id && $bug->project_id==$this->project_id)
					return true; // OK.
			}
			
			// Not a valid bug ID.
			$this->addError('merged_into', 'Merged Into should be a valid bug ID.');
			return false;
		}
		else
		{
			// Unset to avoid errors
			unset($this->merged_into);
		}
		
		// OK.
		return true;
	}
	
	/**
	 * This validator checks if at least one attribute is defined.
	 * @param string $attribute Attribute name.
	 * @param array $params Parameters.
	 * @return boolean  true on success.
	 */
	public function atLeastRequired($attribute, $params) 
	{		
		if (is_array($params)) 
		{
			$fields = explode(', ', $params[0]);
			$min = $params['count'];
		} 
		/*else 
		{
			$fields = explode(', ', $params);
			$min = 1;
		}*/
		$count = 0;
		$labels = array();
		foreach ($fields as $name) 
		{
			$labels[] = $this->getAttributeLabel($name);
			if ( !empty($this->$name) ) { $count++; }							
		}		
		if ($count < $min) 
		{
			$this->addError($attribute, 'At least one of these required: '.implode(', ', $labels) );
			return false;
		}
		
		return true;
	}
	
	/**
	 * This validator method checks if the attribute is a list of space separated
	 * numbers.
	 * @param string $attribute Attribute name.
	 * @param array $params Parameters.
	 * @return boolean true on success.
	 */
	public function numberList($attribute,$params)
    {
		if($this->scenario=='search')
			return true; // Do not check on search.
			
		if(!isset($this->$attribute) || strlen(trim($this->$attribute))==0)
			return true; // Empty
		
		$numbers = MiscHelpers::splitNumberList($this->$attribute);
		if($numbers==false)
		{
			$this->addError($attribute, $this->getAttributeLabel($attribute).' must be a list of numbers separated with spaces.');
			return false;
		}
				
		$appversion = 0;
		foreach($numbers as $number)
		{	
			// Check if such a crash report/crash group exists
			$model = $params[0]::model()->findByPk($number);
			if($model===null || !$model->canOpenNewBug())
			{
				$this->addError($attribute, $this->getAttributeLabel($attribute).' must contain valid ID(s).');
				return false;
			}		
									
		}
		        
		return true;
    }
	
	/**
	 *  This validator compares Date From and Date To to ensure Date To is the greater one.
	 * @param string $attribute Attribute name.
	 * @param array $params Parameters.
	 * @return boolean true on success.
	 */
	public function compareDates($attribute, $params)
	{
		// Check scenarion
		if($this->scenario!='search')
			return true;
		
		// Check if both dates are set
		if(!isset($this->createdFrom) || !isset($this->createdTo))
			return true;
		
		// Convert date strings to timestamps
		$date1 = $this->strToDate($this->createdFrom);
		$date2 = $this->strToDate($this->createdTo);
		
		// Check timestamps
		if($date1==False || $date2==False)
			return true; // One of dates is invalid - nothing to compare
		
		// Compare dates
		if($date2>=$date1)
			return true; // Date To is greater than Date From
		
		// Error
		$this->addError('createdTo', 'Created To must be greater or equal to Created From.');
		return false;		
	}
	
	/**
	 * A helper method that converts a date string to timestamp.
	 * @param string $date Date string
	 * @return integer timestamp.
	 */
	public function strToDate($date)
	{
		$timestamp=CDateTimeParser::parse($date,'MM/dd/yyyy');
		return $timestamp;
	}
	
	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
			'reporter'=>array(self::BELONGS_TO, 'User', 'reported_by'),
			'owner'=>array(self::BELONGS_TO, 'User', 'assigned_to'),
			'crashReports'=>array(self::HAS_MANY, 'BugCrashReportMapping', 'bug_id'),
			'crashGroups'=>array(self::HAS_MANY, 'BugCrashGroupMapping', 'bug_id'),
			'bugchanges'=>array(self::HAS_MANY, 'BugChange', 'bug_id'),
			'project'=>array(self::BELONGS_TO, 'Project', 'project_id'),
            'appVersion'=>array(self::BELONGS_TO, 'AppVersion', 'appversion_id'),			
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'date_created' => 'Created',
			'status' => 'Status',
			'summary' => 'Summary',
			'assigned_to' => 'Owner',
			'reported_by' => 'Reporter',
			'crashreports' => 'Crash reports associated with this bug',
			'crashgroups' => 'Collections associated with this bug',
            'reproducability' => 'Reproducibility'
		);
	}

	/**
	 * Fills summary and description field based on associated crash group
	 * or crash report information.
	 * @return void
	 * @throws CHttpException 
	 */
	public function autoFillSummary()
	{
		if(isset($this->crashgroups))
		{
			if(!is_numeric($this->crashgroups))
				throw new CHttpException(403, 'Invalid request.');
		
			$crashGroupId = $this->crashgroups;
			$crashGroup = CrashGroup::model()->findByPk($crashGroupId);
			if($crashGroup===null)
				throw new CHttpException(403, 'Invalid request.');

			$this->summary = $crashGroup->title;
			$this->description = 'Bug on Collection #'.$crashGroup->id;
		}
		else if(isset($this->crashreports))
		{
			if(!is_numeric($this->crashreports))
				throw new CHttpException(403, 'Invalid request.');
			
			$crashReportId = $this->crashreports;
			$crashReport = CrashReport::model()->find($crashReportId);
			if($crashReport===null)
				throw new CHttpException(403, 'Invalid request.');

			$this->summary = 'Bug on Crash Report #'.$crashReport->id;
			$this->description = $this->summary;
		}
		else
		{
			throw new CHttpException(403, 'Invalid request.');
		}
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
                
		$criteria=new CDbCriteria;
		
		// Simple search filter
		if(!$this->isAdvancedSearch)
		{
			if(isset($this->filter))
			{			
				$criteria->compare('summary', $this->filter, true, 'OR');
				$criteria->compare('reporter.username', $this->filter, true, 'OR');
				$criteria->compare('owner.username', $this->filter, true, 'OR');
				$criteria->with = array('reporter', 'owner');
			}
			
			if(!isset($this->bugStatusFilter))
			{
				// Set default bug search filter
				$user = Yii::app()->user->loadModel();
				if($user!=null)
				{				
					$this->bugStatusFilter = $user->group->default_bug_status_filter;					
				}
				else
					$this->bugStatusFilter = 'open';
			}
			
			if($this->bugStatusFilter=='open')
				$criteria->compare('t.status', '<'.Bug::STATUS_OPEN_MAX, false, 'AND');
			else if($this->bugStatusFilter=='closed')
				$criteria->compare('t.status', '>'.Bug::STATUS_OPEN_MAX, false, 'AND');
			else if($this->bugStatusFilter=='reported')
			{				
				$criteria->compare('t.status', '<'.Bug::STATUS_OPEN_MAX, false, 'AND');
				$criteria->compare('reported_by', Yii::app()->user->id, false, 'AND');
			}
			else if($this->bugStatusFilter=='owned')
			{				
				$criteria->compare('t.status', '<'.Bug::STATUS_OPEN_MAX, false, 'AND');
				$criteria->compare('assigned_to', Yii::app()->user->id, false, 'AND');
			}
			else if($this->bugStatusFilter=='verify')
			{				
				$criteria->compare('t.status', Bug::STATUS_FIXED, false, 'AND');				
			}
			else if($this->bugStatusFilter!='all')
				throw new CHttpException(403, 'Invalid request.');				
		}
		else if($this->isAdvancedSearch) // Advanced search
		{				
			// Fill search criteria
			if(isset($this->summary))
				$criteria->compare('summary', $this->summary, true, 'AND');
			if(isset($this->status) && $this->status!=-1)
				$criteria->compare('t.status', $this->status, false, 'AND');			
			if(isset($this->priority) && $this->priority!=-1)
				$criteria->compare('priority', $this->priority, false, 'AND');			
			if($this->createdFrom!="" & $this->createdTo!="")
				$criteria->addBetweenCondition('date_created', 
						$this->strToDate($this->createdFrom), 
						$this->strToDate($this->createdTo), 'AND');						
			if(isset($this->reported_by))
				$criteria->compare('reporter.username', $this->reported_by, true, 'AND');
			if(isset($this->assigned_to))
				$criteria->compare('owner.username', $this->assigned_to, true, 'AND');
			
			$criteria->with = array('reporter', 'owner');
		}		
		
		// Get current project ID and version
		$criteria->compare('project_id', $curProjectId, false, 'AND');
		$curProjectVer = Project::PROJ_VER_NOT_SET;
		Yii::app()->user->getCurProjectVersions($curProjectVer);
		if($curProjectVer!=Project::PROJ_VER_ALL)			
			$criteria->compare('appversion_id', $curProjectVer, false, 'AND');

		$dataProvider = new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
            'sort'=>array(
                // Sort bugs by id in descendent order
                'defaultOrder'=>'t.id DESC'
            ),
            'pagination'=>array(
                'pageSize'=>50
            )
		));
		
        return $dataProvider;
	}
	
	/**
	 * This method should be used instead of save on new bug creation.
	 * @return boolean true on success. 
	 */
	public function open()
	{
        $this->scenario = 'create';
        
		// Begin DB transaction
		$transaction = Yii::app()->db->beginTransaction();
		
		try 
		{				
			// Save bug AR to database
			if(!$this->save())
				throw new Exception('Error saving bug record to database.');
						
			// Create an initial bug change	record		
			$bugStatusChange = new BugStatusChange;
			if(isset($this->status))
				$bugStatusChange->status = $this->status;
			if(isset($this->priority))
				$bugStatusChange->priority = $this->priority;			
			if(isset($this->reproducability))
				$bugStatusChange->reproducability = $this->reproducability;
			if(isset($this->owner))
				$bugStatusChange->owner = $this->owner;
			
			if(!$bugStatusChange->save())
				throw new Exception('Error saving initial bug status change to database.');
			
			$bugChange = new BugChange;			
			$bugChange->timestamp = $this->date_created;
			$bugChange->user_id = $this->reported_by;
			$bugChange->bug_id = $this->id;
			$bugChange->status_change_id = $bugStatusChange->id;
			$bugChange->flags = BugChange::FLAG_INITIAL_CHANGE;
						
			if(!$bugChange->save())
				throw new Exception('Error saving initial bug change to database.');
		
			// Update crash reports and crash groups associations
			$this->updateAssociatedCrashReportsAndCrashGroups();
						
			// Commit transaction
			$transaction->commit();			
		}
		catch(Exception $e)
		{
			// Roll back transaction
			$transaction->rollBack();
				
			// Add a message to log.
			Yii::log('An exception caught: '.$e->getMessage(), "error");            
			
			return false;
		}
		
		// Send notification email
		$this->sendBugOpenNotificationEmail();
		
		// Success.
		return true;
	}
	
	/**
	 * This method updates crash reports and crash groups associations 
	 * for this bug.
	 * @return void
	 * @throws Exception
	 */
	public function updateAssociatedCrashReportsAndCrashGroups()
	{
		if(isset($this->crashreports))
		{
			// Split string
			$numbers = MiscHelpers::splitNumberList($this->crashreports);		
			if(count($numbers)!=0)
			{	
				// First delete old {{bug_crashreport}} mappings
				BugCrashReportMapping::model()->deleteAll('bug_id='.$this->id);
				
				// Walk through numbers
				foreach($numbers as $number)
				{				
					// Add a record to {{bug_crashreport}} table.				
					$bugCrashReportMapping = new BugCrashReportMapping;
					$bugCrashReportMapping->bug_id = $this->id;
					$bugCrashReportMapping->crashreport_id = $number;

					if(!$bugCrashReportMapping->save())
					{
						throw new Exception('Error saving crash report IDs.');										
					}
				}
			}
		}
		
		if(isset($this->crashgroups))
		{
			// Split string
			$numbers = MiscHelpers::splitNumberList($this->crashgroups);		
			if(count($numbers)!=0)
			{
				// First delete old {{bug_crashgroup}} mappings
				BugCrashGroupMapping::model()->deleteAll('bug_id='.$this->id);
									
				// Walk through numbers
				foreach($numbers as $number)
				{				
					// Add a record to {{bug_crashgroup}} table.				
					$bugCrashGroupMapping = new BugCrashGroupMapping;
					$bugCrashGroupMapping->bug_id = $this->id;
					$bugCrashGroupMapping->crashgroup_id = $number;

					if(!$bugCrashGroupMapping->save())
					{
						throw new Exception('Error saving crash report IDs.');										
					}
				}
			}
		}
	}
	
	/**
	 * This method applies changes to model fields. Used when 
     * changing/commenting bug.
	 * @param array $attributes new (changed) model attributes.
     * @return boolean true on success.
	 */
	public function change($attributes)
	{
		// Apply 'change' scenario.
		$this->scenario = 'change';
		        
        // Remove "empty" attributes
        foreach($attributes as $name=>$val)
        {            
            if(trim($val)=='')
                unset($attributes[$name]);
        }
        
        // Begin DB transaction
		$transaction = Yii::app()->db->beginTransaction();
		
		try 
		{	
            $bugChanged = false;
            
            // Check if title has changed
            if(array_key_exists('summary', $attributes))
            {
                if($this->summary!=$attributes['summary'])
                {
                    $this->summary = $attributes['summary'];
                    $bugChanged = true;
                }
            }
            
            // Check if crashgroups changed
            if(array_key_exists('crashgroups', $attributes))
            {
                $my_crashgroups = MiscHelpers::splitNumberList($this->crashgroups);
                $their_crashgroups = MiscHelpers::splitNumberList($attributes['crashgroups']);
                $diff_crashgroups = array_merge(
                        array_diff($my_crashgroups, $their_crashgroups), 
                        array_diff($their_crashgroups, $my_crashgroups));
                if(count($diff_crashgroups)!=0)
                {
                    $this->crashgroups = $attributes['crashgroups'];
                    $bugChanged = true;
                }
            }
            
            // Check if crashreports changed
            if(array_key_exists('crashreports', $attributes))
            {
                $my_crashreports = MiscHelpers::splitNumberList($this->crashreports);
                $their_crashreports = MiscHelpers::splitNumberList($attributes['crashreports']);            
                $diff_crashreports = array_merge(
                        array_diff($my_crashreports, $their_crashreports), 
                        array_diff($their_crashreports, $my_crashreports));
                if(count($diff_crashreports)!=0)
                {
                    $this->crashreports = $attributes['crashreports'];
                    $bugChanged = true;
                }
            }
            
            
            // Create new BugChange model
			$bugChange = new BugChange;			
            $bugChange->bug_id = $this->id; // bug id
			$bugChange->user_id = Yii::app()->user->id; // logged-in user
			$bugChange->timestamp = time(); // current time
			$bugChange->flags = 0;
			
			// Check if comment presents
			if(array_key_exists('comment', $attributes))
			{
                $this->comment = $attributes['comment'];
                
				// Create new comment record
				$bugComment = new BugComment;				
				$bugComment->text = $attributes['comment'];
				
				// Save comment record
				if(!$bugComment->save())
				{
					$errors = $bugComment->getErrors();
					foreach($errors as $fieldName=>$fieldErrors)
						$this->addErrors('comment', $fieldErrors);
					throw new Exception('Error saving comment.');
				}
				
                // Set link between bug change record and comment record
				$bugChange->comment_id = $bugComment->id;
			}
			            
			// Check file attachment
			$bugAttachment = null;
			if(isset($this->fileAttachment) &&
               $this->fileAttachment instanceOf CUploadedFile)
			{	          
				// Create attachment record
				$bugAttachment = new BugAttachment;				
				$bugAttachment->fileAttachment = $this->fileAttachment;				
			}	
            		
            // Create new bug status change model
			$bugStatusChange = new BugStatusChange;            
			            
			// Check if some of bug attribute values have changed
			$changed = false;                          
			foreach($attributes as $name=>$val)
			{                  
                // Check if attribute has changed
                if(array_key_exists($name, $bugStatusChange->attributes) && $this->$name!=$val)
                {                    
                    $changed = true; // Attribute has changed	                        
                    $bugStatusChange->$name = $val;
                    $this->$name = $val;
                }                
			}
									
			if($changed)			
			{
				if(!$bugStatusChange->save())
				{
					$errors = $bugStatusChange->getErrors();
					throw new Exception('Error saving bug status change.');
				}
				
				$bugChange->status_change_id = $bugStatusChange->id;				
			}
						
            // Check if at least something (status fields, attachment or comment)
            // have changed
            $someFieldsChanged = isset($bugChange->status_change_id) ||
			   $bugAttachment!=null ||
			   isset($bugChange->comment_id);
			if(!$someFieldsChanged &&
               !$bugChanged)
			{	
				$this->addError('comment', 'At least one field should be changed.');
				throw new Exception('Nothing changed.');
			}
			
			// Save changes to bug record	
            if(!isset($this->fileAttachment))
                $this->fileAttachment='';
            if(!$this->save())
				throw new Exception('Error saving bug.');
            
			// Save bug change record			
			if($someFieldsChanged && !$bugChange->save())
			{
				$errors = $bugChange->getErrors();
				foreach($errors as $fieldName=>$fieldErrors)
				{
					foreach($fieldErrors as $error)
						Yii::log('Error saving bug: '.$error, 'error');
					$this->addErrors('comment', $fieldErrors);
				}
				throw new Exception('Error saving bug change.');
			}
						
			// Save file attachment
			if($bugAttachment!=null)				
			{
				$bugAttachment->bug_change_id = $bugChange->id;
				
				if(!$bugAttachment->save())
				{
					$errors = $bugAttachment->getErrors();
					foreach($errors as $fieldName=>$fieldErrors)
						$this->addErrors('fileAttachment', $fieldErrors);
					throw new Exception('Error saving bug attachemnt.');
				}
			}
			
			// Reset comment and file attachment
			$this->comment = '';
			unset($this->fileAttachment);			
			
			// Update crash reports and crash groups associations
			$this->updateAssociatedCrashReportsAndCrashGroups();
			
			// Commit transaction
			$transaction->commit();			
		}
		catch(Exception $e)
		{
			// Roll back transaction
			$transaction->rollBack();
				
			// Add a message to log.
			Yii::log('An exception caught: '.$e->getMessage(), "error");                        
			
			return false;
		}
		
		// Send notification email
		$this->sendBugChangeNotificationEmail($bugChange);
				
		// Success.
		return true;			
	}
	
	/**
	 * This method sends an email to bug owner and reporter.
	 * The email contains bug information.
	 * @return boolean true on success.
	 */
	public function sendBugOpenNotificationEmail()
	{			
		// Check if reporter==owner, and if so, do nothing.
		if($this->owner===null ||
			$this->owner->id==$this->reporter->id)
			return true;
		
		// Format email subject
		$emailSubject = "[CrashFix] Bug #".$this->id." Has Been Opened";
		
		$emailText = "Bug #".$this->id." ".$this->summary."\r\n";
		$emailText .= "URL: ".Yii::app()->createAbsoluteUrl('bug/view', array('id'=>$this->id));
		$emailText .= "\r\n";
		$emailText .= "Reported by ".$this->reporter->username." on ".date('j F Y, G:i', $this->date_created)."\r\n";		
		$emailText .= "\r\n";				
		
		if(isset($this->description))
		{
			$emailText .= "Bug description:\r\n";
			$emailText .= wordwrap($this->description, 70)."\r\n";
			$emailText .= "\r\n";				
		}
		
		$emailText .= "Status:".Lookup::item('BugStatus', $this->status)."\r\n";
				
		$emailText .= "Priority: ".Lookup::item('BugPriority', $this->priority)."\r\n";									

		$emailText .= "Reproducibiliity: ".Lookup::item('BugReproducability', $this->reproducability)."\r\n";									
			
		if(isset($this->owner))
			$emailText .= "Owner: ".$this->owner->username."\r\n";									
			
		$emailText .= "\r\n";				
		
		// Send an email to bug owner
		return MailQueue::addMail($this->owner->email, $emailSubject, $emailText);							
	}
	
	/**
	 * This method sends an email to bug owner and reporter.
	 * The email contains bug change information.
	 * @return boolean true on success.
	 */
	public function sendBugChangeNotificationEmail($bugChange)
	{			
		$emailSubject = "[CrashFix] Bug #".$bugChange->bug->id." Has Been Changed";
		
		$emailText = "Bug #".$bugChange->bug_id." ".$bugChange->bug->summary."\r\n";
		$emailText .= "URL: ".Yii::app()->createAbsoluteUrl('bug/view', array('id'=>$bugChange->bug_id));
		$emailText .= "\r\n";
		$emailText .= "Changed by ".$bugChange->user->username." on ".date('j F Y, G:i', $bugChange->timestamp)."\r\n";		
		$emailText .= "\r\n";				
		
		if($bugChange->comment!=null)
		{
			$emailText .= "Comment has been entered:\r\n";
			$emailText .= wordwrap($bugChange->comment->text, 70)."\r\n";
			$emailText .= "\r\n";				
		}
		
		if($bugChange->statuschange!=null)
		{
			if(isset($bugChange->statuschange->status))
			{				
				$emailText .= "Status:".Lookup::item('BugStatus', $bugChange->statuschange->status)."\r\n";
				if($bugChange->statuschange->status==Bug::STATUS_DUPLICATE)
				{
					$emailText .= "Merged into bug: #".$bugChange->statuschange->merged_into."\r\n";
				}					
			}
			
			if(isset($bugChange->statuschange->priority))
				$emailText .= "Priority: ".Lookup::item('BugPriority', $bugChange->statuschange->priority)."\r\n";									

			if(isset($bugChange->statuschange->reproducability))
				$emailText .= "Reproducibiliity: ".Lookup::item('BugReproducability', $bugChange->statuschange->reproducability)."\r\n";									
			
			if(isset($bugChange->statuschange->owner))
				$emailText .= "Owner: ".$bugChange->statuschange->owner->username."\r\n";									
			
			$emailText .= "\r\n";				
		}
		
		foreach($bugChange->attachments as $attachment)
		{
			$emailText .= "File attached: ".$attachment->filename;			
			$emailText .= "\r\n";				
		}
				
		$emailText .= "\r\n";				
				
		// Send an email to bug reporter
		$add1 = MailQueue::addMail($bugChange->bug->reporter->email, $emailSubject, $emailText);		
		$add2 = false;
		// Send an email to bug owner
		if($bugChange->bug->owner!=null &&
			$bugChange->bug->owner->id!=$bugChange->bug->reporter->id)
		{
			$add2 = MailQueue::addMail($bugChange->bug->owner->email, $emailSubject, $emailText);					
		}
		
		return $add1 && $add2;
	}
	
	/**
	 * This method is called after the model has been instantiated.
	 * In this method, we assign some model attributes, such as 'crashreports'
	 * and 'crashgroups'.
	 * @return void 
	 */
	protected function afterFind()
	{
		// Call parent impl
		parent::afterFind();
        
        if(!isset($this->id))
            return;
		
		// Find crash group(s) associated with this bug
		$criteria = new CDbCriteria;
		$criteria->select = 'crashgroup_id';
		$criteria->distinct = true;
		$criteria->condition = 'bug_id='.$this->id;
		
		$bugCrashGroups = BugCrashGroupMapping::model()->findAll($criteria);
		// Format crash group list
		$list = "";
		foreach($bugCrashGroups as $bugCrashGroup)
		{
			$list = $list.$bugCrashGroup->crashgroup_id.' ';
		}
        $list = trim($list);
		// Assign crashgroups attribute
		$this->crashgroups = $list;
		
		// Find crash report(s) associated with this bug
		$criteria2 = new CDbCriteria;
		$criteria2->select = 'crashreport_id';
		$criteria2->distinct = true;
		$criteria2->condition = 'bug_id='.$this->id;
		
		$bugCrashReports = BugCrashReportMapping::model()->findAll($criteria2);
		// Format crash group list
		$list = "";
		foreach($bugCrashReports as $bugCrashReport)
		{
			$list = $list.$bugCrashReport->crashreport_id.' ';
		}
        $list = trim($list);
		// Assign crashgroups attribute
		$this->crashreports = $list;
	}
	
	/**
	 * This method is executed before AR is saved to database.
	 * In this method, we set some model attributes.
	 * @return boolean True on success.
	 */
	protected function beforeSave()
	{
		if(!parent::beforeSave())
			return false;
		
		// Check if this is a new record.
		if($this->isNewRecord)
		{		
			// Set project id
			$this->project_id = Yii::app()->user->getCurProjectId();

			// Set project version
			$curVer = false;
			$versions = Yii::app()->user->getCurProjectVersions($curVer);		
			if($curVer!=Project::PROJ_VER_ALL)
				$this->appversion_id = $curVer;
			else
			{
				// Project version '<all>' currently selected.
				// We have to determine bug version from associated 
				// crash reports/collections.
				$numbers = MiscHelpers::splitNumberList($this->crashgroups);
				foreach($numbers as $number)
				{
					$crashGroup = CrashGroup::model()->findByPk($number);
					if($crashGroup===Null)
						throw new HttpException('Invalid crash group ID');
					$curVer = $crashGroup->appversion_id;
					break;
				}
				
				if($curVer==-1)
				{
					$numbers = MiscHelpers::splitNumberList($this->crashreports);
					foreach($numbers as $number)
					{
						$crashReport = CrashReport::model()->findByPk($number);
						if($crashReport===Null)
							throw new HttpException('Invalid crash report ID');
						$curVer = $crashReport->appversion_id;
						break;
					}
				}
				
				if($curVer==-1)
					throw new CHttpException(403, 'Invalid request');
				
				$this->appversion_id = $curVer;
			}

			// Set date created
			$this->date_created = time();
			
			// Set date last modified
			$this->date_last_modified = $this->date_created;
			
			// Set reported by
			$this->reported_by = Yii::app()->user->id;
		
			if($this->assigned_to<0)
				unset($this->assigned_to);
		}
		else
		{
			// Set date last modified
			$this->date_last_modified = time();
			
			if($this->status>Bug::STATUS_OPEN_MAX)
				$this->date_closed = $this->date_last_modified;
			else
				unset($this->date_closed);
		}
				
		// Success.
		return true;
	}
	
	/**
	 * This method is called before deleting AR model from database table.
	 * In this method, we delete file attachments, crash reports, crash groups
	 * associated with this bug.
	 */
	protected function beforeDelete()
	{
		// First call the base class
		if(!parent::beforeDelete())
			return false;
		
		// Remove related bug changes
		foreach($this->bugchanges as $bugChange)
			$bugChange->delete();		
		
		// Remove all related crash report records
		foreach($this->crashReports as $bugCrashReport)
			$bugCrashReport->delete();		
		
		// Remove all related crash groups
		foreach($this->crashGroups as $bugCrashGroup)
			$bugCrashGroup->delete();		
		
		// Success.
		return true;
	}	
	
	/**
	 * Generates a bug status dynamics graph for current project and desired
	 * time period.
	 * @param integer $w Image width.
	 * @param integer $h Image height.
	 * @param integer $period Time period (7, 30 or 365). 
	 * @return void
	 */
	public static function generateBugStatusDynamicsGraph($w, $h, $period, $file=null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($period) || $period<=0 || $period>365)
			throw new CHttpException(403, 'Invalid parameter');
			
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
		$curVer = Yii::app()->user->getCurProjectVer();
		
		// Prepare data
		$dataAll = array();
		$dataOpen = array();
		$dataClosed = array();
		$dataFixed = array();
		$dataVerified = array();
		
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

			// Get bug changes within the period
			$criteria = new CDbCriteria();									
			$criteria->compare('bug.project_id', $curProjectId, false, 'AND');
			if($curVer!=-1)
				$criteria->compare('bug.appversion_id', $curVer, false, 'AND');		
			$criteria->addCondition('t.status_change_id IS NOT NULL', 'AND');			
			$criteria->addCondition('t.timestamp <='.$curDate, 'AND');					
			$criteria->addCondition('t.id IN (SELECT MAX({{bug_change}}.id) FROM {{bug_change}} GROUP BY {{bug_change}}.bug_id)', 'AND');
			$criteria->with = array('bug', 'statuschange');
			$bugChanges = BugChange::model()->findAll($criteria);			
			
			$countAll = 0;
			$countOpen = 0;
			$countClosed = 0;
			$countFixed = 0;
			$countVerified = 0;
			
			foreach($bugChanges as $bugChange)
			{	
				//print_r($bugChange->statuschange->status);
				
				if($bugChange->statuschange->status<Bug::STATUS_OPEN_MAX)
					$countOpen++;
				else if($bugChange->statuschange->status>Bug::STATUS_OPEN_MAX)
					$countClosed++;
				
				if($bugChange->statuschange->status==Bug::STATUS_FIXED)
					$countFixed++;
				
				if($bugChange->statuschange->status==Bug::STATUS_VERIFIED)
					$countVerified++;				
			}
			
			// Add an item to data
			$key = $period>30?date('M y', $curDate):date('j M', $curDate);								
			$dataAll = array($key=>($countOpen+$countClosed)) + $dataAll;
			$dataOpen = array($key=>$countOpen) + $dataOpen;
			$dataClosed = array($key=>$countClosed) + $dataClosed;
			$dataFixed = array($key=>$countFixed) + $dataFixed;
			$dataVerified = array($key=>$countVerified) + $dataVerified;
			
			// Next time interval
			$curDate = $dateFrom-1;
		}
		
		/*var_dump($dataAll);
		var_dump($dataOpen);
		var_dump($dataClosed);
		var_dump($dataFixed);
		var_dump($dataVerified);
		return;*/
		
		// Create graph
		$graph = new ezcGraphLineChart();		
		$graph->palette = new ezcGraphPaletteEzBlue();
		$graph->palette->dataSetColor = array( 
			'#0000FF',			
			'#FF0000',
			'#00FF00',			
			'#000000',			
		);
		
		$graph->data['All'] = new ezcGraphArrayDataSet($dataAll);
		$graph->data['Open'] = new ezcGraphArrayDataSet($dataOpen);
		$graph->data['Fixed'] = new ezcGraphArrayDataSet($dataFixed);
		$graph->data['Verified'] = new ezcGraphArrayDataSet($dataVerified);
		
		$graph->yAxis->majorStep = 10;
		$graph->yAxis->minorStep = 1;
		
		$graph->xAxis->labelCount = 30;
				
		$graph->options->fillLines = 210;
		
		$graph->legend = true;
		$graph->legend->position = ezcGraph::BOTTOM;		
		$graph->options->font->name = 'Tahoma';
				
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
	public static function generateBugStatusDistributionGraph($w, $h, $file=null)
	{
		if(!is_numeric($w) || $w<=0 || $w>1024)
			throw new CHttpException(403, 'Invalid parameter');
		
		if(!is_numeric($h) || $h<=0 || $h>960)
			throw new CHttpException(403, 'Invalid parameter');
		
		// Get current project info
		$curProjectId = Yii::app()->user->getCurProjectId();
		if($curProjectId==false)
			throw new CHttpException(403, 'Invalid parameter');
		
        $curVer = Yii::app()->user->getCurProjectVer();
		        
		// Prepare data
		$criteria = new CDbCriteria();
		$criteria->select = 'status, COUNT(*) as cnt';
		$criteria->group = 'status';
		$criteria->compare('project_id', $curProjectId);		
		$criteria->order = 'status DESC';				
		
        if($curVer!=-1) // -1 means '<all>'
        {
            $criteria->compare('appversion_id', $curVer==0?null:$curVer);		
        }
        
		$data = array();        
		$models = Bug::model()->findAll($criteria);
		$totalCount = 0;			
		$curVerStr = '';
		foreach($models as $model)
		{
			$totalCount += $model->cnt;			
			$data[Lookup::item('BugStatus', $model->status)] = $model->cnt;			
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
		$graph->palette = new ezcGraphPaletteEzRed();	

		$graph->data['Versions'] = new ezcGraphArrayDataSet($data);
		
		$graph->legend = true;
		$graph->legend->position = ezcGraph::RIGHT;
		$graph->options->font->name = 'Tahoma';		
		$graph->options->sum = $totalCount;		
		$graph->options->summarizeCaption = 'Others';
		
		$graph->renderer->options->pieChartGleam = .3;
		$graph->renderer->options->pieChartGleamColor = '#FFFFFF';
		$graph->renderer->options->pieChartGleamBorder = 2;
		
		$graph->renderer = new ezcGraphRenderer3d();
		$graph->renderer->options->pieChartRotation = .8;
		$graph->renderer->options->pieChartShadowSize = 10;

		if($file===null)
			$graph->renderToOutput($w, $h);
		else
			$graph->render($w, $h, $file);
	}
}