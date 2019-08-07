<?php

/**
 *   This test case contains tests for Lookup helper class.
 */
class LookupTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        
    );
		
    public function testItem()
	{		
		$this->assertTrue(Lookup::item('CrashReportStatus', 1)=='Waiting');				
		
		$this->assertTrue(Lookup::item('UserStatus', 1)=='Active');						
	}	
	
	public function testItems()
	{		
		$items = Lookup::items('CrashReportStatus');
		$this->assertTrue($items[1]=='Waiting');				
		
		Lookup::reset();
		$items = Lookup::items('BugStatus', Bug::STATUS_OPEN_MAX);
				
		$this->assertTrue($items[1]=='New');						
		$this->assertTrue(count($items)==4);						
	}
}