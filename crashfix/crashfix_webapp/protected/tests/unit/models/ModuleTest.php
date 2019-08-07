<?php

/**
 *   This test case contains tests for Module model class.
 */
class ModuleTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'modules'=>':test_module',		
    );

	public function testAttributeLabels()
	{	
		$model = new Module;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['name']=='Module Name');
		$this->assertTrue($attrLabels['loaded_image_name']=='Loaded Image Name');						
	}
	
	public function testCreate()
	{
		$model = new Module;
		$model->crashreport_id=1;
		$model->name='user32.dll';
                $model->file_version = '6.1.0.7600';
		$model->sym_load_status=0;
		$model->loaded_debug_info_id=null;
		
		$saved = $model->save();
		$this->assertTrue($saved);
		
		$model = Module::model()->find('name="user32.dll"');
		$this->assertTrue($model!=null);
	}
}