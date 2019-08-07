<?php

class ProjectController extends Controller
{
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';

	public $sidebarActiveItem = 'Administer';
	public $adminMenuItem='Projects';
	
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
				'actions'=>array('index','view', 'create','update', 'addUser', 'deleteUser', 'disable', 'enable', 'delete'),
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
		$model=new Project;
		
		if(isset($_POST['Project']))
		{
			$model->attributes=$_POST['Project'];
			if($model->save())
            {
                Yii::app()->user->setFlash(
                            'success', 
                            'Project '.$model->name.' has been created successfully.'
                            );     
                
				$this->redirect(array('view','id'=>$model->id));
            }
		}

		$this->render('create',array(
			'model'=>$model,
		));
	}

	/**
	 * Updates a particular model.
	 * If update is successful, the browser will be redirected to the 'view' page.
	 * @param integer $id the ID of the model to be updated
	 */
	public function actionUpdate($id)
	{
		$model=$this->loadModel($id);

		// Uncomment the following line if AJAX validation is needed
		// $this->performAjaxValidation($model);

		if(isset($_POST['Project']))
		{
			$model->attributes=$_POST['Project'];
			if($model->save())
            {
                Yii::app()->user->setFlash(
                            'success', 
                            'Project information has been updated.'
                            );                
                
				$this->redirect(array('view','id'=>$model->id));
            }
		}

		$this->render('update',array(
			'model'=>$model,
		));
	}

	/**
	 * Disables the project.
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
	 * Enables the project.
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
						'The project '.$model->name.' has been deleted.');
			}
			else
			{
				Yii::app()->user->setFlash('error', 
						'The project '.$model->name.' could not be deleted.');
			}
			
			$this->redirect(array('index'));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}

	/**
	 * Adds a user to project.
	 */
	public function actionAddUser()
	{		
		if(!isset($_POST['id']))
            throw new CHttpException(403, 'Invalid request');
		
        $projectId = $_POST['id'];
        
        $project = Project::model()->findByPk($projectId);
        if($project===Null)
            throw new CHttpException(403, 'Invalid request');
        
		if(isset($_POST['User']))
		{   
            // Get items
            $items = $_POST['User'];
            
            // Get values of checkbox column
            $checks = array();
            if(isset($_POST['check']))
                $checks = $_POST['check'];
            
            // Walk through items
            foreach($items as $userId=>$item)
            {
                // Find user project access record 
                $model = UserProjectAccess::model()->findByAttributes(
                        array('user_id'=>$userId,
                            'project_id'=>$projectId));
                
                // Find out if this item is checked or not
                if(in_array($userId, $checks)) // checked
                {
                    if($model===Null) // If this user has no access to this project...
                    {
                        // Give him access
                        $model = new UserProjectAccess();
                        $model->user_id = $userId;
                        $model->project_id = $projectId;
                        $model->usergroup_id = $item['usergroup'];

                        if(!$model->save())
                            throw new CHttpException(403, 'Error saving record to database');				                
                    }
                }
                else // unchecked
                {
                    if($model!=Null) // If user has access to project
                    {
                        // Revoke access
                        if(!$model->delete())
                            throw new CHttpException(403, 'Error deleting record from database');				                
                    }
                }
			}
            
            Yii::app()->user->setFlash(
                            'success', 
                            'User roles have been updated.'
                            );
                    
            $this->redirect(array('project/view', 'id'=>$projectId));					
            return;
		}
        
        $criteria=new CDbCriteria;                

        $dataProvider =  new CActiveDataProvider('User', array(
            'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'username ASC'
            ),
            'pagination'=>false
        ));

        
        $this->render('addUser',array(
            'project'=>$project,
            'dataProvider'=>$dataProvider
        ));        
	}
	
	/**
	 * Removes one or several users from project.
	 */
	public function actionDeleteUser()
	{		
		if(!isset($_POST['DeleteRows']) || !isset($_POST['project_id']))
		{
			throw new CHttpException(404, 'The parameter is invalid');
		}
				
		// Get data from user
		$deleteRows=$_POST['DeleteRows'];
		$projectId = $_POST['project_id'];
		
		foreach($deleteRows as $id)
		{
			$model = UserProjectAccess::model()->findByPk($id);
			if($model===Null)
				throw new CHttpException(404, 'The specified record doesn\'t exist in the database.');
				
			// Delete row from database table 
			if(!$model->delete())
			{
				throw new CHttpException(404, 'The specified record doesn\'t exist in the database or could not be deleted.');
			}
		}
        
        Yii::app()->user->setFlash(
            'success', 
            'User roles have been updated.'
            );
		
		// Redirect to index
		$this->redirect(array('project/view', 'id'=>$projectId));
	}	
	
	/**
	 * Lists all models.
	 */
	public function actionIndex()
	{
		$dataProvider=new CActiveDataProvider('Project');
		$this->render('index',array(
			'dataProvider'=>$dataProvider,
		));
	}

	/**
	 * Manages all models.
	 */
	public function actionAdmin()
	{
		$model=new Project('search');
		$model->unsetAttributes();  // clear any default values
		if(isset($_GET['Project']))
			$model->attributes=$_GET['Project'];

		$this->render('admin',array(
			'model'=>$model,
		));
	}

	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer the ID of the model to be loaded
	 */
	public function loadModel($id)
	{
		$model=Project::model()->findByPk($id);
		if($model===null)
			throw new CHttpException(404,'The requested page does not exist.');
		return $model;
	}

	/**
	 * Performs the AJAX validation.
	 * @param CModel the model to be validated
	 */
	protected function performAjaxValidation($model)
	{
		if(isset($_POST['ajax']) && $_POST['ajax']==='project-form')
		{
			echo CActiveForm::validate($model);
			Yii::app()->end();
		}
	}
}
