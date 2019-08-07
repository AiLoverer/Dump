<?php

/**
 * This is the model class for table "{{usergroup}}".
 * 
 * This class represents a user group (a group of users). A group may contain
 * zero or more users. Users within the same group has the same permissions
 * and UI preferences.
 * 
 * A group may be standard (like 'Admin') or not standard. A standard group
 * can't be deleted. Admin group also can't be updated and disabled.
 * 
 * A group may be active or disabled. If group is disabled, all users within
 * the group also become disabled.
 * 
 * A group can be deleted if it is not standard and contains no users.
 */
class UserGroup extends CActiveRecord
{
	// User group statuses
	const STATUS_ACTIVE    = 1; // This group is active.
	const STATUS_DISABLED  = 2; // This group is disabled. 
	
	// User group flags
	const FLAG_STANDARD_GROUP  = 0x1; // This group is standard.
	const FLAG_CANT_DISABLE    = 0x2; // This group can't be disabled.
	const FLAG_CANT_UPDATE     = 0x4; // This group can't be updated.
	
	/**
	 * Returns the static model of the specified AR class.
	 * @return UserGroup the static model class
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
		return '{{usergroup}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		return array(
			array('name', 'required'),			
			array('name', 'length', 'max'=>32),
			array('name', 'checkGroupNameFree'),
			array('description', 'length', 'max'=>128),			
			array('gperm_access_admin_panel', 'required'),			
			array('pperm_browse_crash_reports, pperm_browse_bugs, pperm_browse_debug_info, pperm_manage_crash_reports, pperm_manage_bugs, pperm_manage_debug_info', 'required'),			
			array('pperm_browse_crash_reports, pperm_browse_bugs, pperm_browse_debug_info, pperm_manage_crash_reports, pperm_manage_bugs, pperm_manage_debug_info', 'boolean'),						
			array('default_sidebar_tab, default_bug_status_filter', 'required')
		);
	}

	/**
	 *   This validator method checks if provided usergroup name is free (not existing) or not.
	 *   @return boolean true if user doesn't exist, otherwise false.
	 */
	public function checkGroupNameFree($attribute, $params)
	{
		// Do not perform this check on update
		if(!$this->isNewRecord)
			return true;
			
		// Find group with such name
		$group = UserGroup::model()->find('name=:name', array(':name'=>$this->name));
		if($group==Null)
			return true;
		
		// Such a group name already exists.
		$this->addError('name', 'Such a group name already exists.');
		return false;
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
			'id' => 'ID',
			'name' => 'Name',
			'pperm_browse_crash_reports' => 'Browse Crash Reports',
			'pperm_browse_bugs' => 'Browse Bugs',
			'pperm_browse_debug_info' => 'Browse Debug Info',			
			'pperm_manage_crash_reports' => 'Manage Crash Reports',
			'pperm_manage_bugs' => 'Manage Bugs',
			'pperm_manage_debug_info' => 'Manage Debug Info',
			'gperm_access_admin_panel' => 'Access Admin Panel',			
		);
	}
	
	/**
	 * Retrieves a list of users belonging this group.
	 * @return CActiveDataProvider the data provider that can return the models based on the search/filter conditions.
	 */
	public function searchUsers()
	{
		$criteria=new CDbCriteria;
		$criteria->compare('usergroup', $this->id);				
		
		return new CActiveDataProvider('User', array(
			'criteria'=>$criteria,
		));
	}
	
	/**
	 * Sets group status to Active or Disabled.
	 * Cant change status of a standard group (Admins).
	 * @param boolean $enable If True, group is being enabled, otherwise disabled.
	 * @return boolean True on success. 
	 */
	public function enable($enable)
	{			
		// Change status appropriatelly
		if($enable==true)
			$this->status = self::STATUS_ACTIVE;			
		else
		{
			if(!$this->canDisable())
				return false; // Cant disable standard group (Admins)		
			
			$this->status = self::STATUS_DISABLED;			
		}
		
		// Apply changes.
		if(!$this->save())		
			return false;
		
		// Success
		return true;
	}
	
	/**
	 * Determines if this group is a standard group (like Admin).
	 * @return True if the group is a standard group, otherwise False.
	 */
	public function isStandard()
	{
		return 0!=($this->flags & self::FLAG_STANDARD_GROUP);		
	}
	
	/**
	 * Determines if this group can be disabled.
	 * Standard groups can't be updated.
	 * @return True if the group can be disabled, otherwise False.
	 */
	public function canDisable()
	{
		return 0==($this->flags & self::FLAG_CANT_DISABLE);		
	}
	
	/**
	 * Determines if this group can be updated.
	 * The standard group Admins can't be updated.
	 * @returns True if the group can be updated, otherwise False.
	 */
	public function canUpdate()
	{
		return 0==($this->flags & self::FLAG_CANT_UPDATE);		
	}
	
	/**
	 * Determines if this group has any users. 
	 * @return True if the group is non-empty, otherwise returns False.
	 */
	public function hasUsers()
	{
		// Find users belonging to this group.
		$users = User::model()->findAll('usergroup='.$this->id);
		
		// Return true if count of users is greater than zero.
		return 0!=(count($users));
	}
	
	/**
	 * This callback is executed before saving the AR to database.
	 * In this method, we set some attributes used internally (like 'status'
	 * and 'flags').
	 * @return boolean True on success; otherwise False.
	 */
    public function beforeSave()
    {
		// Check if this is a new record
        if($this->isNewRecord) 
		{
			// Reset flags
			$this->flags = 0;
			
			// Set status to Active
			$this->status = self::STATUS_ACTIVE;			
					
        }
		else
		{
			// Check if this group can be updated.
			if(!$this->canUpdate())
				return false;
		}
		
		// Done
        return true;
    }
	
	/**
	 * This callback is executed before deleting the AR from database.
	 * Here we check if the model can be deleted or not (standard groups
	 * cant be deleted).
	 * @return boolean True if successfull, otherwise False.
	 */
    public function beforeDelete()
    {
		// Do not allow to delete a standard group or a group having users.
		if($this->isStandard() || $this->hasUsers())
			return false;
		
		// Done
        return true;
    }
	
	/**
	 * This method returns the list of sidebar menu items (used in admin panel).
	 * @return array the list of sidebar items for this group. 
	 */
	public function getSidebarItemList()
	{
		$list = array();
		$list['Digest'] = 'Digest';
		$list['CrashReports'] = 'Crash Reports';
		$list['CrashGroups'] = 'Crash Groups';
		$list['Bugs'] = 'Bugs';
		$list['DebugInfo'] = 'Debug Info';
		$list['Administer'] = 'Administer';
		return $list;
	}
	
	/**
	 * This method returns the default sidebar tab label for this group.
	 * This info is used on admin panel.
	 * @return string Default sidebar item's string label.
	 */
	public function getDefaultSidebarTabLabel()
	{
		// Get the list of sidebar items
		$list = $this->getSidebarItemList();
		// Check if our default item is in the list
		if(array_key_exists($this->default_sidebar_tab, $list))
		{
			// Return item label
			return $list[$this->default_sidebar_tab];
		}
		
		// Error
		return '';
	}
	
	/**
	 * This method returns the list of bug status filter items.
	 * This info is used on admin panel.
	 * @return array list of bug status filter items. 
	 */
	public function getBugStatusFilterItemList()
	{
		$list = array();
		$list['all'] = 'All bugs';
		$list['open'] = 'Open bugs';
		$list['owned'] = 'Open and owned by me';
		$list['reported'] = 'Open and reported by me';		
		$list['verify'] = 'Bugs to verify';
		$list['closed'] = 'Closed bugs';		
		return $list;
	}

	/**
	 * This method returns the default bug status filter label.
	 * This info is used on admin panel.
	 * @return string default bug status filter label.
	 */
	public function getDefaultBugStatusFilterLabel()
	{
		// Get the list of bug status filters
		$list = $this->getBugStatusFilterItemList();
		// Check if ours is in the list
		if(array_key_exists($this->default_bug_status_filter, $list))
		{
			// Return item label.
			return $list[$this->default_bug_status_filter];
		}
		
		// Error.
		return '';
	}
}