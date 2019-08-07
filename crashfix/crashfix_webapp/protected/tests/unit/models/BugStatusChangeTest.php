<?php

/**
 *   This test case contains tests for BugStatusChange model class.
 */
class BugStatusChangeTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
		
    );
		
    public function testAttributeLabels()
	{	
		$model = new BugStatusChange;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['bug_id']=='Bug');
		$this->assertTrue($attrLabels['merged_into']=='Merged Into');						
	}	
}