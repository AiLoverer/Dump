<?php

class CrashGroupController extends Controller
{
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';
	
	/**
	 * @var string the active item for sidebar menu.
	 */
	public $sidebarActiveItem = 'CrashGroups';
	
	/**
	 * @return array action filters
	 */
	public function filters()
	{
		return array(
			'accessControl', // perform access control for CRUD operations
		);
	}

	/**
	 * Specifies the access control rules.
	 * This method is used by the 'accessControl' filter.
	 * @return array access control rules
	 */
	public function accessRules()
	{
		return array(
			array('allow',  // allow authenticated users 
				'actions'=>array('index', 'view', 'delete', 'deleteMultiple'),
				'users'=>array('@'),
			),
			array('deny',  // deny all users
				'users'=>array('*'),
			),
		);
	}
	
	/**
	 * Lists all models.
	 */
	public function actionIndex()
	{
		// Check that user is authorized to access this action
		$this->checkAuthorization(null);
		
		$model = new CrashGroup();

		if(isset($_GET['q']))			
			$model->filter = $_GET['q'];
		
		if(isset($_GET['status']))			
			$model->bugStatusFilter = $_GET['status'];
		
		$dataProvider=$model->search();		
		
		$this->render('index',array(
			'dataProvider'=>$dataProvider,
			'model'=>$model
		));
	}
	
	/**
	 * Displays a particular model.
	 * @param integer $id the ID of the model to be displayed
	 */
	public function actionView($id)
	{
		// Load crash group
		$model = $this->loadModel($id);
		
		// Check that user is authorized to access this model
		$this->checkAuthorization($model);
		
		// Create new crash report model as search conditions container.
		$crashReport = new CrashReport;		
		$crashReport->groupid = $model->id;
		
		// Fill search conditions
		if(isset($_GET['q']))
			$crashReport->filter = $_GET['q'];
		else if(isset($_POST['CrashReport']))
		{			
			// Advanced search	
			$crashReport->isAdvancedSearch = true;			
			
			// Fill form with data
			$crashReport->attributes = $_POST['CrashReport'];			
		}
		
		// Perform search
		$crashReportDataProvider = $crashReport->search();
						
		// Render view
		$this->render('view',array(
			'model'=>$model,
			'crashReportModel'=>$crashReport,
			'crashReportDataProvider'=>$crashReportDataProvider,
		));
	}

	/**
	 * Deletes a particular model.	 
	 */
	public function actionDelete()
	{
		// We only allow deletion via POST request
		if(Yii::app()->request->isPostRequest && isset($_POST['id']))
		{			
			$id = $_POST['id'];
		
			// Load model
			$model = $this->loadModel($id);
			
			// Check that user is authorized to access this model
			$this->checkAuthorization($model, 'pperm_manage_crash_reports');
			
			// Delete model
			$model->delete();			
			
			// Redirect to index
			$this->redirect(array('crashGroup/index'));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}

	/**
	 * This is the 'deleteMultiple' action that is invoked
	 * when a user wants to delete several crash groups at once.	 
	 */	
	public function actionDeleteMultiple()
	{	
		if(!isset($_POST['DeleteRows']))
		{
			throw new CHttpException(404, 'The parameter is invalid');
		}
				
		// Get data from user
		$deleteRows=$_POST['DeleteRows'];
		
		foreach($deleteRows as $id)
		{
			// Load model
			$model = $this->loadModel($id);
			
			// Check that user is authorized to access this model
			$this->checkAuthorization($model, 'pperm_manage_crash_reports');
				
			// Delete row from database table and remove file attachment
			if(!$model->delete())
			{
				throw new CHttpException(404, 'Requested page does not exist.');
			}
		}
		
		// Redirect to index
		$this->redirect(array('crashGroup/index'));
	}	

	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer the ID of the model to be loaded
	 */
	public function loadModel($id)
	{
		$model=CrashGroup::model()->findByPk($id);
		if($model===null)
			throw new CHttpException(404,'The requested page does not exist.');
		return $model;
	}
	
	/**
	 * Checks if user is authorized to perform the action.
	 * @param CrashGroup $model Authorization object. Can be null.
	 * @param string $permission Permission name.
	 * @throws CHttpException
	 * @return void
	 */
	protected function checkAuthorization($model, $permission = 'pperm_browse_crash_reports')
	{
		
		if($model==null)
		{
			$projectId = Yii::app()->user->getCurProjectId();
			if($projectId==false)
				return;
		}
		else
			$projectId = $model->project_id;	
		
		// Check if user can perform this action
		if(!Yii::app()->user->checkAccess($permission, 
				array('project_id'=>$projectId)) )
		{
			throw new CHttpException(403, 'You are not authorized to perform this action.');
		}
	}
}
