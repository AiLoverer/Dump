<?php

/**
 *   This test case contains tests for Bug model class.
 */
class BugTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'users'=>':test_user',
		'usergroups'=>':test_usergroup',
		'projects'=>':test_project',
        'users'=>':test_user',		
		'user_project_access'=>':test_user_project_access',	
		'crashreports'=>':test_crashreport',	
		'crashgroups'=>':test_crashgroup',	
		'appversions'=>':test_appversion',	
		'bugs'=>':test_bug',	
    );
	
    public function testChange()
	{
		// Some cheating
		$_SERVER["SERVER_NAME"] = '127.0.0.1';
		
        // Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
        // Create new bug without filling in all fields
		$bug = new Bug;		
        $bug->summary = "";
		$bug->description = "";		
		$bug->status = Bug::STATUS_ACCEPTED;
		$bug->priority = Bug::PRIORITY_HIGH;
		$bug->reproducability = Bug::REPRO_NOT_TRIED;		
		// Apply changes - assume failure (summary, desc, crashreports not specified)
		$opened = $bug->open();		
		$this->assertFalse($opened);
        
		// Create new bug correctly
		$bug = new Bug;
		$bug->summary = "Bug on Crash Report #3";
		$bug->description = "Some description";		
		$bug->assigned_to = 1; 
		$bug->status = Bug::STATUS_ACCEPTED;
		$bug->priority = Bug::PRIORITY_HIGH;
		$bug->reproducability = Bug::REPRO_NOT_TRIED;
		$bug->crashreports = "3";					
		// Apply changes (save new bug) - assume success
		$opened = $bug->open();		
		$this->assertTrue($opened);
		
		// Find the bug we've just created - should succeed
		$bug = Bug::model()->findByAttributes(array('summary'=>'Bug on Crash Report #3'));
		$this->assertTrue($bug!=null);
		        
        // Change bug - set owner nobody
		$attributes = array(
			'assigned_to'=>-1);		
		// Apply changes - should succeed
		$changed = $bug->change($attributes);
        $this->assertTrue($changed);		
        
        // Change bug - set owner root
		$attributes = array(
			'assigned_to'=>1);		
		// Apply changes - should succeed
		$changed = $bug->change($attributes);
        $this->assertTrue($changed);		
                
		// Change bug - multiple changes
		$attributes = array(
			'status'=>Bug::STATUS_FIXED,
			'comment'=>"I fixed it! Whoaaa!",
			'fileAttachment'=>'',
			'reproducability'=>Bug::REPRO_ALWAYS,            
			'assigned_to'=>-1);
		
		// Apply changes - should succeed
		$changed = $bug->change($attributes);
        $this->assertTrue($changed);		
				
		// Modify bug again - set status to New
		$attributes = array(
			'status'=>Bug::STATUS_NEW,
			'comment'=>"Not so fast!",
			'fileAttachment'=>'',
			'reproducability'=>Bug::REPRO_SOMETIMES,
			'assigned_to'=>2);
		
		// Apply changes - should succeed
		$changed = $bug->change($attributes);
		$this->assertTrue($changed);	
		
		// Modify bug again - mark as duplicate
		$attributes = array(
			'fileAttachment'=>'',
			'status'=>Bug::STATUS_DUPLICATE,
			'merged_into'=>$bug->id
			);
		
		// Apply changes - should fail, because cant merge into itself
		$changed = $bug->change($attributes);
		$this->assertFalse($changed);		
				
		// Modify bug again 
		$attributes = array(
			'fileAttachment'=>'',
			'status'=>Bug::STATUS_DUPLICATE,
			'merged_into'=>2
			);
		
		// Apply changes - - should fail (can not merge into another project's bug)
		$changed = $bug->change($attributes);
		$this->assertFalse($changed);		
		
		// Modify bug again 
		$attributes = array(
			'fileAttachment'=>'',
			'status'=>Bug::STATUS_DUPLICATE,
			'merged_into'=>1
			);
		
		// Apply changes - - should succeed
		$changed = $bug->change($attributes);
		$this->assertTrue($changed);	
		
		// Try to enter very long comment
		// Modify bug again - reopen it
		$attributes = array(			
			'comment'=>str_repeat('1234567890', 1024),
			'fileAttachment'=>'');
		// Apply changes - should fail
		$changed = $bug->change($attributes);
		$this->assertFalse($changed);	
	}	
    
    public function testAttributeLabels()
	{	
		$model = new Bug;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['date_created']=='Created');
		$this->assertTrue($attrLabels['crashreports']=='Crash reports associated with this bug');						
	}	
	
	public function testOpenNewBug()
	{		
		// Some cheating
		$_SERVER["SERVER_NAME"] = '127.0.0.1';
		
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
				
		// Create new bug
		$bug = new Bug('create');
		$bug->summary = "Bug on Crash Report #1";
		$bug->description = "Some description";		
		$bug->assigned_to = 2; 
		$bug->status = Bug::STATUS_NEW;
		$bug->priority = Bug::PRIORITY_MEDIUM;
		$bug->reproducability = Bug::REPRO_ALWAYS;
				
		// Validate - should fail (crashreports or crashgroups should be set)
		$this->assertFalse($bug->validate());
		
		// Assign incorrect crashreports attribute
		$bug->crashreports = "1 abc";		
		// Validate - should fail 
		$this->assertFalse($bug->validate());
		
		// Assign incorrect crashreports attribute - not existing ID
		$bug->crashreports = "10";		
		// Validate - should fail 
		$this->assertFalse($bug->validate());
		
		// Assign correct crashreport IDs
		$bug->crashreports = "3 4";		
		// Ensure the info is correct
		$this->assertTrue($bug->validate());
	
		// Assign crashgroup IDs
		$bug->crashgroups = '3';
						
		// Apply changes
		$opened = $bug->open();
        //$err = $bug->getErrors();
        //var_dump($err);
		// Should succeed
		$this->assertTrue($opened);
		
	}
	
	public function testAutoFillSummary()
	{
		// Some cheating
		$_SERVER["SERVER_NAME"] = '127.0.0.1';
				
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create new bug
		$bug = new Bug;
		$bug->crashreports='3';		
		$bug->autoFillSummary();		
		$bug->assigned_to = 2; 
		$bug->status = Bug::STATUS_NEW;
		$bug->priority = Bug::PRIORITY_MEDIUM;
		$bug->reproducability = Bug::REPRO_ALWAYS;
				
		// Apply changes
		$opened = $bug->open();
		// Should succeed
		$this->assertTrue($opened);
		
		// Create new bug
		$bug = new Bug;
		$bug->crashgroups='3';		
		$bug->autoFillSummary();		
		$bug->assigned_to = 2; 
		$bug->status = Bug::STATUS_NEW;
		$bug->priority = Bug::PRIORITY_MEDIUM;
		$bug->reproducability = Bug::REPRO_ALWAYS;
		
		// Apply changes
		$opened = $bug->open();
		// Should succeed
		$this->assertTrue($opened);
	}
	
	public function testDelete()
	{
		// Create new bug
		$bug = new Bug;
		$bug->summary = "Bug on Crash Report #3";
		$bug->description = "Some description";		
		$bug->assigned_to = 1; 
		$bug->status = Bug::STATUS_ACCEPTED;
		$bug->priority = Bug::PRIORITY_HIGH;
		$bug->reproducability = Bug::REPRO_NOT_TRIED;
		$bug->crashreports = "3";
		
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
				
		// Apply changes
		$opened = $bug->open();
		// Should succeed
		$this->assertTrue($opened);
		
		// Find the bug
		$bug = Bug::model()->findByAttributes(array('summary'=>'Bug on Crash Report #3'));
		$this->assertTrue($bug!=null);
		
		// Delete the bug
		$this->assertTrue($bug->delete());
	}
	
	public function testSimpleSearch()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
        // Set cur project for search
        Yii::app()->user->setCurProjectId(1);
        Yii::app()->user->setCurProjectVer(1);
                
		// Test simple search
		$bug = new Bug('search');
		$bug->isAdvancedSearch = false;
		$bug->filter = 'Bug on';		
		$dataProvider = $bug->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);		
		
		// Change bug status filter
		$bug->bugStatusFilter = 'reported';
		$dataProvider = $bug->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);		
		
		// Change bug status filter
		$bug->bugStatusFilter = 'owned';
		$dataProvider = $bug->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);		
		
		// Change bug status filter
		$bug->bugStatusFilter = 'verify';
		$dataProvider = $bug->search();				
		$this->assertTrue(count($dataProvider->data)==0);		
		
		// Change bug status filter
		$bug->bugStatusFilter = 'closed';
		$dataProvider = $bug->search();				
		$this->assertTrue(count($dataProvider->data)==0);		
		
		// Change bug status filter
		$bug->bugStatusFilter = 'all';
		$dataProvider = $bug->search();				
		$this->assertTrue(count($dataProvider->data)!=0);		
	}
	
	public function testAdvancedSearch()
	{	
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Test advanced search
		$bug = new Bug('search');
		$bug->isAdvancedSearch = true;
		$bug->status = Bug::STATUS_NEW;
		$bug->reported_by = 'root';
		$bug->createdFrom = 1234567;
		$bug->createdTo = 123456789;
		$dataProvider = $bug->search();
		$this->assertTrue(count($dataProvider->data)==0);
	}
	
	public function testGenerateBugStatusDynamicsGraph365()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year bug statistics
		Bug::generateBugStatusDynamicsGraph(320, 240, 365, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		@unlink($outFile);
	}
	
	public function testGenerateBugStatusDynamicsGraph30()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year bug statistics
		Bug::generateBugStatusDynamicsGraph(320, 240, 30, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
	
	public function testGenerateBugStatusDynamicsGraph7()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year bug statistics
		Bug::generateBugStatusDynamicsGraph(320, 240, 7, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}	
}