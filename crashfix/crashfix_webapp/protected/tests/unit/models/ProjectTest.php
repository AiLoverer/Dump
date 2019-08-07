<?php

/**
 *   This test case contains tests for Project model class.
 */
class ProjectTest extends CDbTestCase
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
		'crashgroups'=>':test_crashgroup',	
		'appversions'=>':test_appversion',	
		'debuginfo'=>':test_debuginfo'
    );
		
	public function testCreateNewProject()
	{
		// Create new record
		$model = new Project;
		$model->name = 'SomeProject';
		$model->description = 'A very good project'; 
		$model->crash_report_files_disc_quota = 500*1024*1024;
		$model->crash_reports_per_group_quota = 100;
		$model->debug_info_files_disc_quota = 2*1024*1024*1024;
		
		// Apply changes
		$this->assertTrue($model->save());
		
		// Find newly created record
		$model = Project::model()->findAll('name="SomeProject"');
		$this->assertTrue($model!=null);
	}
	
	public function testDelete()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		
		// Delete
		$this->assertTrue($model->delete());
	}
	
	public function testEnable()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		
		// Disable
		$this->assertTrue($model->enable(false));
		// Enable
		$this->assertTrue($model->enable(true));
	}
	
	public function testCanAddUser()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		
		// Ensure there are users that can be added to project		
		$this->assertTrue($model->canAddUser());		
	}
	
    public function testAttributeLabels()
	{	
		$model = new Project;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['name']=='Name');
		$this->assertTrue($attrLabels['crash_reports_per_group_quota']=='Max crash reports per collection');						
	}	
	
	public function testSearch()
	{
		// Create model
		$model = new Project;
		// Search - this will create data provider
		$dataProvider = $model->search();
		// Ensure some data returned
		$this->assertTrue(count($dataProvider->data)!=0);
	}
	
	public function testSearchUsers()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		// Search - this will create data provider
		$dataProvider = $model->searchUsers();
		// Ensure there are two users
		$this->assertTrue(count($dataProvider->data)==2);
	}
	
	public function testGetTopCrashGroups()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		$this->assertTrue($model!=null);
				
		$crashGroups = $model->getTopCrashGroups();
		$this->assertTrue(count($crashGroups)==3);
		
		$crashGroups = $model->getTopCrashGroups(1);
		$this->assertTrue(count($crashGroups)==1);
	}
	
	public function testGetRecentBugChanges()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		$this->assertTrue($model!=null);
		
		// Get recent bug changes
		$bugChanges = $model->getRecentBugChanges();
		$this->assertTrue(count($bugChanges)==2);
		
		$bugChanges = $model->getRecentBugChanges(1);
		$this->assertTrue(count($bugChanges)==2);
	}
	
	public function testGetCrashReportCount()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		$this->assertTrue($model!=null);
		
		// Get crash report file count
		$totalFileSize = 0;
		$percentOfDiskQuota = 0;
		$count = $model->getCrashReportCount($totalFileSize, $percentOfDiskQuota);		
		// Ensure corect info
		$this->assertTrue($count==6);		
		
        // Get crash reportcount for particular app version
		$count = $model->getCrashReportCount($totalFileSize, $percentOfDiskQuota, 1);		
		// Ensure corect info
		$this->assertTrue($count==1);		
	}
	
	public function testGetDebugInfoCount()
	{
		// Find 'CrashRpt Tests' project
		$model = Project::model()->find('name="CrashRpt Tests"');
		$this->assertTrue($model!=null);
		
		// Get debug info file count
		$totalFileSize = 0;
		$percentOfDiskQuota = 0;
		$count = $model->getDebugInfoCount($totalFileSize, $percentOfDiskQuota);
		
		// Ensure corect info
		$this->assertTrue($count==1);
		$this->assertTrue($totalFileSize==1024);
		$this->assertTrue($percentOfDiskQuota<1);
		
	}
}