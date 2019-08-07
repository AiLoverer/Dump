<?php

// The following is needed to use ezComponents Graph
Yii::import('application.vendors.ezcomponents.*');
require_once "Base/src/base.php"; 
Yii::registerAutoloader(array('ezcBase', 'autoload'), true);

class BugController extends Controller
{
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';
	
	/**
	 * @var string the active item for sidebar menu.
	 */
	public $sidebarActiveItem = 'Bugs';
	
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
			array('allow',  
				'actions'=>array(
					'index', 
					'view', 
					'downloadAttachment', 
					'statusDynamics',
                    'statusDist',
					'create', 
					'delete', 
					'deleteMultiple'),
				'users'=>array('@'),
			),
			array('deny',  // deny all users
				'users'=>array('*'),
			),
		);
	}
	
	/**
	 * Displays a particular model.
	 * @param integer $id the ID of the model to be displayed
	 */
	public function actionView($id)
	{
		// Load bug with such an ID
		$model = $this->loadModel($id);
		
		// Check that user is authorized to access this model
		$this->checkAuthorization($model);
		
		if(isset($_POST['Bug']))
		{		
			// Get file attachment            
			$model->fileAttachment = CUploadedFile::getInstance($model, 'fileAttachment');
			
			// Apply the change to bug
			$model->change($_POST['Bug']);			
		}
				
		// Render view
		$this->render('view',array(
			'model'=>$model,
		));
	}
		
	/**
	 * Creates a new model.
	 * If creation is successful, the browser will be redirected to the 'view' page.
	 */
	public function actionCreate()
	{
		// Check that user is authorized to create new bugs
		$this->checkAuthorization(null);
		
		// Create new model
		$model=new Bug;
		
		if(isset($_GET['crashgroup']))
		{		
			// The bug is being created for a particular crash group.
			$model->crashgroups = $_GET['crashgroup'];
			// Automatically fill the summary and description fields
			$model->autoFillSummary();
		}
		
		if(isset($_GET['crashreport']))
		{
			// The bug is being created for a particular crash report.
			$model->crashreports = $_GET['crashreport'];	
			// Automatically fill the summary and description fields
			$model->autoFillSummary();
		}
		
		// Fill model fields
		if(isset($_POST['Bug']))
		{
			$model->attributes=$_POST['Bug'];
			if($model->open())
			{				
				$this->redirect(array('view','id'=>$model->id));
			}
		}

		// Render view
		$this->render('create',array(
			'model'=>$model,
		));
	}

	/**
	 * Deletes a particular model.	 
	 */
	public function actionDelete()
	{
		if(Yii::app()->request->isPostRequest)
		{
			// we only allow deletion via POST request
			if(!isset($_POST['id']))
				throw new CHttpException(400, 'Invalid request');
			
			$id = $_POST['id'];
			
			$model = $this->loadModel($id);
			
			// Check that user is authorized to access this model
			$this->checkAuthorization($model);
			
			// Delete model
			$model->delete();			
			
			// Redirect to index
			$this->redirect('index');
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}

	/**
	 * Lists all models.
	 */
	public function actionIndex()
	{
		// Check that user is authorized to perform this action
		$this->checkAuthorization(null);
		
		$model = new Bug('search');
		if(isset($_GET['q']))
		{
			$model->filter = $_GET['q'];
			if(isset($_GET['status']))
				$model->bugStatusFilter = $_GET['status'];
		}
		else if(isset($_POST['Bug']))
		{			
			// Advanced search	
			$model->isAdvancedSearch = true;			
			
			// Fill form with data
			$model->attributes = $_POST['Bug'];			
		}
		
		$dataProvider=$model->search();
		
		$this->render('index',array(
			'dataProvider'=>$dataProvider,
			'model'=>$model
		));
	}

	/**
	 * Downloads a bug attachment file.
	 * @param type $id
	 * @throws CHttpException 
	 */
	public function actionDownloadAttachment($id)
	{
		$bugAttachment = BugAttachment::model()->findByPk($id);
		if($bugAttachment===null)
			throw new CHttpException(403, 'Invalid request');
		
		// Get the bug this attachment belongs to
		$bug = $bugAttachment->bugChange->bug;
		
		// Check that user is authorized to access this model
		$this->checkAuthorization($bug);
		
		// Dump contents of the attachment
		$bugAttachment->dumpFileAttachmentContent();		
	}
	
	
	/**
	 * This action generates an image with bug status statistics
	 * for the current project/version. 
	 * @param integer $w Image width.
	 * @param integer $h Image height.
	 * @param integer $period Time period (7, 30 or 365).
	 */
	public function actionStatusDynamics($w, $h, $period)
	{	
		// Check that user is authorized to view statistics of this project
		$this->checkAuthorization(null);
		
		Bug::generateBugStatusDynamicsGraph($w, $h, $period);
	}
    
    /**
	 * This action generates an image with bug status distribution
	 * for the current project/version. 
	 * @param integer $w Image width.
	 * @param integer $h Image height.	 
	 */
	public function actionStatusDist($w, $h)
	{	
		// Check that user is authorized to view statistics of this project
		$this->checkAuthorization(null);
		
		Bug::generateBugStatusDistributionGraph($w, $h);
	}
	
	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer the ID of the model to be loaded
	 */
	public function loadModel($id)
	{
		$model=Bug::model()->findByPk($id);
		if($model===null)
			throw new CHttpException(404,'The requested page does not exist.');
		return $model;
	}	
	
	
	/**
	 * Checks if user is authorized to perform the action.
	 * @param Bug $model Authorization object. Can be null.
	 * @param string $permission Permission name.
	 * @throws CHttpException
	 * @return void
	 */
	protected function checkAuthorization($model, $permission = 'pperm_browse_bugs')
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
