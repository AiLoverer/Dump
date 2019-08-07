<?php

/**
 * This is the model class for table "{{crashgroup}}".
 *
 * The followings are the available columns in table '{{crashgroup}}':
 * @property integer $id
 * @property string $created
 * @property integer $status
 * @property string $title
 */
class CrashGroup extends CActiveRecord
{
	// Status constants
	const STATUS_NEW = 1; // New group
	
	// Search related variables
	public $filter; // Simple search filter.
	public $bugStatusFilter; // Bug status filter.
	
	/**
	 * Returns the static model of the specified AR class.
	 * @return CrashGroup the static model class
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
		return '{{crashgroup}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		$rules = array(			
			//array('created, title', 'length', 'max'=>32),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, created, status, title', 'safe', 'on'=>'search'),
		);
		
		if($this->scenario!='search')
			$rules[] = array('title', 'required');
		
		return $rules;
	}

	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
			'crashReports'=>array(self::HAS_MANY, 'CrashReport', 'groupid'),
			'crashReportCount'=>array(self::STAT, 'CrashReport', 'groupid'),
			'bugs'=>array(self::HAS_MANY, 'BugCrashGroupMapping', 'crashgroup_id'),
			'bugCount'=>array(self::STAT, 'BugCrashGroupMapping', 'crashgroup_id'),
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'created' => 'Date Created',
			'status' => 'Status',
			'title' => 'Title',
			'crashReportCount' => 'Reports',
		);
	}

	/**
	 * This method is executed before AR is saved to database.
	 * @return boolean true on success.
	 */
	protected function beforeSave()
	{
		// Call base class
		if(!parent::beforeSave())
			return false;
		
		// Set 'status' attribute
		$this->status = self::STATUS_NEW;			
		
		// Set 'created' attribute
		$this->created = time();
		
		// OK
		return true;
	}
	
	/**
	 * This method is executed before AR is deleted from database.
	 * @return boolean True on success.
	 */
	protected function beforeDelete()
	{
		// Call parent class.
		if(!parent::beforeDelete())	
			return false;
				
		// Remove all related crash reports
		foreach($this->crashReports as $crashReport)
		{
			$crashReport->delete();
		}
				
		// Success.
		return true;		
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
		
		// Check that current project selected.
		$curProjectId = Yii::app()->user->getCurProjectId();		
		if($curProjectId==false)
			return Null;

        if(!isset($this->bugStatusFilter))
			$this->bugStatusFilter = 'open';
		        
		$criteria=new CDbCriteria;
        
        $criteria->compare('t.project_id', $curProjectId, false, 'AND');
		$curProjectVer = false;
		Yii::app()->user->getCurProjectVersions($curProjectVer);
		if($curProjectVer!=-1)
			$criteria->compare('t.appversion_id', $curProjectVer, false, 'AND');
		        
		// Simple search filter
		if(isset($this->filter))
		{
			$criteria->compare('title', $this->filter, true, 'AND');			
		}
				
		$criteria->compare('id', $this->id);
		$criteria->select = 't.*, COUNT({{crashreport}}.id) AS crashReportCount';
		$criteria->compare('created',$this->created, true);
		$criteria->compare('t.status', $this->status);
		$criteria->compare('title', $this->title, true);		
        
        if($this->bugStatusFilter=='open')
		    $criteria->join = 'INNER JOIN {{crashreport}} ON {{crashreport}}.groupid = t.id';
        else
            $criteria->join = 'LEFT JOIN {{crashreport}} ON {{crashreport}}.groupid = t.id';
            
		$criteria->group = 't.id';
		
		if($this->bugStatusFilter=='open')
		{			
			$criteria->select .= ', b.status AS bug_status';
			$criteria->addCondition('(b.status IS NULL OR b.status<'.Bug::STATUS_OPEN_MAX.')', 'AND');			
			$criteria->join .= ' LEFT JOIN {{bug_crashgroup}} bc ON bc.crashgroup_id = t.id LEFT JOIN {{bug}} b ON bc.bug_id = b.id';
		}
		else if($this->bugStatusFilter!='all')
			throw new CHttpException(403, 'Invalid request.');
					
		
		$sort = new CSort();
		$sort->attributes =array(
			'crashReportCount' => array(
               'asc'=>'crashReportCount ASC',
               'desc'=>'crashReportCount DESC',
			),
			'*' // add all other columns to sort
		);
		$sort->defaultOrder = 'crashReportCount DESC';
		
		return new CActiveDataProvider('CrashGroup', array(
			'criteria'=>$criteria,
			'sort'=>$sort,
            'pagination'=>array(
                'pageSize'=>50,
            ),
		));
	}
	
