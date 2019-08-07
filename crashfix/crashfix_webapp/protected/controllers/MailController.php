<?php

class MailController extends Controller 
{
    public $layout = 'column2';
    
    public $sidebarActiveItem = 'Administer';
	public $adminMenuItem='Mail';
    
    /**
     * @return array action filters
     */
    public function filters() 
    {
        return array(
            'accessControl', // perform access control              
        );
    }    

    /**
     * Specifies the access control rules.
     * This method is used by the 'accessControl' filter.
     * @return array access control rules
     */
    public function accessRules() {
        return array(
            array('allow', // Allow root
                'actions' => array(
                    'index', 
                    'view',       
                    'delete',
                    'resetStatus'
                ),
                'roles'=>array('gperm_access_admin_panel'),
            ),
            array('deny', // deny all users
                'users' => array('*'),
            ),
        );
    }

    /*
     * Display certain email message page.
     */
    public function actionView($id) 
    {   
        $model = $this->loadModel($id);
        
        $this->render('view', array('model' => $model));
    }

    /**
     * Displays all emails. 
     */
    public function actionIndex() 
    {
        // Create search form
        $model = new MailQueue('search');
        if(isset($_GET['q']) && isset($_GET['status']))
        {
            $model->isSimpleSearch = true;
            $model->searchFilter = $_GET['q'];
            $model->statusFilter = $_GET['status'];
        }
        else if(isset($_POST['MailQueue']))
        {
            $model->isSimpleSearch = false;
            $model->attributes = $_POST['MailQueue'];
        }
        
        $this->render('index', 
                array('dataProvider' => $model->search(), 
                    'model'=>$model));
    }
    
    /**
     * Deletes a particular model.
     * If deletion is successful, the browser will be redirected to the 'index' page.	 
     */
    public function actionDelete() 
    {
        // We only allow deletion via POST request
        if (Yii::app()->request->isPostRequest && isset($_POST['id'])) {
            // Get user ID to delete
            $id = $_POST['id'];

            $model = $this->loadModel($id);
            $model->delete();

            Yii::app()->user->setFlash('success', 'Mail message #'.$model->id.' has been deleted.');
            
            $this->redirect(array('index'));
        }
        else
            throw new CHttpException(400, 'Invalid request.');
    }
    
    /*
     * Reset status of certain E-mail.
     */
    public function actionResetStatus() 
    {   
        if (Yii::app()->request->isPostRequest && isset($_POST['id'])) {            

            $id = $_POST['id'];
            $model = $this->loadModel($id);            

            $model->status = MailQueue::STATUS_PENDING;
            $model->save();

            Yii::app()->user->setFlash('success', 'Status of mail message #'.$model->id.' has been reseted.');
            
            $this->render('view', array('model' => $model));
        }
        else
            throw new CHttpException(400, 'Invalid request.');
    }
    
    /**
     * Returns the data model based on the primary key given in the GET variable.
     * If the data model is not found, an HTTP exception will be raised.
     * @param string the username of the model to be loaded
     */
    public function loadModel($id) 
    {
        $model = MailQueue::model()->findByAttributes(array('id' => $id));
        if ($model === null)
            throw new CHttpException(404, 'The requested page does not exist.');
        return $model;
    }

}