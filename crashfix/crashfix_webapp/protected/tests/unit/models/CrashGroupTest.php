<?php

/**
 *   This test case contains tests for CrashGroup model class.
 */
class CrashGroupTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		'projects'=>':test_project',
        'crashreports'=>':test_crashreport',	
		'appversions'=>':test_appversion',	
		'crashgroups'=>':test_crashgroup',	
    );
	
	public function testCreate()
	{
		// Create new crash group
		$model = new CrashGroup();
		$model->project_id = 1;
		$model->appversion_id = 1;
		$model->title = 'Crash group title';
		$model->md5 = '12345678901234567890123456789012';
		
		// Save - should succeed
		$saved = $model->save();
		$this->assertTrue($saved);
		
		// Find created model		
		$model = CrashGroup::model()->findByAttributes(array('title'=>'Crash group title'));
		// Ensure found
		$this->assertTrue($model!=null);
	}
	
	public function testDelete()
	{
		// Find a crash group
		$model = CrashGroup::model()->findByPk(1);
		// Ensure found
		$this->assertTrue($model!=null);
		
		// Delete
		$this->assertTrue($model->delete());
	}
	
	public function testAttributeLabels()
	{	
		$model = new CrashGroup;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['created']=='Date Created');
		$this->assertTrue($attrLabels['crashReportCount']=='Reports');						
	}	
	
	public function testSimpleSearch()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Test simple search without
		$crashGroup = new CrashGroup('search');
		$crashGroup->filter = 'Unsorted';		
		$dataProvider = $crashGroup->search();
		// Ensure nothing found
		$this->assertTrue(count($dataProvider->data)==1);
	}	
	
	public function testGetCrashReportCount()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Find a crash group
		$model = CrashGroup::model()->findByPk(1);
		// Ensure found
		$this->assertTrue($model!=null);
		
		// Get crash report count in group
		$totalFileSize = 0;
		$percentOfQuota = 0;
		$count = $model->getCrashReportCount(1, $totalFileSize, $percentOfQuota);		
		$this->assertTrue($count==1);
		$this->assertTrue($totalFileSize==23870);
		$this->assertTrue($percentOfQuota>0);
	}
}