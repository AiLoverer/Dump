<?php

/**
 *   This test case contains tests for Operation model class.
 */
class OperationTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'ops'=>':test_operation',	
    );
		
    public function testGetOperationDescStr()
	{		
		$op = new Operation;
		$op->status = Operation::STATUS_STARTED;
		$op->timestamp = time();
		$op->srcid = 1;
		$op->cmdid = '1234.1';
		$op->optype = Operation::OPTYPE_IMPORTPDB;
		$op->operand1 = 'C:/Program Files/Apache Software Foundations/htdocs/crashfix/protected/data/debugInfo/CrashRpt.pdb';
		$op->operand2 = 'C:/Program Files/Apache Software Foundations/htdocs/crashfix/protected/runtime/tmp1234.tmp';	
		
		$desc = $op->getOperationDescStr();
		$this->assertTrue($desc == 'Import debug info file CrashRpt.pdb');
		
		$op = new Operation;
		$op->status = Operation::STATUS_STARTED;
		$op->timestamp = time();
		$op->srcid = 1;
		$op->cmdid = '1234.2';
		$op->optype = Operation::OPTYPE_PROCESS_CRASH_REPORT;
		$op->operand3 = 'C:/Program Files/Apache Software Foundations/htdocs/crashfix/protected/data/crashReports/crashrpt.zip';
		$op->operand2 = 'C:/Program Files/Apache Software Foundations/htdocs/crashfix/protected/runtime/tmp1234.tmp';	
		
		$desc = $op->getOperationDescStr();
		$this->assertTrue($desc == 'Process crash report file crashrpt.zip');
	}
	
	public function testDeleteOldOperations()
	{
		// Create 10 operations
		
		$i=0;
		for($i=0; $i<10; $i++)
		{
			$op = new Operation;
			$op->status = Operation::STATUS_SUCCEEDED;
			$op->timestamp = time();
			$op->srcid = $i;
			$op->cmdid = '1234.'.$i;
			$op->optype = Operation::OPTYPE_IMPORTPDB;
			$op->operand1 = "C:\Program Files\Apache Software Foundations\htdocs\crashfix\protected\data\debugInfo\CrashRpt$i.pdb";
			$op->operand2 = "C:\Program Files\Apache Software Foundations\htdocs\crashfix\protected\runtime\tmp1234$i.tmp";	
			
			$this->assertTrue($op->save());
		}
		
		// Delete old operations
		Operation::deleteOldOperations(5);
		
		// Expect there are 5 ops now
		$count = Operation::model()->count();
		$this->assertTrue($count==5);
	}	
}