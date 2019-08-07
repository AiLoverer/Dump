<?php

/**
 *   This test case contains tests for DebugInfo model class.
 */
class DebugInfoTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'debuginfo'=>':test_debuginfo',
    );
	
	private $_fileName;
	
	public function setUp()
	{
		parent::setUp();
		
		$dirName = Yii::app()->getBasePath()."/data/debugInfo/CrashRptd.pdb/6dbf5b63-6c53-4cc1-a9c4-5a97a7c66cad";		
		@mkdir($dirName, 0777, true);	
		
		$fileName = $dirName."/CrashRptd.pdb";
		$this->_fileName = $fileName;
		$f = @fopen($fileName, 'w');
		fwrite($f, 'MZP1234567');
		fclose($f);		
	}
	
	public function tearDown()
	{
		parent::tearDown();
		
		@unlink($this->_fileName);
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
		$tmpfile = $tmpfile1 . '.pdb';
		$f = fopen($tmpfile, 'w');
		fwrite($f, 'MZP1234567');
		fclose($f);		
		
		// Create new model
		$model = new DebugInfo('create');
		$model->ignoreFileAttachmentErrors = true; // This is to ignore errors
		$model->project_id = 1;
		$model->guid = 'tmp_12345678901234456745';
		$model->fileAttachment = new CUploadedFile(
					'CrashRptd.pdb',
					$tmpfile,
					'application/octet-stream',
					5,
					0
				);
				
		// Apply changes
		$saved = $model->save();
        $this->assertTrue($saved);
		
		// Try to save again - should fail (GUID already exists)
		// Create new model
		$model2 = new DebugInfo('create');
		$model2->ignoreFileAttachmentErrors = true; // This is to ignore errors
		$model2->project_id = 1;
		$model2->guid = 'tmp_12345678901234456745';
		$model2->fileAttachment = new CUploadedFile(
					'CrashRptd.pdb',
					$tmpfile,
					'application/octet-stream',
					5,
					0
				);
		
		// Apply changes
		$saved2 = $model2->save();
		$this->assertFalse($saved2);
		
		// Remove temp file
		unlink($tmpfile);
        unlink($tmpfile1);
	}
	
	public function testDelete()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Find
		$debugInfo = DebugInfo::model()->findByPk(1);
		// Delete
		$deleted = $debugInfo->delete();
		// Ensure deleted
		$this->assertTrue($deleted);
	}
	
	public function testDumpFileAttachmentContent()
	{
		// Login as root
		$model = new LoginForm('RegularLogin');	
		$model->username = "root";						
		$model->password = "rootpwd";
		$this->assertTrue($model->login());
		
		// Create temp file 
		$tmpfile = tempnam(Yii::app()->getRuntimePath(), "test");
		
		// Find a debug info file
		$debugInfo = DebugInfo::model()->findByPk(1);
		$this->assertTrue($debugInfo!=null);
		
		// Dump file content
		$debugInfo->dumpFileAttachmentContent($tmpfile);
		// Ensure the file exists
		$this->assertTrue(file_exists($tmpfile));
		// Ensure file size is not 0
		$this->assertTrue(filesize($tmpfile)>0);
		
        // Remove temp file
		unlink($tmpfile);
	}
	
    public function testAttributeLabels()
	{	
		$model = new DebugInfo();
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['dateuploaded']=='Date Uploaded');
		$this->assertTrue($attrLabels['filesize']=='File Size');						
	}	
	
    public function testSimpleSearchWithoutLogin()
	{
		// Test simple search without logging in
		$model = new DebugInfo('search');
		$model->isAdvancedSearch = false;
		$model->filter = 'Cr';		
		$dataProvider = $model->search();		
		$this->assertTrue($dataProvider==Null);
	}
	
	public function testSimpleSearch()
	{		
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Test simple search
		$model = new DebugInfo('search');
		$model->isAdvancedSearch = false;
		$model->filter = 'Cr';		
		$dataProvider = $model->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);		
	}
	
	public function testAdvancedSearch()
	{
		// Login as root
		$loginForm = new LoginForm('RegularLogin');	
		$loginForm->username = "root";						
		$loginForm->password = "rootpwd";
		$this->assertTrue($loginForm->login());
		
		// Test advanced search
		$model = new DebugInfo('search');
		$model->isAdvancedSearch = true;
		$model->status = DebugInfo::STATUS_PROCESSED;
		$model->guid = '6dbf5b63-6c53-4cc1-a9c4-5a97a7c66cad';
		$model->dateFrom = date("m/d/Y", mktime(0, 0, 0, 4, 14, 2012));
		$model->dateTo = date("m/d/Y", mktime(0, 0, 0, 4, 16, 2012));
		
		// Perform search
		$dataProvider = $model->search();		
		// Ensure something found
		$this->assertTrue(count($dataProvider->data)!=0);		
	}
	
	public function testGenerateDebugInfoUploadStat7()
	{
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		// Generate an image for one year debug info statistics
		DebugInfo::generateDebugInfoUploadStat(320, 240, 7, $outFile);		
		// Ensure the image exists
		$this->assertTrue(file_exists($outFile));
		// Ensure image size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete image
		unlink($outFile);
	}
}