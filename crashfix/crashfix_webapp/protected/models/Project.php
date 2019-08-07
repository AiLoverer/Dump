<?php

/**
 * This is the model class for table "{{project}}".
 *
 * The followings are the available columns in table '{{project}}':
 * @property integer $id
 * @property string $name
 */
class Project extends CActiveRecord
{
    // Project version special constants
    const PROJ_VER_ALL = -1;    // (all)
    const PROJ_VER_NOT_SET = 0; // (not set)    
    
	// Project statuses
	const STATUS_ACTIVE   = 1; // This is an active project.
	const STATUS_DISABLED = 2; // This is a disabled project.
	    
	/**
	 *  Callback executed after construction.	 
	 */
	public function afterConstruct()
	{
		parent::afterConstruct();
		
		// Init default quotas
		$this->crash_reports_per_group_quota = 100;
		$this->crash_report_files_disc_quota = 512;
		$this->bug_attachment_files_disc_quota = 100;
		$this->debug_info_files_disc_quota = 2048;
	}
	
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return Project the static model class
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
		return '{{project}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('name, crash_reports_per_group_quota, crash_report_files_disc_quota, debug_info_files_disc_quota, bug_attachment_files_disc_quota, require_exact_build_age', 'required'),
            array('crash_reports_per_group_quota', 'numerical', 'integerOnly'=>true, 'min'=>0), 
            array('crash_report_files_disc_quota', 'numerical', 'integerOnly'=>true, 'min'=>0), 
            array('debug_info_files_disc_quota', 'numerical', 'integerOnly'=>true, 'min'=>0), 
            array('bug_attachment_files_disc_quota', 'numerical', 'integerOnly'=>true, 'min'=>0), 
			array('name', 'length', 'max'=>32),
			array('description', 'length', 'max'=>128),			
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, name', 'safe', 'on'=>'search'),
		);
	}

	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(		
			'users'=>array(self::HAS_MANY, 'UserProjectAccess', 'project_id'),
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'name' => 'Name',
			'description' => 'Description',
			'status' => 'Status',
			'crash_reports_per_group_quota' => 'Max crash reports per collection',
			'crash_report_files_disc_quota' => 'Max total size of crash report files (MB)',
            'bug_attachment_files_disc_quota' => 'Max total size of bug attachment files (MB)',
			'debug_info_files_disc_quota' => 'Max total size of debug info files (MB)',
            'require_exact_build_age' => 'Require exact build age for debugging symbols.'
		);
	}

	/**
	 * Retrieves a list of models based on the current search/filter conditions.
	 * @return CActiveDataProvider the data provider that can return the models based on the search/filter conditions.
	 */
	public function search()
	{
		// Warning: Please modify the following code to remove attributes that
		// should not be searched.

		$criteria=new CDbCriteria;

		$criteria->compare('id',$this->id);
		$criteria->compare('name',$this->name,true);

		return new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
		));
	}
	
	/**
	 * Retrieves a list of users belonging this project.
	 * @return CActiveDataProvider the data provider.
	 */
	public function searchUsers()
	{
		$criteria=new CDbCriteria;
		$criteria->compare('project_id', $this->id);								
		
		return new CActiveDataProvider('UserProjectAccess', array(
			'criteria'=>$criteria,
		));
	}
			
	/**
	 * This callback is executed before saving the AR to database.
	 * @return boolean true on success.
	 */
    public function beforeSave()
    {
		// Check if this is a new record
        if($this->isNewRecord) 
		{
			// Set status to Active
			$this->status = self::STATUS_ACTIVE;			
        }
		
		// Done
        return true;
    }
	
	/**
	 * This callback is executed before deleting the AR from database.
	 * @return boolean true on success.
	 */
    public function beforeDelete()
    {
		// Find all users participating in this project
		$models = UserProjectAccess::model()->findAll('project_id='.$this->id);
		
		// Delete each role associated with this project
		foreach($models as $model)
		{
			$model->delete();
		}
		
		
		// Done
        return true;
    }
	
	/**
	 * Sets project status to Active or Disabled. 
	 * @return boolean true on success.	 
	 */
	public function enable($enable)
	{				
		if($enable==true)
		{
			$this->status = self::STATUS_ACTIVE;
		}
		else
		{
			$this->status = self::STATUS_DISABLED;
		}
		
		return $this->save();
	}
	
	/**
	 * This method determines if there any user that can be added to this project. 
	 * @return boolean true if can add user, otherwise false.
	 */
	public function canAddUser()
	{
		// Find users not in this project
		$users = User::model()->findAllNotInProject($this->id);
		if(count($users)==0)
			return false; // There are no user
		
		// There is a user
		return true;
	}
	
	/**
	 * Returns count of crash reports in this project. 
	 * @param integer $totalFileSize On output, receives the total size in bytes of files.
	 * @param integer $percentOfDiskQuota On output, receives percent of disk quota.
	 * @param integer $appver Optional. If specified, count of files is calculated for the given version.
	 * @return integer count of crash report files.
	 */
	public function getCrashReportCount(&$totalFileSize, &$percentOfDiskQuota, $appver = Project::PROJ_VER_ALL)
	{
		// Find all crash reports belonging to this project and having
		// $appver application version.
		$criteria=new CDbCriteria;				
		$criteria->compare('project_id', $this->id, false, 'AND');
		if($appver!=Project::PROJ_VER_ALL)
		{
			$criteria->compare('appversion_id', $appver, false, 'AND');
		}
		
		$crashReports = CrashReport::model()->findAll($criteria);
		
		// Calculate count of crash reports
		$count = CrashReport::model()->count($criteria);
		
		// Calculate total file size
		$totalFileSize = 0;
		foreach($crashReports as $crashReport)
			$totalFileSize += $crashReport->filesize;
				
		// Calc percent of disk quota
        if($this->crash_report_files_disc_quota<=0) // unlimited
            $percentOfDiskQuota = -1; 
        else
            $percentOfDiskQuota = 100*$totalFileSize/($this->crash_report_files_disc_quota*1024*1024);
		
		// Return file count
		return $count;
	}
	
	/**
	 * Returns count of bug attachment files in this project. 
	 * @param integer $totalFileSize On output, receives the total size in bytes of files.
	 * @param integer $percentOfDiskQuota On output, receives percent of disk quota.
	 * @param integer $appver Optional. If specified, count of files is calculated for the given version.
	 * @return integer count of bug attachment files.
	 */
	public function getBugAttachmentCount(&$totalFileSize, &$percentOfDiskQuota, $appver = Project::PROJ_VER_ALL)
	{
		// Find all bug attachment files belonging to this project and having
		// $appver application version.
		$criteria=new CDbCriteria;								
		$criteria->compare('b.project_id', $this->id, false, 'AND');		
		if($appver!=Project::PROJ_VER_ALL)
		{
			$criteria->compare('b.appversion_id', $appver, false, 'AND');
		}
		$criteria->join = 'LEFT JOIN {{bug_change}} bc ON t.bug_change_id=bc.id';
		$criteria->join .= ' LEFT JOIN {{bug}} b ON bc.bug_id=b.id';		
		$bugAttachments = BugAttachment::model()->findAll($criteria);
		
		// Calculate count of bug attachments
		$count = count($bugAttachments);
		
		// Calculate total file size
		$totalFileSize = 0;
		foreach($bugAttachments as $bugAttachment)
			$totalFileSize += $bugAttachment->filesize;
				
		// Calc percent of disk quota
        if($this->bug_attachment_files_disc_quota<=0)
            $percentOfDiskQuota = -1;
        else            
            $percentOfDiskQuota = 100*$totalFileSize/($this->bug_attachment_files_disc_quota*1024*1024);
		
		// Return file count
		return $count;
	}
	
	/**
	 * Returns the array of top crash groups. 
	 * @param Optional. If specified, search is performed for the given version.
	 * @return array The list of CrashGroup models.
	 */
	public function getTopCrashGroups($appver = null)
	{
		$criteria=new CDbCriteria;				
		$criteria->select = 't.*, COUNT({{crashreport}}.id) AS crashReportCount';
		$criteria->compare('t.project_id', $this->id, false, 'AND');		
		if($appver!=null && $appver!=-1)
		{
			$criteria->compare('t.appversion_id', $appver, false, 'AND');
		}		
		$criteria->join = 'INNER JOIN {{crashreport}} ON {{crashreport}}.groupid = t.id';
		$criteria->group = 't.id';		
		$criteria->order = 'crashReportCount DESC';
		$criteria->limit = 10;
		
		$crashGroups = CrashGroup::model()->findAll($criteria);
		
		return $crashGroups;
	}
	
	/**
	 * Returns the array of recent bugs. 
	 * @param Optional. If specified, search is performed for the given version.
	 * @return boolean The list of BugChange models.
	 */
	public function getRecentBugChanges($appver = null)
	{
		$criteria=new CDbCriteria;						
		$criteria->compare('bug.project_id', $this->id, false, 'AND');
		if($appver!=null)
		{
			if($appver!='-1' /*(all)*/)				
				$criteria->compare('bug.appversion_id', $appver, false, 'AND');
		}		
		$criteria->order = 'crashReportCount DESC';
		$criteria->with = 'bug';
		$criteria->order = 't.timestamp DESC';		
		$criteria->limit = 10;
		
		$bugChanges = BugChange::model()->findAll($criteria);
		
		return $bugChanges;
	}
	
	/**
	 * Returns count of debug info files in this project.
	 * @param integer  $totalFileSize Total file size in bytes.
	 * @param integer $percentOfDiskQuota Percent of disk quita.
	 * @return integer file count.
	 */
	public function getDebugInfoCount(&$totalFileSize, &$percentOfDiskQuota)
	{
		// Find all debug info files belonging to this project and having
		// $appver application version.
		$criteria=new CDbCriteria;				
		$criteria->compare('project_id', $this->id, false, 'AND');
		$debugInfoFiles = DebugInfo::model()->findAll($criteria);
		
		// Calculate count of debug info
		$count = count($debugInfoFiles);
		
		// Calculate total file size
		$totalFileSize = 0;
		foreach($debugInfoFiles as $debugInfo)
			$totalFileSize += $debugInfo->filesize;
				
		// Calc percent of disk quota
        if($this->debug_info_files_disc_quota<=0)
            $percentOfDiskQuota = -1;
        else
            $percentOfDiskQuota = 100*$totalFileSize/($this->debug_info_files_disc_quota*1024*1024);
		
		// Return file count
		return $count;
	}
}