	/**
	 * Returns count of crash reports in this collection and other information. 
	 * @param $appver Application version.
	 * @param $totalFileSize On output, receives total file size.
	 * @param $percentOfQuota On output, receives percent of quota.
	 * @throws CHttpException
	 * @return integer count of crash reports.
	 */
	public function getCrashReportCount($appver, &$totalFileSize, &$percentOfQuota)
	{
		// Get currently selected project		
		$curProject = Yii::app()->user->getCurProject();
		if($curProject===null)
			throw new CHttpException(403, 'Invalid request');
				
		// Find all crash reports belonging to this collection		
		$criteria=new CDbCriteria;						
		/*$criteria->compare('project_id', $curProject->id, false, 'AND');
		if($appver!=-1)
			$criteria->compare('appversion_id', $appver, false, 'AND');*/
        $criteria->compare('groupid', $this->id, false, 'AND');
		$crashReports = CrashReport::model()->findAll($criteria);
		
		// Calculate count of crash reports
		$count = count($crashReports);
		
		// Calculate total file size
		$totalFileSize = 0;
		foreach($crashReports as $crashReport)
			$totalFileSize += $crashReport->filesize;
		
		// Calc percent of quota
        if($curProject->crash_reports_per_group_quota<=0)
            $percentOfQuota = -1;
        else
            $percentOfQuota = 100*$count/$curProject->crash_reports_per_group_quota;
				
		// Return file count
		return $count;
	}
	
	/**
	 * This method returns a string containing the list of bugs associated with
	 * this crash group.
	 * @return string list of bugs.
	 */
	public function formatBugListStr()
	{
		if(count($this->bugs)==0)
			return '';
		
		$bugList = '';
		$bugNum = 0;
		foreach($this->bugs as $bugCrashGroup)
		{	
			if($bugNum>1)
			{
				$bugList .= ' ...';
				break;
			}
						
			$bugList .= CHtml::link('#'.$bugCrashGroup->bug_id, array('bug/view', 'id'=>$bugCrashGroup->bug_id)).' ';	
			$bugNum++;						
		}
        
        return $bugList;
	}
    
    /**
     * Returns count of different IP addresses that contributed crash reports
     * into this collection.
     * @return integer Count.
     */
    public function getDistinctIPs()
    {
        $criteria = new CDbCriteria();
        $criteria->compare('groupid', $this->id);
        $criteria->group = 'ipaddress';
        $criteria->distinct = true;
        
        $count = CrashReport::model()->count($criteria);
        return $count;
    }
	
	/**
	 * Determines if a bug can be opened for this group. A bug can not be opened
	 * if the crash group is empty or does not contain processed crash reports.
	 * @return boolean true if a bug can be opened.
	 */
	public function canOpenNewBug()
	{
		// Calculate count of processed reports in this group
		$criteria = new CDbCriteria;
		$criteria->compare('groupid', $this->id);
		$criteria->compare('status', CrashReport::STATUS_PROCESSED);
		
		$count = CrashReport::model()->count($criteria);
		if($count==0)
			return false; // No such reports
		
		// OK
		return true;
	}
}