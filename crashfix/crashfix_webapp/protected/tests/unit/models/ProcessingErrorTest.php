<?php

/**
 *   This test case contains tests for ProcessingError model class.
 */
class ProcessingErrorTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		'processingerrors'=>':test_processingerror',
		'crashreports'=>':test_crashreport',
		'debuginfo'=>':test_debuginfo',		
    );
		
    public function testCreate()
	{	
		// Create new processing error associated with a crash report
		$model = new ProcessingError;
		$model->type = ProcessingError::TYPE_CRASH_REPORT_ERROR;
		$model->srcid = 100;
		$model->message = 'Some error message';
		// srcid is incorrect
		$this->assertFalse($model->validate());				
		// Set correct srcid
		$model->srcid = 1;
		$saved = $model->save();
		// Ensure created ok
		$this->assertTrue($saved);
		
		// Find created model
		$model = ProcessingError::model()->find('message="Some error message"');
		$this->assertTrue($model!=null);
		
		// Create new processing error associated with a debug info
		$model = new ProcessingError;
		$model->type = ProcessingError::TYPE_DEBUG_INFO_ERROR;
		$model->srcid = 100;
		$model->message = 'Error message';
		// srcid is incorrect
		$this->assertFalse($model->validate());				
		// Set correct srcid
		$model->srcid = 1;
		$saved = $model->save();
		// Ensure created ok
		$this->assertTrue($saved);
	}
}