<?php

/**
 *   This test case contains tests for AppVersion model class.
 */
class AppVersionTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		'appversions'=>':test_appversion',			
		'projects'=>':test_project',					
		'users'=>':test_user',					
    );
	
	public function testCreateIfNotExists()
	{
		// Create new version - assume success
		$model = AppVersion::createIfNotExists('1.4.0.0', 1);
		$this->assertTrue($model!=Null);
		
		// Find the new model
		$model = AppVersion::model()->find("version='1.4.0.0'");
		$this->assertTrue($model!=Null);
	}
	
	public function testCreateIfNotExistsNullProject()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create new version - assume success
		$model = AppVersion::createIfNotExists('1.5.0.0');
		$this->assertTrue($model!=Null);
		
		// Find the new model
		$model = AppVersion::model()->find("version='1.5.0.0'");
		$this->assertTrue($model!=Null);
	}
}