<?php

/**
 *   This test case contains tests for StackFrame model class.
 */
class StackFrameTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'threads'=>':test_thread',
		'stackframes'=>':test_stackframe',
    );
		
    public function testCreate()
	{	
		$model = new StackFrame;
		$model->thread_id=1;
		$model->addr_pc=123456;
		$model->module_id=1;
		$model->offs_in_module=43435;
		$model->symbol_name = '__test@test@@AAZ';
		$model->und_symbol_name='test::test()';
		$model->offs_in_symbol=503;
		$model->src_file_name='CrashRpt.cpp';
		$model->src_line=60;
		$model->offs_in_line=0;
		
		// Create new model
		$saved = $model->save();
		$this->assertTrue($saved);
		
		// Find created model
		$model = StackFrame::model()->find('addr_pc=123456');
		$this->assertTrue($model!=null);
	}
	
	public function testGetTitle()
	{
        // Create a stack frame with zero fields (special case)
        $model = new StackFrame;
		$title = $model->getTitle();
        $this->assertTrue($title=='[WARNING: Stack unwind information not available. Frames below may be wrong.]');
        
        $model->addr_pc=0x123456;
        $title = $model->getTitle();        
        $this->assertTrue($title=='0x123456');
        
        $model->module_id=1;
        $model->offs_in_module=0x43435;
        $title = $model->getTitle();        
        $this->assertTrue($title=='CrashRptd.dll!+0x43435');
        
        // Set some sym info        		
        $model->symbol_name = '__test@test@@AAZ';
        $model->offs_in_symbol=0x123;
        $title = $model->getTitle();        
        $this->assertTrue($title=='CrashRptd.dll! __test@test@@AAZ +0x123 ');
        
        // Set source file info
        $model->src_line=60;		
        $title = $model->getTitle();        
        $this->assertTrue($title=='CrashRptd.dll! __test@test@@AAZ +0x123 [line 60]');
        
        $model->src_file_name='CrashRpt.cpp';
        $title = $model->getTitle();        
        $this->assertTrue($title=='CrashRptd.dll! __test@test@@AAZ +0x123 [CrashRpt.cpp: 60]');
        
        // Find a stack frame with symbol info
		$model = StackFrame::model()->findByPk(1);
		$title = $model->getTitle();
        $this->assertTrue($title=='CrashRptd.dll! CMainDlg::OnCrash() +0x1 [CrashRptTest.cpp: 1023]');
	}
}