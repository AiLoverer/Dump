<?php

Yii::import('application.commands.*');

/**
 *   This test case contains tests for PollCommand class.
 */
class PollCommandTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'crashreports'=>':test_crashreport',
        'projects'=>':test_project',	
		'appversions'=>':test_appversion',	
		'crashgroups'=>':test_crashgroup',	
		'threads'=>':test_thread',	
		'modules'=>':test_module',	
		'fileItems'=>':test_fileitem',	
        'stackFrames'=>':test_stackframe',	
		'customProps'=>':test_customprop',	
		'processingErrors'=>':test_processingerror',
        'debugInfo'=>':test_debuginfo',
    );
	
    public function setUp()
	{
		parent::setUp();		
	}
	
	public function tearDown()
	{
		parent::tearDown();
		
        // Remove created temp files.
        $filePattern = Yii::app()->getRuntimePath()."/aop*.tmp";        
        foreach(glob($filePattern) as $filename) 
        {
            unlink($filename);
        }
	}
    
    public function testRun()
	{	
		$runner=new CConsoleCommandRunner();
		$command = new PollCommand('poll', $runner);
		$args = array();
		
		$retCode = $command->run($args);
				
		$this->assertTrue($retCode==0);
	}	
	
	
	
}
