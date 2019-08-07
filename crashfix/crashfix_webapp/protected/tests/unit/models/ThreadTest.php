<?php

/**
 *   This test case contains tests for Thread model class.
 */
class ThreadTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'threads'=>':test_thread',
		'stackframes'=>':test_stackframe',
    );
		
    public function testAttributeLabels()
	{	
		$model = new Thread;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['thread_id']=='Thread ID');
		$this->assertTrue($attrLabels['crashreport_id']=='Crash Report');						
	}
	
	public function testCreate()
	{
		$thread = new Thread;
		$thread->crashreport_id = 1;
		$thread->thread_id = 4094;
		$thread->stack_trace_md5 = '1234567890123456789012';
		
		$saved = $thread->save();
		$this->assertTrue($saved);
		
		// Find created model
		$model = Thread::model()->findByAttributes(array('thread_id'=>4094));
		$this->assertTrue($model!=null);
	}
	
	public function testDelete()
	{
		// Find an existing thread
		$thread = Thread::model()->findByPk(1);
		
		// Delete - assume success
		$deleted = $thread->delete();
		$this->assertTrue($deleted);
	}
	
	public function testGetExceptionStackFrameTitle()
	{
		// Find an existing thread
		$thread = Thread::model()->findByPk(1);
		
		$title = $thread->getExceptionStackFrameTitle();
                $this->assertTrue($title=='CrashRptd.dll! CMainDlg::OnCrash() +0x1 [CrashRptTest.cpp: 1023]');
	}
	
	public function testGetThreadFuncName()
	{
		// Find an existing thread
		$thread = Thread::model()->findByPk(1);
		
		$funcName = $thread->getThreadFuncName();		
		$this->assertTrue($funcName=='_WinMainCRTStartup()');
	}
}