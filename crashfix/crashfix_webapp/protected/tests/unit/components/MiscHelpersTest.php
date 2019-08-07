<?php

/**
 *   This test case contains tests for MiscHelpers helper class.
 */
class MiscHelpersTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        
    );
		
    public function testFileSizeToStr()
	{		
		$this->assertTrue(MiscHelpers::fileSizeToStr(512)=='512 byte(s)');				
		$this->assertTrue(MiscHelpers::fileSizeToStr(1024)=='1 KB');				
		$this->assertTrue(MiscHelpers::fileSizeToStr(2*1024*1024)=='2 MB');
		$this->assertTrue(MiscHelpers::fileSizeToStr(5*1024*1024*1024)=='5 GB');		
	}	
	
	public function testAddEllipsis()
	{
		// Define test string
		$str = '1234567890';
		
		// Ensure it does not add '...' when string is short
		$this->assertTrue(MiscHelpers::addEllipsis($str, 10)==$str);
		
		// Ensure it does add '...' when string is long
		$this->assertTrue(MiscHelpers::addEllipsis($str, 5)=='12...');
	}
	
    public function testGUID()
    {
        // Generate GUID
        $guid = MiscHelpers::GUID();
        
        // Ensure its length is 32 characters
        $this->assertTrue(strlen($guid)==36);
    }
    
	public function testCountryCodeToCountryName()
	{
		// In lower case
		$this->assertTrue(MiscHelpers::countryCodeToCountryName('ru')=='Russian Federation');
		
		// In upper case
		$this->assertTrue(MiscHelpers::countryCodeToCountryName('US')=='United States');
		
		// Incorrect code
		$this->assertTrue(MiscHelpers::countryCodeToCountryName('SU')=='Unknown');
	}
    
    public function testSplitNumberList()
    {
        $list = MiscHelpers::splitNumberList('');
        //var_dump($list);
        $this->assertTrue(is_array($list));
        $this->assertTrue(count($list)==0);
        
        $list = MiscHelpers::splitNumberList(' 123  ');
        //var_dump($list);
        $this->assertTrue(is_array($list));
        $this->assertTrue(count($list)==1);        
        //$a = array_search(123, $list);
        //var_dump($a);
        $this->assertTrue(0==array_search(123, $list));
        
        $list = MiscHelpers::splitNumberList('10 11, 12');
        //var_dump($list);
        $this->assertTrue(is_array($list));
        $this->assertTrue(count($list)==3);
        $this->assertTrue(2==array_search(12, $list));
        
        $list = MiscHelpers::splitNumberList('10 11 12 13');
        //var_dump($list);
        $this->assertTrue(is_array($list));
        $this->assertTrue(count($list)==4);
        $this->assertTrue(2==array_search(12, $list));
        
        $list = MiscHelpers::splitNumberList('10 f 12');
        //var_dump($list);
        $this->assertTrue($list==false);        
    }
}