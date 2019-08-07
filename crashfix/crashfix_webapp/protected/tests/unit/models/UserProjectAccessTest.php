<?php

/**
 *   This test case contains tests for UserProjectAccess model class.
 */
class UserProjectAccessTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'users'=>':test_user',
		'usergroups'=>':test_usergroup',
		'projects'=>':test_project',
		'user_project_access'=>':test_user_project_access',
    );
	
	public function testCreateNew()
	{
		// Create new record
		$model = new UserProjectAccess;
		$model->user_id = 1; // root
		$model->project_id = 2;   // wpack
		$model->usergroup_id = 1; // Admin
		
		// Apply changes
		$this->assertTrue($model->save());
		
		// Find newly created record
		$model = UserProjectAccess::model()->findAll('user_id=1');
		$this->assertTrue($model!=null);
	}
	
    public function testAttributeLabels()
	{	
		$model = new UserProjectAccess;
		
		$attrLabels = $model->attributeLabels();
		
		$this->assertTrue($attrLabels['user_id']=='User');
		$this->assertTrue($attrLabels['usergroup_id']=='Role');						
	}	
	
	public function testSearch()
	{
		// Create model
		$model = new UserProjectAccess;
		// Search - this will create data provider
		$dataProvider = $model->search();
		// Ensure some data returned
		$this->assertTrue(count($dataProvider->data)!=0);
	}	
}