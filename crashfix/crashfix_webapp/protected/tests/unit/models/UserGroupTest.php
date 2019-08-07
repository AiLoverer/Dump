<?php

/**
 *   This test case contains tests for User model class.
 */
class UserGroupTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'users'=>':test_user',
		'usergroups'=>':test_usergroup',
		'projects'=>':test_project',
		'user_project_access'=>':test_user_project_access',		
    );
		
    public function testCreateNewGroup()
	{		
		// Create new UserGroup model
		$model = new UserGroup();
		$model->name = 'Admin';
		$model->description = 'Test Group';
		$model->gperm_access_admin_panel=1;
		$model->pperm_browse_crash_reports=1;
		$model->pperm_browse_bugs=1;
		$model->pperm_browse_debug_info=1;
		$model->pperm_manage_crash_reports=1;
		$model->pperm_manage_bugs=1;
		$model->pperm_manage_debug_info=1;
		$model->default_sidebar_tab = 'Digest';
		$model->default_bug_status_filter = 'open';
			
		// Validate model - assume failure (group name already exist)
		$this->assertFalse($model->validate());
				
		// Correct name		
		$model->name = 'NewGroup';
			
		// Validate model - assume success 
		$this->assertTrue($model->validate());		
				
		// Apply changes to database - assume success
		$this->assertTrue($model->save());
						
		// Ensure group's status is Active
		$this->assertTrue($model->status==UserGroup::STATUS_ACTIVE);
		
		// Check the new group is not standard
		$this->assertFalse($model->isStandard());
		
		// Check the new group can be updated
		$this->assertTrue($model->canUpdate());
		
		// Delete new group - should succeed
		$this->assertTrue($model->delete());
		
	}	
	
	public function testDelete()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Try to delete group Admins - should fail (cant delete standard groups)
		$this->assertFalse($model->delete());
	}
	
	public function testAttributeLabels()
	{
		$model = new UserGroup;
		
		// Check attribute label exists
		$attrLabels = $model->attributeLabels();
		$this->assertTrue(isset($attrLabels['name']));		
	}
	
	public function testSearchUsers()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Search users belonging this group - this will return a data provider
		$dataProvider = $model->searchUsers();		
		// Expect one user found (root).
		$this->assertTrue(count($dataProvider->data)==1);		
	}
	
	public function testEnable()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Try to disable this group  - assume failure (cant disable standard group)
		$this->assertFalse($model->enable(false));		
		
		// Find group Dev
		$model = UserGroup::model()->find('name="Dev"');
		// Disable - should succeed
		$this->assertTrue($model->enable(false));
		$this->assertTrue($model->status==UserGroup::STATUS_DISABLED);
		// Enable again - should succeed
		$this->assertTrue($model->enable(true));
		$this->assertTrue($model->status==UserGroup::STATUS_ACTIVE);		
	}
	
	public function testHasUsers()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		// Ensure the group contains users (root)
		$this->assertTrue($model->hasUsers());
		
		// Find group QA
		$model = UserGroup::model()->find('name="QA"');
		$this->assertTrue($model!=Null);
		// Ensure the group contains no users
		$this->assertFalse($model->hasUsers());
	}
	
	public function testGetSidebarItemList()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Get sidebar item list
		$list = $model->getSidebarItemList();
		// Ensure the list contains 6 items
		$this->assertTrue(count($list)==6);
		// Ensure ths list contains item 'Digest'
		$this->assertTrue(isset($list['Digest']));				
	}
	
	public function testGetDefaultSidebarTabLabel()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Ensure the default tab is 'Digest'
		$this->assertTrue($model->getDefaultSidebarTabLabel()=='Digest');		
	}
	
	public function testGetBugStatusFilterItemList()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Get bug status filter item list
		$list = $model->getBugStatusFilterItemList();
		// Ensure ths list contains item 'open'
		$this->assertTrue(isset($list['open']));				
	}
	
	public function testGetBugStatusFilterLabel()
	{
		// Find group Admins
		$model = UserGroup::model()->find('name="Admin"');
		$this->assertTrue($model!=Null);
		
		// Ensure the default bug status filter is 'open'
		$this->assertTrue($model->getDefaultBugStatusFilterLabel()=='Open bugs');		
		
		// Find group QA
		$model = UserGroup::model()->find('name="QA"');
		$this->assertTrue($model!=Null);
		
		// Ensure the default bug status filter is 'verify'
		$this->assertTrue($model->getDefaultBugStatusFilterLabel()=='Bugs to verify');		
	}
}