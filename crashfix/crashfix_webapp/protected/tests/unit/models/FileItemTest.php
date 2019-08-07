<?php

/**
 *   This test case contains tests for FileItem model class.
 */
class FileItemTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		'crashreports'=>':test_crashreport',
        'fileitems'=>':test_fileitem',
    );
		
    public function testAttributeLabels()
	{	
		$model = new FileItem;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['filename']=='File Name');
		$this->assertTrue($attrLabels['crashreport_id']=='Crash Report');						
	}
	
	public function testCreate()
	{
		// Create new model
		$model = new FileItem;
		$model->crashreport_id = 1;
		$model->filename = 'crashdump.dmp';
		$model->description = 'Crash Minidump';
		
		// Save - assume success
		$saved = $model->save();
		$this->assertTrue($saved);
		
		// Find created model
		$model = FileItem::model()->find('filename="crashdump.dmp"');
		$this->assertTrue($model!=null);
	}
	
}

