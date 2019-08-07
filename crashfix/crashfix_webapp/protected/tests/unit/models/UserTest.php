<?php

/**
 *   This test case contains tests for User model class.
 */
class UserTest extends CDbTestCase
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
		
    public function testCreateNewUser()
	{		
		// Create new user model
		$model = new User('create');
		$model->username = 'root';
		$model->usergroup = 1; // Admins
		$model->password = 'rootpwd';		
		$model->email = 'test@localhost'; 
		
		// Validate model - assume failure (user name already exist)
		$this->assertFalse($model->validate());
		
		// Correct username		
		$model->username = 'newuser';
				
		// Validate model - assume success 
		$this->assertTrue($model->validate());		
		
		// Apply changes to database - assume success
		$this->assertTrue($model->save());
		
		// Ensure user's password is resetted
		$this->assertTrue($model->isPasswordResetted());
	}	
	
	public function testDeleteUser()
	{
		// Find the first user (root)
		$model = User::model()->findByPk(1);
		
		// Try to delete - assume failed (can't delete standard user root)
		$this->assertFalse($model->delete());
		
		// Find the second user (devel)
		$model = User::model()->findByPk(2);
		// Delete - assume success
		$this->assertTrue($model->delete());
		// Ensure there are no project roles for user #2
		$roles = UserProjectAccess::model()->findAll('user_id=2');
		$this->assertTrue(count($roles)==0);
	}
	
	public function testSearch()
	{
		// Create a new model containing search filter info
		$model = new User();
		$model->username = 'root'; // search by username
		$model->password = 'test@localhost'; // and by password
		// Search users - this will return a data provider
		$dataProvider = $model->search();		
		// Expect one user found
		$this->assertTrue(count($dataProvider->data)==1);
		
		$model->username = 'nobody'; // search by username only
		unset($model->password); 
		$dataProvider = $model->search();		
		// Expect nothing found
		$this->assertTrue(count($dataProvider->data)==0);
	}
	
	public function testAttributeLabels()
	{
		$model = new User;
		
		// Check attribute label exists
		$attrLabels = $model->attributeLabels();
		$this->assertTrue(isset($attrLabels['username']));
	}
	
	public function testRetireResurrect()
	{
		// Find the first user (root)
		$model = User::model()->find("username='root'");
		
		// Try to retire - should fail (can't disable root).
		$this->assertFalse($model->retire());
		
		// Find the second user (devel)
		$model = User::model()->find("username='devel'");
		// Try to retire - should succeed.
		$this->assertTrue($model->retire());		
		// Resurrect devel - should succeed
		$this->assertTrue($model->resurrect());
		// Ensure devel's status is Active
		$model = User::model()->find("username='devel'");
		$this->assertTrue($model->status==User::STATUS_ACTIVE);		
	}
	
	public function testCanAddProjectRole()
	{
		// Find the first user (root)
		$user = User::model()->find("username='root'");
		
		// Check that root can be added to one project 'wpack'				
		$this->assertTrue($user->canAddProjectRole());
		
		// Add user to the second project 'wpack'
		$role = new UserProjectAccess;
		$role->user_id = $user->id;
		$role->project_id = 2;
		$role->usergroup_id = 1; // make him admin
		$this->assertTrue($role->save());
        
        // Add user to the third project 'WTLDemo'
		$role = new UserProjectAccess;
		$role->user_id = $user->id;
		$role->project_id = 3;
		$role->usergroup_id = 1; // make him admin
		$this->assertTrue($role->save());
		
		// Check that root can be added to any project now
		$this->assertFalse($user->canAddProjectRole());
	}
	
	public function testFindAllNotInProject()
	{
		// Find all users not in project 'CrashRpt Tests'
		$users = User::findAllNotInProject(1);		
		// Should be two such users (devel and guest)
		$this->assertTrue(count($users)==2);
	}
	
	public function testGetEffectiveStatus()
	{
		// Find the second user (devel)
		$user = User::model()->find("username='devel'");
		$this->assertTrue($user!=Null);
						
		// Ensure his effective status is active
		$this->assertTrue($user->getEffectiveStatus()==User::STATUS_ACTIVE);
		
		// Ensure corect status string
		$this->assertTrue($user->getEffectiveStatusStr()=='Active');
		
		// Disable usergroup 
		$this->assertTrue($user->group->enable(false));
		
		// Ensure his effective status is diabled
		$this->assertTrue($user->getEffectiveStatus()==User::STATUS_DISABLED);
		
		// Ensure corect status string
		$this->assertTrue($user->getEffectiveStatusStr()=='Disabled (group)');
		
		// Enable usergroup, but disable user
		$this->assertTrue($user->group->enable(true));		
		$this->assertTrue($user->retire());
		
		// Ensure his effective status is diabled
		$this->assertTrue($user->getEffectiveStatus()==User::STATUS_DISABLED);
		
		// Ensure corect status string
		$this->assertTrue($user->getEffectiveStatusStr()=='Disabled');
	}		
    
    public function testIsInProject()
    {
        // Find the second user (devel)
		$user = User::model()->find("username='devel'");
		$this->assertTrue($user!=Null);
        
        // Ensure this user belongs to project 2 (wpack)
        $this->assertTrue($user->isInProject(2));
        
        // Ensure this user does not belong to project 1 (crashrpttests)
        $this->assertFalse($user->isInProject(1));
    }
	
    public function testSuggestProjectRole()
    {
        // Find the second user (devel)
		$user = User::model()->find("username='devel'");
		$this->assertTrue($user!=Null);
        
        $this->assertTrue($user->suggestProjectRole(1)==2);
        
        $this->assertTrue($user->suggestProjectRole(2)==2);
    }
}

