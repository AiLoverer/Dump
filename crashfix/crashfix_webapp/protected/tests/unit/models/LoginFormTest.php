<?php

/**
 *   This test case contains tests for LoginForm model class.
 */
class LoginFormTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'users'=>':test_user'
    );
	
    public function testValidate_RegularLogin()
	{		
		// Create new login form model, use 'RegularLogin' scenario
		$model = new LoginForm('RegularLogin');
		
		// Try to validate with empty password - assume fails
		$model->username = "root";						
		$this->assertFalse($model->validate());
		
		// Set password and validate again - assume success
		$model->password = "rootpwd";
		$this->assertTrue($model->validate());
		
	}
	
	public function testValidate_OneTimeLogin()
	{
		// Create new login form model, use 'OneTimeLogin' scenario
		$model = new LoginForm('OneTimeLogin');
		
		// Try to validate with incorrect access token - assume failure
		$model->oneTimeAccessToken = "abcdefghijklmnopqrstuvw314534";						
		$this->assertFalse($model->validate());			
		$model->clearErrors();
		
		// Try to validate with correct access token - assume success
		$model->oneTimeAccessToken = "6c9016a509b565a0eb9a4d6c5fd7be0c";						
		$this->assertTrue($model->validate());			
	}
	
	public function testLogin()
	{	
		// Create new login form model
		$model = new LoginForm('RegularLogin');		
		
		// Specify correct username/password - assume success
		$model->username = "root";						
		$model->password = "rootpwd";
		$loginResult = $model->login();
		$this->assertTrue($loginResult);				
	}
	
	public function testLogin_incorrect_credentials()
	{	
		// Create new login form model
		$model = new LoginForm('RegularLogin');		
		
		// Specify incorrect username/password - assume failure
		$model->username = "root";						
		$model->password = "roobot";
		$loginResult = $model->login();
		$this->assertFalse($loginResult);				
	}
	
	public function testGetUserId()
	{
		// Create new login form model
		$model = new LoginForm('RegularLogin');
		
		// Check that user Id is invalid before login
		$this->assertEquals($model->getUserId(), False);
		
		// Login - assume success
		$model->username = "root";						
		$model->password = "rootpwd";		
		$this->assertTrue($model->login());
		
		// Check that user Id is valid (>0) after login
		$this->assertTrue($model->getUserId()>0);
	}	
}