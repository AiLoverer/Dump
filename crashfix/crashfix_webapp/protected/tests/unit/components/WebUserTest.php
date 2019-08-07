<?php

/**
 *   This test case contains tests for WebUser component class.
 */
class WebUserTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'users'=>':test_user',
		'usergroups'=>':test_usergroup',
		'projects'=>':test_project',
		'user_project_access'=>':test_user_project_access',	
		'crashreports'=>':test_crashreport',	
		'appversions'=>':test_appversion',	
    );
		
    public function testGetMyProjects()
	{		
		// Create new login form model, use 'RegularLogin' scenario
		$model = new LoginForm('RegularLogin');		
		// Login as root
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Get the list of projects for root
		$myProjects = Yii::app()->user->getMyProjects();
		// root is a member of CrashRpt Tests project
		$this->assertTrue(count($myProjects)==1);				
	}	
	
	public function testGetCurProjectId()
	{
		// Create new login form model, use 'RegularLogin' scenario
		$model = new LoginForm('RegularLogin');		
		// Login as root
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Get current project ID.
		$curProjectId = Yii::app()->user->getCurProjectId();
		// Ensure that current project ID==1 (root's project)
		$this->assertTrue($curProjectId==1);
		
		// Get cur project model
		$project = Yii::app()->user->getCurProject();
		// Ensure that project name is 'CrashRpt Tests'
		$this->assertTrue($project->name=='CrashRpt Tests');
		
		// Log out
		Yii::app()->user->logout();
				
		// Login as guest
		$model = new LoginForm('RegularLogin');	
		$model->username = "guest";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Get current project ID.
		$curProjectId = Yii::app()->user->getCurProjectId();
		// Ensure that current project ID==false (guest has no projects assigned)		
		$this->assertTrue($curProjectId==false);
		
		// Get cur project model
		$project = Yii::app()->user->getCurProject();
		// Ensure that project is null
		$this->assertTrue($project===null);
	}
	
	public function testSetCurProjectId()
	{
		// Login as guest2
		$model = new LoginForm('RegularLogin');	
		$model->username = "guest2";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Get current project ID.
		$curProjectId = Yii::app()->user->getCurProjectId();
		// Ensure that current project ID==1 
		$this->assertTrue($curProjectId==1);
		
		// Set cur project ID to 2 (wpack)
		$this->assertTrue(Yii::app()->user->setCurProjectId(2));
		// Get current project ID.
		$curProjectId = Yii::app()->user->getCurProjectId();
		// Ensure that current project ID==2 
		$this->assertTrue($curProjectId==2);
		
		// Set cur project ID to invalid number
		$this->assertFalse(Yii::app()->user->setCurProjectId(100));
		// Ensure that current project ID is still 2
		$curProjectId = Yii::app()->user->getCurProjectId();
		$this->assertTrue($curProjectId==2);		
	}
	
	public function testLoadModel()
	{
		// Login as guest
		$model = new LoginForm('RegularLogin');	
		$model->username = "guest";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Load User model
		$user = Yii::app()->user->loadModel();
		$this->assertTrue($user!=null);
		
		// Load User model again
		$user = Yii::app()->user->loadModel();
		$this->assertTrue($user!=null);
	}	
		
	/**	
	 * @expectedException CHttpException 
	 */
	public function testGetCurProjectVersionsWhenProjectNotAssigned()
	{
		// Login as guest
		$model = new LoginForm('RegularLogin');	
		$model->username = "guest";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Check that cur project ID is false (no projects assigned to guest)
		$this->assertTrue(Yii::app()->user->getCurProjectId()==false);
		
		// This should cause exception
		$curVer = false;
		$versions = Yii::app()->user->getCurProjectVersions($curVer);		
	}
	
	public function testGetCurProjectVersions()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Ensure that current project is 'CrashRpt Tests'
		$curProject = Yii::app()->user->getCurProject();
		$this->assertTrue($curProject!=null);
		$this->assertTrue($curProject->name=='CrashRpt Tests');
		
		// Get list of versions for cur project
		$curVer = false;
		$versions = Yii::app()->user->getCurProjectVersions($curVer);
		// Ensure there are three versions
		$this->assertTrue(count($versions)==4);
		$this->assertTrue($curVer==1);		
		
		// Set invalid version to session data
		Yii::app()->session['curProjectVer'] = array(10=>'1.1.101');
		// Get list of versions for cur project
		$curVer = false;
		$versions = Yii::app()->user->getCurProjectVersions($curVer);
		// Ensure there are 3 versions
		$this->assertTrue(count($versions)==4);		
		$this->assertTrue($curVer==1);		
				
	}
	
	public function testGetCurProjectVer()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Get cur ver
		$curVer = Yii::app()->user->getCurProjectVer();
		$this->assertTrue($curVer==1);
		
		// Set another ver
		$set = Yii::app()->user->setCurProjectVer(2);
		$this->assertTrue($set);
		
		// Get cur ver
		$curVer = Yii::app()->user->getCurProjectVer();
		$this->assertTrue($curVer==2);
		
		// Set invalid ver - assume failed
		$set = Yii::app()->user->setCurProjectVer(10);
		$this->assertFalse($set);
	}
}