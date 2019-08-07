<?php

/**
 *   This test case contains tests for BatchImporter model class.
 */
class BatchImporterTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		'projects'=>':test_project',
        'appversions'=>':test_appversion',
    );
	    
	public function setUp()
	{
		parent::setUp();	
		
        $dirName = Yii::app()->getBasePath()."/testData/import/crashReports/WTLDemo/1.4.0.0";
        if(!is_dir($dirName))
            mkdir($dirName, 0777, true);
        		
		$fileName = $dirName."/7d3f465c-acee-4df6-a5fb-15a093452127.zip";
		$f = copy(__DIR__.'/../../fixtures/files/7d3f465c-acee-4df6-a5fb-15a093452127.zip', $fileName);		
        
        $dirName = Yii::app()->getBasePath()."/testData/import/debugInfo/WTLDemo/1.4.0.0";
        if(!is_dir($dirName))
            mkdir($dirName, 0777, true);
        		
		$fileName = $dirName."/crashcond.pdb";
		$f = copy(__DIR__.'/../../fixtures/files/crashcond.pdb', $fileName);		
                
	}
	
	public function tearDown()
	{
		parent::tearDown();
		
        
	}
	    
    /**
     * This test imports all crash report files from the given directory
     * and checks how many files have been imported.
     */
	public function testImportFiles()
	{        
        $batchImporter = new BatchImporter;
        
        $dirName = Yii::app()->getBasePath()."/testData/import";
        // Import crash report files found in the directory
        $importedCrashReportCount = 0;
        $importedDebugInfoCount = 0;
        $ret = $batchImporter->importFiles($dirName, $importedCrashReportCount, 
                $importedDebugInfoCount);       
        $this->assertTrue($ret);
        // Ensure 1 crash report imported
        $this->assertTrue($importedCrashReportCount==1);
        // Ensure 1 debug info file imported
        $this->assertTrue($importedDebugInfoCount==1);
	}
		
	
}