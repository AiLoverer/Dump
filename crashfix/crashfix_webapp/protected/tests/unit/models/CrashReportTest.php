<?php

/**
 *   This test case contains tests for CrashReport model class.
 */
class CrashReportTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'crashreports'=>':test_crashreport',	
		'appversions'=>':test_appversion',	
		'crashgroups'=>':test_crashgroup',	
		'threads'=>':test_thread',	
		'modules'=>':test_module',	
		'fileItems'=>':test_fileitem',	
        'stackFrames'=>':test_stackframe',	
		'customProps'=>':test_customprop',	
		'processingErrors'=>':test_processingerror',	
    );
	
	private $_fileName;
	
	public function setUp()
	{
		parent::setUp();
		
		$dirName = Yii::app()->getBasePath()."/data/crashReports/4c6/d99";
        if(!is_dir($dirName))
            mkdir($dirName, 0777, true);	
		
		$fileName = $dirName."/4c6d99d7cd11906e7e9b146f3bb668ad.zip";
		$this->_fileName = $fileName;
		$f = copy(__DIR__.'/../../fixtures/files/4c6d99d7cd11906e7e9b146f3bb668ad.zip', $fileName);		
	}
	
	public function tearDown()
	{
		parent::tearDown();
		
        if(file_exists($this->_fileName))
            unlink($this->_fileName);
	}
	
	public function testCreate()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create temp file 
		$tmpfile1 = tempnam(Yii::app()->getRuntimePath(), "test");
		$tmpfile = $tmpfile1 . '.zip';
		$f = fopen($tmpfile, 'w');
		fwrite($f, 'MZP1234567');
		fclose($f);		
		
		// Create new model
		$model = new CrashReport;
		$model->project_id = 1;
		$model->ignoreFileAttachmentErrors = true; // Ignore errors		
		$model->fileAttachment = new CUploadedFile(
					'test.zip',
					$tmpfile,
					'application/octet-stream',
					5,
					0
				);
		$model->md5 = md5_file($tmpfile);
        $model->emailfrom = 'olegkrivtsov@mail.ru';
		
		$model->crashguid='88fbb0ca-899b-4921-87a5-8e0df65d14cf';
		
		// Apply changes - should fail (guid already exists)
		$saved = $model->save();
		$this->assertFalse($saved);
		
		$model->crashguid='00fbb0ca-899b-4921-87a5-8e0df65d14cf';
		
		// Apply changes
		$saved = $model->save();
		$this->assertTrue($saved);
		
		// Remove temp file
		unlink($tmpfile);
        unlink($tmpfile1);
	}
	
	public function testDelete()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		$deleted = $model->delete();
		$this->assertTrue($deleted);
	}
	
    public function testAttributeLabels()
	{	
		$model = new CrashReport;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['received']=='Date Received');
		$this->assertTrue($attrLabels['geo_location']=='Geographic Location');						
	}	
	
	public function testSimpleSearch()
	{		
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Test simple search
		$crashReport = new CrashReport('search');
		$crashReport->isAdvancedSearch = false;
		$crashReport->filter = '127.0.0.1';		
		$dataProvider = $crashReport->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);	
	}
    
    public function testSimpleSearch_In_Collection()
	{		
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
        // Set current project
        Yii::app()->user->setCurProjectId(1);
        Yii::app()->user->setCurProjectVer(-1); // all versions
        
		// Test simple search in a collection
		$crashReport = new CrashReport('search');
		$crashReport->isAdvancedSearch = false;
		$crashReport->filter = '127.0.0.1';	
        $crashReport->groupid = 1; 
		$dataProvider = $crashReport->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)==1);	
	}
	
	public function testAdvancedSearch()
	{		
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Test simple search
		$crashReport = new CrashReport('search');
		$crashReport->isAdvancedSearch = true;
		$crashReport->ipaddress = '127.0.0.1';		
		$crashReport->emailfrom = 'test@localhost';		
		$crashReport->status = CrashReport::STATUS_PENDING_PROCESSING;
		$crashReport->receivedFrom = date("m/d/Y", mktime(0, 0, 0, 4, 14, 2012));
		$crashReport->receivedTo = date("m/d/Y", mktime(0, 0, 0, 4, 16, 2012));
		
		// Perform search
		$dataProvider = $crashReport->search();		
		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);	
	}
	
	public function testSearchFileItems()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		$dataProvider = $model->searchFileItems();
		$this->assertTrue(count($dataProvider->data)!=0);
	}
    
    public function testSearchScreenshots()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
        // Ensure there are no screenshots in this crash report
		$dataProvider = $model->searchScreenshots();
		$this->assertTrue($dataProvider->totalItemCount==0);
        
        // Find a crash report #3
		$model = CrashReport::model()->findByPk(3);
		$this->assertTrue($model!=null);
		
        // Ensure there are 2 screenshots in this crash report
		$dataProvider = $model->searchScreenshots();
		$this->assertTrue($dataProvider->totalItemCount==2);
        
	}
    
    public function testSearchVideos()
	{
		// Find a video in crash report #1
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);        
		$dataProvider = $model->searchVideos();        
        //Ensure there are no videos
		$this->assertTrue(count($dataProvider->data)==0);
        
        // Find a video in crash report #3
		$model = CrashReport::model()->findByPk(3);
		$this->assertTrue($model!=null);        
		$dataProvider = $model->searchVideos();        
        //Ensure there is one video file
		$this->assertTrue(count($dataProvider->data)==1);        
	}
	    
	public function testSearchModules()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		$dataProvider = $model->searchModules();
		$this->assertTrue(count($dataProvider->data)!=0);
	}
	
	public function testSearchThreads()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		$dataProvider = $model->searchThreads();
		$this->assertTrue(count($dataProvider->data)!=0);
	}
	
	public function testSearchCustomProps()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		$dataProvider = $model->searchCustomProps();
		$this->assertTrue(count($dataProvider->data)!=0);
	}
	
	public function testDumpFileAttachmentContent()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		$model->dumpFileAttachmentContent($outFile);
		
		// Ensure the file exists
		$this->assertTrue(file_exists($outFile));
		// Ensure file size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete file
		unlink($outFile);
	}
	
	public function testDumpFileItemContent()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		$model->dumpFileItemContent('crashrpt.xml', true, $outFile);
		
		// Ensure the file exists
		$this->assertTrue(file_exists($outFile));
		// Ensure file size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete file
		unlink($outFile);
	}
	
	public function testExtractFileItem()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		$outFile = $model->extractFileItem('crashrpt.xml');
		$this->assertTrue(file_exists($outFile));
		$this->assertTrue(filesize($outFile)>0);
		
		unlink($outFile);
	}
	
	
	public function testDumpScreenshotThumbnail()
	{
		// Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		$model->dumpScreenshotThumbnail('screenshot0.jpg', $outFile);
		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
	
	public function testGeneratorVersionToStr()
	{
		$ver = CrashReport::generatorVersionToStr('1300');
		$this->assertTrue($ver=='1.3.0');
	}
	
	public function testGeoIdToCountryName()
	{
		$countryName = CrashReport::geoIdToCountryName('ru-ru');
		$this->assertTrue($countryName=='Russian Federation (ru-ru)');
	}
	
	public function testGenerateUploadStatisticsGraph7()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year debug info statistics
		CrashReport::generateUploadStatisticsGraph(320, 240, 7, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
	
	public function testGenerateVersionDistributionGraph7()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year debug info statistics
		CrashReport::generateVersionDistributionGraph(320, 240, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
    
    public function testGenerateOSVersionDistributionGraph7()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year debug info statistics
		CrashReport::generateOSVersionDistributionGraph(320, 240, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
    
    public function testGenerateGeoLocationDistributionGraph()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year debug info statistics
		CrashReport::generateGeoLocationDistributionGraph(320, 240, $outFile);		
        
		// Ensure the file exists
		$this->assertTrue(file_exists($outFile));
		// Ensure file size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
    
    public function testCanResetStatus()
    {
        // Find a crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
        
        // Can't reset it's status (its Pending)
        $this->assertFalse($model->canResetStatus());
        
        // Find a processed crash report
		$model = CrashReport::model()->findByPk(3);
		$this->assertTrue($model!=null);
        
        // Status can be reseted
        $this->assertTrue($model->canResetStatus());
    }
    
    public function testResetStatus()
    {
        // Find a processed crash report
		$model = CrashReport::model()->findByPk(3);
		$this->assertTrue($model!=null);
        
        // Reset its status - assume success
        $model->resetStatus();
        $this->assertTrue($model->status==CrashReport::STATUS_PENDING_PROCESSING);
    }
    
    /**
     * @expectedException CHttpException
     */
    public function testResetStatus_of_pending_report()
    {
        // Find a pending crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);
        
        // Can't reset it's status (its Pending) - exception expected
        $model->resetStatus();
    }
    
    public function testGetOsBittnessStr()
    {
        // Find a pending crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);        
        $this->assertTrue($model->getOsBittnessStr()==null);
        
        // Find another crash report
		$model = CrashReport::model()->findByPk(3);
		$this->assertTrue($model!=null);        
        $this->assertTrue($model->getOsBittnessStr()=='64-bit');
        
        // Find another crash report
		$model = CrashReport::model()->findByPk(4);
		$this->assertTrue($model!=null);        
        $this->assertTrue($model->getOsBittnessStr()=='32-bit');
    }
    
    public function testCheckFileAttachment()
    {
        // Create temp file 
		$tmpfile1 = tempnam(Yii::app()->getRuntimePath(), "test");
		$tmpfile = $tmpfile1 . '.zip';
		$f = fopen($tmpfile, 'w');
		fwrite($f, 'MZP1234567');
		fclose($f);		
        
        // Create new model
		$model = new CrashReport;
		$model->md5 = '123456789012345678';
        
        // Ensure file attachment check fails (null attachment)
        $this->assertFalse($model->checkFileAttachment());
        
        
        $model->fileAttachment = new CUploadedFile(
					'test.zip',
					$tmpfile,
					'application/octet-stream',
					5,
					0
				);
        
        // Ensure file attachment check fails (md5 is incorrect)
        $this->assertFalse($model->checkFileAttachment());
        
		$model->md5 = md5_file($tmpfile);
        
		// Ensure file attachment check succeeds
        $this->assertTrue($model->checkFileAttachment());		
        
        // Remove temp file
		unlink($tmpfile);
        unlink($tmpfile1);
    }
    
    public function testGetCrashGroupTitle()
    {
        // Create new crash report
		$model = new CrashReport;
		$model->md5 = '123456789012345678';
        $model->project_id = 100; // invalid project id
        $model->status = CrashReport::STATUS_PROCESSED;
                
        $groupMD5 = '';
        $title = $model->getCrashGroupTitle($groupMD5);
        $this->assertTrue($title == 'Unknown Project');
        $this->assertTrue(strlen($groupMD5)!=0);
        
        $model->project_id = 1; // valid project id
        $groupMD5 = '';
        $title = $model->getCrashGroupTitle($groupMD5);
        $this->assertTrue($title == 'Reports without Exception Info');
        $this->assertTrue(strlen($groupMD5)!=0);
        
        // Find a pending crash report
		$model = CrashReport::model()->findByPk(1);
		$this->assertTrue($model!=null);        
        $groupMD5 = '';
        $title = $model->getCrashGroupTitle($groupMD5);
        $this->assertTrue($title == 'Unsorted Reports');
        $this->assertTrue(strlen($groupMD5)!=0);
        
        // Find an invalid crash report
		$model = CrashReport::model()->findByPk(5);
		$this->assertTrue($model!=null);        
        $groupMD5 = '';
        $title = $model->getCrashGroupTitle($groupMD5);
        $this->assertTrue($title == 'Invalid Reports');
        $this->assertTrue(strlen($groupMD5)!=0);
        
        // Find a processed crash report
		$model = CrashReport::model()->findByPk(4);
		$this->assertTrue($model!=null);        
        
        $groupMD5 = '';
        $title = $model->getCrashGroupTitle($groupMD5);
        $this->assertTrue($title == 'CrashRptd.dll! wmain() +0x2bd9 [crtex.c: 120]');
        $this->assertTrue(strlen($groupMD5)!=0);
        
        // Find another processed crash report
		$model = CrashReport::model()->findByPk(6);
		$this->assertTrue($model!=null);        
        
        $groupMD5 = '';
        $title = $model->getCrashGroupTitle($groupMD5);                
        $this->assertTrue($title == 'CrashRpt.dll!+0xf');
        $this->assertTrue(strlen($groupMD5)!=0);
    }
}