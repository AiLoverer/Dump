<?php

/**
 *   This test case contains tests for CustomProp model class.
 */
class CustomPropTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		
    );
		
    public function testCreate()
	{	
		$model = new CustomProp;
		$model->crashreport_id=1;
		$model->name = 'VideoCard';
		$model->value = 'nVidia GeForce';
		
		$this->assertTrue($model->save());
		
		// Find created model
		$model = CustomProp::model()->find("name='VideoCard'");
		$this->assertTrue($model!=Null);		
	}	
}