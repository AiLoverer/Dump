<?php

/**
 *   This test case contains tests for CrashGroup model class.
 */
class BugAttachmentTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		'bugAttachments'=>':test_bug_attachment',	
		'bugChanges'=>':test_bug_change',	
    );
	
	private $_fileName;
	
	public function setUp()
	{
		parent::setUp();
		
		$dirName = Yii::app()->getBasePath()."/data/bugAttachments/123/456";		
		@mkdir($dirName, 0777, true);	
		
		$fileName = $dirName."/12345678901234567890123456789012";
		$this->_fileName = $fileName;
		
		$fd = fopen($fileName, 'w');
		fwrite($fd, '12345');
		fclose($fd);
	}
	
	public function tearDown()
	{
		parent::tearDown();
		
		@unlink($this->_fileName);
		
		// Get parent directory names
		$dirName = dirname($this->_fileName);
		$outerDirName = dirname($dirName);
				
		@rmdir($dirName);
		@rmdir($outerDirName);
	}
	
	public function testCreate()
	{
		// Create temp file 
		$tmpfile1 = tempnam(Yii::app()->getRuntimePath(), "test");
		$tmpfile = $tmpfile1 . '.zip';
        unlink($tmpfile1);
		$f = fopen($tmpfile, 'w');
		fwrite($f, 'MZP1234567');
		fclose($f);		
				
		$model = new BugAttachment;
		$model->ignoreFileAttachmentErrors = true; // Ignore errors				
		$model->bug_change_id = 1;
		$model->fileAttachment = new CUploadedFile(
					'somefile.txt',
					$tmpfile,
					'application/octet-stream',
					10,
					0
				);
		
		$saved = $model->save();
		$this->assertTrue($saved);
	}
	
	public function testDelete()
	{
		// Find 
		$model = BugAttachment::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		// Delete
		$deleted = $model->delete();
		$this->assertTrue($deleted);
	}
	
	public function testDumpFileAttachmentContent()
	{
		// Find 
		$model = BugAttachment::model()->findByPk(1);
		$this->assertTrue($model!=null);
		
		// Create temp file for output
		$outFile = tempnam(Yii::app()->getRuntimePath(), "test");	
		
		$model->dumpFileAttachmentContent($outFile);
		
		// Ensure the file exists
		$this->assertTrue(file_exists($outFile));
		// Ensure file size is not 0
		$this->assertTrue(filesize($outFile)>0);
		
		// Delete file
		@unlink($outFile);
	}
}