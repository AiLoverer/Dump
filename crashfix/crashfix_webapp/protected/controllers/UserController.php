<?php

class UserController extends Controller
{
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';

	public $sidebarActiveItem = 'Administer';
	
	public $adminMenuItem='Users';
	
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
				'actions'=>array('view'),
				'users'=>array('@'),
			),
			array('allow', 
				'actions'=>array('index', 'create', 'update', 'retire', 'resurrect', 'delete', 'addProjectRole', 'deleteProjectRoles'),
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
		// Allow admin to view all profiles and
		// allow user to view his own profile.
		if( !Yii::app()->user->checkAccess('gperm_access_admin_panel') &&
		    !Yii::app()->user->checkAccess('gperm_view_own_profile', array('user_id'=>$id)) )
		{
			throw new CHttpException(403, 'You are not authorized to perform this action.');
		}
		
		$userProjectAccess = new UserProjectAccess;
		$userProjectAccess->user_id = $id;
		
		if(!Yii::app()->user->checkAccess('gperm_access_admin_panel'))			
			$this->adminMenuItem=null;
		
		$this->render('view',array(
			'model'=>$this->loadModel($id),
			'userProjectAccess'=>$userProjectAccess
		));
	}

	/**
	 * Creates a new model.
	 * If creation is successful, the browser will be redirected to the 'view' page.
	 */
	public function actionCreate()
	{
		$model=new User('create');
		$model->usergroup = 4;

		// Uncomment the following line if AJAX validation is needed
		// $this->performAjaxValidation($model);		
		if(isset($_POST['User']))
		{	
			$model->attributes=$_POST['User'];			
			if($model->save())
			{
				if($model->sendEmailWithAccountActivationLink())
				{
					Yii::app()->user->setFlash('success', 
						'The new user has been added successfully. An E-mail message with account activation link has been sent to new user\'s mailbox.');
				}
				else
				{
					Yii::app()->user->setFlash('error', 'The new user has been added. However an E-mail message with account activation link couldn\'t be sent to new user\'s mailbox. Please check your SMTP server.');
				}
				
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
	 */
	public function actionUpdate()
	{
		if(!isset($_POST['id']))
			throw new CHttpException(400, 'Invalid request');
		
		$id = $_POST['id'];
		
		$model=$this->loadModel($id);

		// Uncomment the following line if AJAX validation is needed
		// $this->performAjaxValidation($model);

		if(isset($_POST['User']))
		{
			$model->attributes=$_POST['User'];
			if($model->save())
				$this->redirect(array('view','id'=>$model->id));
		}

		$this->render('update',array(
			'model'=>$model,
		));
	}

	/**
	 * Marks a particular model as retired (inactive).
	 */
	public function actionRetire()
	{
		// We only allow this via POST request		
		if(Yii::app()->request->isPostRequest && isset($_POST['id']))
		{			
			// Get user ID 
			$id = $_POST['id'];			
			
			// Make changes in AR
			$this->loadModel($id)->retire();

			// Redirect to index action
			$this->redirect(array('index'));
		}
		else
			throw new CHttpException(400, 'Invalid request.');
	}
	
	/**
	 * Marks a particular retired model as active.
	 */
	public function actionResurrect()
	{
		// We only allow this via POST request		
		if(Yii::app()->request->isPostRequest && isset($_POST['id']))
		{			
			// Get user ID 
			$id = $_POST['id'];			
			
			// Make changes in AR
			$user = $this->loadModel($id);
			
			// Check if user is a real retired.
			if($user->status!=User::STATUS_DISABLED)
				throw new CHttpException(400, 'Invalid request.');
			
			$user->resurrect();

			// Redirect to index action
			$this->redirect(array('index'));
		}
		else
			throw new CHttpException(400, 'Invalid request.');
	}
	
	/**
	 * Deletes a particular model.
	 * If deletion is successful, the browser will be redirected to the 'index' page.	 
	 */
	public function actionDelete()
	{
		// We only allow deletion via POST request
		if(Yii::app()->request->isPostRequest && isset($_POST['id']))
		{			
			// Get user ID to delete
			$id = $_POST['id'];			
			
			$this->loadModel($id)->delete();

			$this->redirect(array('index'));
		}
		else
			throw new CHttpException(400, 'Invalid request.');
	}

	/**
	 * Lists all models.
	 */
	public function actionIndex()
	{
		// Create search form
		$model = new User('search');
		$model->unsetAttributes();  // clear any default values
		if(isset($_POST['User']))
			$model->attributes = $_POST['User'];
		
		$this->render('index',array(
			'model'=>$model,
		));
	}

	/**
	 * Adds a project role for user.
	 */
	public function actionAddProjectRole()
	{
		if(!isset($_POST['id']))
            throw new CHttpException(403, 'Invalid request');
		
        $userId = $_POST['id'];
        
        $user = User::model()->findByPk($userId);
        if($user===Null)
            throw new CHttpException(403, 'Invalid request');
        
		if(isset($_POST['Project']))
		{   
            // Get items
            $items = $_POST['Project'];
            
            // Get values of checkbox column
            $checks = array();
            if(isset($_POST['check']))
                $checks = $_POST['check'];
            
            // Walk through items
            foreach($items as $projectId=>$item)
            {
                // Find user project access record 
                $model = UserProjectAccess::model()->findByAttributes(
                        array('user_id'=>$userId,
                            'project_id'=>$projectId));
                
                // Find out if this item is checked or not
                if(in_array($projectId, $checks)) // checked
                {
                    if($model===Null) // If this user has no access to this project...
                    {
                        // Give him access
                        $model = new UserProjectAccess();
                        $model->user_id = $userId;
                        $model->project_id = $projectId;
                        $model->usergroup_id = $item['role'];

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
                    
            $this->redirect(array('user/view', 'id'=>$userId));					
            return;
		}
        
        $criteria=new CDbCriteria;                

        $dataProvider =  new CActiveDataProvider('Project', array(
            'criteria'=>$criteria,
            'sort'=>array(
                'defaultOrder'=>'name ASC'
            ),
            'pagination'=>false
        ));
        
        $this->render('addProjectRole',array(
            'user'=>$user,
            'dataProvider'=>$dataProvider
        ));      
	}
	
	/**
	 * Deletes one or several project roles from user.	 
	 */	
	public function actionDeleteProjectRoles()
	{		
		if(!isset($_POST['DeleteRows']) || !isset($_POST['user_id']))
		{
			throw new CHttpException(404, 'The parameter is invalid');
		}
				
		// Get data from user
		$deleteRows=$_POST['DeleteRows'];
		$userId = $_POST['user_id'];
		
		foreach($deleteRows as $id)
		{
			$model = UserProjectAccess::model()->findByPk($id);
			if($model===Null)
				throw new CHttpException(404, 'The specified record doesn\'t exist in the database.');
				
			// Delete row from database table and remove file attachment
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
		$this->redirect(array('user/view', 'id'=>$userId));
	}
	
	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer the ID of the model to be loaded
	 */
	public function loadModel($id)
	{
		$model=User::model()->findByPk($id);
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
		if(isset($_POST['ajax']) && $_POST['ajax']==='user-form')
		{
			echo CActiveForm::validate($model);
			Yii::app()->end();
		}
	}	
}
