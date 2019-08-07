<?php

class UserGroupController extends Controller
{
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';

	public $sidebarActiveItem = 'Administer';
	public $adminMenuItem='Groups';
	
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
			array('allow',  // allow all users to perform 'index' and 'view' actions
				'actions'=>array('index', 'view', 'create','update', 'disable', 'enable', 'delete'),
				'roles'=>array('gperm_access_admin_panel'),
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
		$model = $this->loadModel($id);
				
		$this->render('view',array(
			'model'=>$model,
			'dataProvider'=>$model->searchUsers()
		));
	}

	/**
	 * Creates a new model.
	 * If creation is successful, the browser will be redirected to the 'view' page.
	 */
	public function actionCreate()
	{
		$model=new UserGroup;
		
		if(isset($_POST['UserGroup']))
		{			
			$model->attributes=$_POST['UserGroup'];
			if($model->save())
				$this->redirect(array('view','id'=>$model->id));
		}

		$this->render('create',array(
			'model'=>$model,
		));
	}

	/**
	 * Updates a particular model.
	 * If update is successful, the browser will be redirected to the 'view' page.	 
	 */
	public function actionUpdate($id)
	{			
		$model = $this->loadModel($id);			
			
		if(isset($_POST['UserGroup']))
		{	
			$model->attributes = $_POST['UserGroup'];			
			if($model->save())
				$this->redirect(array('view','id'=>$model->id));			
		}
		
		$this->render('update',array(
				'model'=>$model,
			));
	}

	/**
	 * Disables the group.
	 */
	public function actionDisable()
	{
		if(Yii::app()->request->isPostRequest)
		{
			if(!isset($_POST['id']))
				throw new CHttpException(403, 'Invalid request');
			
			$id = $_POST['id'];
			
			// we only allow this via POST request
			$this->loadModel($id)->enable(false);

			
			$this->redirect(array('view', 'id'=>$id));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}
	
	/**
	 * Enables the group.
	 */
	public function actionEnable()
	{
		// We only allow this via POST request
		if(Yii::app()->request->isPostRequest)
		{
			if(!isset($_POST['id']))
				throw new CHttpException(403, 'Invalid request');
			
			$id = $_POST['id'];
									
			$this->loadModel($id)->enable(true);

			
			$this->redirect(array('view', 'id'=>$id));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}
	
	/**
	 * Deletes a particular model.
	 * If deletion is successful, the browser will be redirected to the 'admin' page.	 
	 */
	public function actionDelete()
	{
		if(Yii::app()->request->isPostRequest)
		{
			if(!isset($_POST['id']))
				throw new CHttpException(403, 'Invalid request');
			
			$id = $_POST['id'];
			
			$model = $this->loadModel($id);
			
			// we only allow this via POST request
			if($model->delete())
			{
				Yii::app()->user->setFlash('success', 
						'The group '.$model->name.' has been deleted.');
			}
			else
			{
				Yii::app()->user->setFlash('error', 
						'The group '.$model->name.' could not be deleted, because some users are still belonging to it.');
			}
			
			$this->redirect(array('index'));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}

	/**
	 * Lists all models.
	 */
	public function actionIndex()
	{
		$dataProvider=new CActiveDataProvider('UserGroup');
		$this->render('index',array(
			'dataProvider'=>$dataProvider,
		));
	}

	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer the ID of the model to be loaded
	 */
	public function loadModel($id)
	{
		$model=UserGroup::model()->findByPk($id);
		if($model===null)
			throw new CHttpException(404,'The requested page does not exist.');
		return $model;
	}	
}
