<?php

/**
 *   This test case contains tests for BugComment model class.
 */
class BugCommentTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'bugs'=>':test_bug',	
        'bugChanges'=>':test_bug_change',	
    );
	
	public function testCreate()
	{
        // Create new bug comment
		$model = new BugComment();
        $model->text = 'Some comment';
        
        // Save model to database - should succeed
		$saved = $model->save();
		$this->assertTrue($saved);
        
        // Find the model we have just created
        $model = BugComment::model()->findByAttributes(array('text'=>'Some comment'));
        $this->assertTrue($model!=null);
	}
	
	
}