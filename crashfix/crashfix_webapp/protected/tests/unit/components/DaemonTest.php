<?php

/**
 *   This test case contains tests for Daemon component class.
 */
class DaemonTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        
    );
	
    public function testCheckDaemon()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Check daemon - assume success
		$realCheck = false;
        $errorMsg = '';
		$check = Yii::app()->daemon->checkDaemon($realCheck, $errorMsg);
		$this->assertTrue($check==Daemon::DAEMON_CHECK_OK);
		$this->assertTrue($realCheck);
		
		// Check daemon another time - assume success
		$check = Yii::app()->daemon->checkDaemon($realCheck,$errorMsg);
		$this->assertTrue($check==Daemon::DAEMON_CHECK_OK);
		$this->assertFalse($realCheck); // this time the result taken from session data
	}
	
    
    public function testGetDaemonStatus()
	{		
		// Get status
		$status = '';
		$code = Yii::app()->daemon->getDaemonStatus($status);
				
		// Assume status code == 0
		$this->assertTrue($code==0);
		// Assume status message is 'Success'
		$this->assertTrue($status=='The daemon is functioning correctly, no problems detected.');
	}	
	
	public function testGetLicenseInfo()
	{
		// Get license information
		$licenseInfo = Yii::app()->daemon->getLicenseInfo();
		
		// Ensure it is array
		$this->assertTrue(is_array($licenseInfo));
		
		$this->assertTrue($licenseInfo['ProductName']=='crashfix');
		$this->assertTrue($licenseInfo['ProductVersion']=='1.0');
		$this->assertTrue($licenseInfo['Name']=='Oleg');
		$this->assertTrue($licenseInfo['Surname']=='Krivtsov');
	}
    
    public function testGetConfigInfo()
	{
		// Get config information
		$configInfo = Yii::app()->daemon->getConfigInfo();
        $this->assertTrue(is_array($configInfo));
		
        $webRootDir = $configInfo['WebRootDir'].'protected';
        $webRootDir = str_replace('\\', '/', $webRootDir);
        
        $curDir = Yii::app()->basePath;
        $curDir = str_replace('\\', '/', $curDir);
                
		$this->assertTrue($curDir==$webRootDir);		
	}
	
}