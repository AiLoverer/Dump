<?php

Yii::import('application.vendors.ezcomponents.*');
require_once "Base/src/base.php"; 
Yii::registerAutoloader(array('ezcBase', 'autoload'), true);

class CrashReportController extends Controller
{	
	/**
	 * @var string the default layout for the views. Defaults to '//layouts/column2', meaning
	 * using two-column layout. See 'protected/views/layouts/column2.php'.
	 */
	public $layout='//layouts/column2';
	
	/**
	 * @var string the active item for sidebar menu.
	 */
	public $sidebarActiveItem = 'CrashReports';
	
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
			array('allow',  // allow not authenticated users to upload externally
				'actions'=>array(
					'uploadExternal'
				),
				'users'=>array('?'),
			),
			array('allow', // allow athenticated users 
				'actions'=>array(
					'index', 
					'view', 
					'extractFile', 
					'download', 
					'viewScreenshot', 
					'viewScreenshotThumbnail', 
                    'viewVideo',
					'uploadStat', 
					'versionDist',
                    'osVersionDist',
                    'geoLocationDist',
					'processAgain', 
					'reprocessMultiple', 
                    'reprocessAll', 					
                    'delete', 
					'deleteMultiple', 
                    'uploadFile'
				),
				'users'=>array('@'),
			),						
			array('deny',  // deny all users
				'users'=>array('*'),
			),
		);
	}
	
	/**
	 * This action displays a particular model.
	 * @param integer $id the ID of the model to be displayed
	 */
	public function actionView($id, $tab='Summary', $thread=null)
	{
		// Load crash report
		$model = $this->loadModel($id);
		
		// Check if user is authorized to perform the action
		$this->checkAuthorization($model);
				
		// Determine what tab to display
		$activeItem = 'Summary';
		if($tab=='Summary' ||
			$tab=='CustomProps' ||
			$tab=='Screenshots' ||
            $tab=='Videos' ||
			$tab=='Threads' ||
			$tab=='Modules' ||
			$tab=='Files')
			$activeItem = $tab;
		
		$threadModel = null;
		$stackTraceProvider = null;
		if($thread!=null)
		{
			$threadModel = Thread::model()->findByPk((int)$thread);
			if($threadModel==null)
				throw new CHttpException(403, 'Unexpected parameter.');
			
			$criteria = new CDbCriteria;
			$criteria->compare('thread_id', (int)$thread);
            $criteria->order = 'id ASC';
			$stackTraceProvider = new CActiveDataProvider('StackFrame',  array(
							'criteria'=>$criteria,
                            'pagination'=>false
						));            			
		}
        
        $customProps = $model->searchCustomProps();
        $screenshots = $model->searchScreenshots();
        $videos = $model->searchVideos();
        $modules = $model->searchModules();
        $threads = $model->searchThreads();
		
		$this->render('view',array(
			'model'=>$model,
			'activeItem'=>$activeItem,
			'thread'=>(int)$thread,
			'threadModel'=>$threadModel,
			'stackTrace'=>$stackTraceProvider,
            'customProps'=>$customProps,
            'screenshots'=>$screenshots,
            'videos'=>$videos,
            'modules'=>$modules,
            'threads'=>$threads
		));
	}
	
	/**
	 * This action is used to download crash report ZIP archive. 	 
	 */
	public function actionDownload($id)
	{
		// Load requested crash report
		$model = $this->loadModel($id);
		
		// Check if user is authorized to perform the action
		$this->checkAuthorization($model);
		
		// Dump crash report file to stdout
		$model->dumpFileAttachmentContent();		
	}
		
	/**
	 * This action extracts a file item from crash report. 	 
	 */
	public function actionExtractFile($name, $rpt)
	{
		// Load requested crash report
		$model = $this->loadModel($rpt);
		
		// Check if user is authorized to perform the action
		$this->checkAuthorization($model);
		
		// Dump requested file item to stdout
		$model->dumpFileItemContent($name);		
	}
	
	/**
	 * This action opens a screenshot in a new browser window. 	 
	 */
	public function actionViewScreenshot($name, $rpt)
	{
		// Load requested crash report
		$model = $this->loadModel($rpt);
		
		// Check if user is authorized to perform the action
		$this->checkAuthorization($model);
				
		// Dump requested file item to stdout
		$model->dumpFileItemContent($name, false);			
	}
	
	/**
	 * This action downloads a small copy of the screenshot.
	 * @param type $name
	 * @param type $rpt
	 * @throws CHttpException 
	 */
	public function actionViewScreenshotThumbnail($name, $rpt)
	{
		// Load requested crash report
		$model = $this->loadModel($rpt);
		// Check if user is authorized to perform the action
		$this->checkAuthorization($model);
		
		// Dump requested file item to stdout
		$model->dumpScreenshotThumbnail($name);			
	}
	
    /**
	 * This action opens a video in a new browser window. 	 
	 */
	public function actionViewVideo($name, $rpt)
	{
		// Load requested crash report
		$model = $this->loadModel($rpt);
		
		// Check if user is authorized to perform the action
		$this->checkAuthorization($model);
				
		// Dump requested file item to stdout
		$model->dumpFileItemContent($name, false);			
	}
    
	/**
	 * This action resets crash report status to Waiting causing its further
	 * processing. 
	 */
	public function actionProcessAgain()
	{
		if(Yii::app()->request->isPostRequest || isset($_POST['id']))
		{
			$id = $_POST['id'];
						
			$model = $this->loadModel($id);
			
			// Check if user is authorized to perform the action
			$this->checkAuthorization($model, 'pperm_manage_crash_reports');
						
			$model->resetStatus();			
			
			// Refresh view
			$this->redirect(array('crashReport/view', 'id'=>$model->id));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}
	
	/**
	 * This action deletes a particular model.	 	 
	 */
	public function actionDelete()
	{
		if(Yii::app()->request->isPostRequest || isset($_POST['id']))
		{
			$id = $_POST['id'];
			
			// we only allow deletion via POST request
			$model = $this->loadModel($id);
			
			// Check if user is authorized to perform the action
			$this->checkAuthorization($model, 'pperm_manage_crash_reports');
			
			$model->delete();			
			
			$this->redirect(array('crashReport/index'));
		}
		else
			throw new CHttpException(400,'Invalid request. Please do not repeat this request again.');
	}
	
	/**
	 * This is the 'deleteMultiple' action that is invoked
	 * when a user wants to delete several crash reports at once.	 
	 */	
	public function actionDeleteMultiple()
	{	
		if(isset($_POST['DeleteRows']))
		{
			
            // Get data from user
            $deleteRows=$_POST['DeleteRows'];

            foreach($deleteRows as $id)
            {
                $model = $this->loadModel($id);

                // Check if user is authorized to perform the action
                $this->checkAuthorization($model, 'pperm_manage_crash_reports');

                // Delete row from database table and remove file attachment
                if(!$model->delete())
                {
                    throw new CHttpException(404, 'The specified record doesn\'t exist in the database or could not be deleted.');
                }
            }
        }
        
        if(isset($_GET['groupid']))
        {
            // Redirect to collection view
            $groupid = (int)$_GET['groupid'];
            $this->redirect(array('crashGroup/view', 'id'=>$groupid));
            return;
        }
		
		// Redirect to index
		$this->redirect(array('crashReport/index'));
	}
    
    /**
	 * This is the 'reprocessMultiple' action that is invoked
	 * when a user wants to reprocess several crash reports at once.	 
	 */	
	public function actionReprocessMultiple()
	{	
		if(isset($_POST['DeleteRows']))
		{			
            // Get data from user
            $deleteRows=$_POST['DeleteRows'];

            foreach($deleteRows as $id)
            {
                $model = $this->loadModel($id);

                // Check if user is authorized to perform the action
                $this->checkAuthorization($model, 'pperm_manage_crash_reports');

                // Reset model's status
                if($model->canResetStatus())
                {
                    $model->resetStatus();
                }
            }
        }
		
        if(isset($_GET['groupid']))
        {
            // Redirect to collection view
            $groupid = (int)$_GET['groupid'];
            $this->redirect(array('crashGroup/view', 'id'=>$groupid));
            return;
        }
        
		// Redirect to index
		$this->redirect(array('crashReport/index'));
	}
    
    /**
	 * This action resets status of all crash reports in the current project 
     * to Waiting causing their further processing. 
	 */
	public function actionReprocessAll()
	{
        // Check if user is authorized to perform the action
		$this->checkAuthorization(null);
        
        // Get current project ID
        $projectId = Yii::app()->user->getCurProjectId();
        if($projectId==false)
            throw new CHttpException(400,'Invalid request.');
        
        $curProjVer = Yii::app()->user->getCurProjectVer();
               
        // Find all crash reports belonging to project.
        $criteria = new CDbCriteria();
        $criteria->compare('project_id', $projectId);   
        if($curProjVer!=Project::PROJ_VER_ALL)
            $criteria->compare('appversion_id', $curProjVer);
        
        if(isset($_GET['groupid']))
        {
            $groupid = (int)$_GET['groupid'];
            $criteria->compare('groupid', $groupid);   
        }
        
        $models = CrashReport::model()->findAll($criteria);
        
        foreach($models as $model)
        {
			// Check if user is authorized to perform the action
			$this->checkAuthorization($model, 'pperm_manage_crash_reports');
						
            if($model->canResetStatus())
                $model->resetStatus();			
		}
		
        if(isset($_GET['groupid']))
        {
            // Redirect to collection view
            $groupid = (int)$_GET['groupid'];
            $this->redirect(array('crashGroup/view', 'id'=>$groupid));
            return;
        }
                
        // Redirect to index
        $this->redirect(array('crashReport/index'));
	}

	/**
	 * This action lists all models.
	 */
	public function actionIndex()
	{	
		// Check if user is authorized to perform the action
		$this->checkAuthorization(null);
		
		$model = new CrashReport('search');
				
		if(isset($_GET['q']))
			$model->filter = $_GET['q'];
		else if(isset($_POST['CrashReport']))
		{			
			// Advanced search	
			$model->isAdvancedSearch = true;			
			
			// Fill form with data
			$model->attributes = $_POST['CrashReport'];			
		}
		
		$dataProvider=$model->search();
				
		$this->render('index',array(
			'dataProvider'=>$dataProvider,
			'model'=>$model
		));
	}

	/**
	 * This action displays a form for crash report upload. 
	 */
	public function actionUploadFile()
	{
		// Check if user is authorized to perform the action
		$this->checkAuthorization(null, 'pperm_manage_crash_reports');
		
		$model = new CrashReport;
		$form = new CForm('application.views.crashReport.uploadForm', $model);		
				
		if($form->submitted('upload') && $form->validate())
		{				
			$model->fileAttachment = CUploadedFile::getInstance($model, 'fileAttachment');			
		
			// This will create a new record in the {{crashreport}} db table
			// and move the uploaded file to its persistent location.
			$form->model->save();
		}
		
		// Display the result
		$this->render('uploadFile', array('form'=>$form));			
	}
	
	/**
	 * This action allows user to upload a crash report ZIP file using
	 * an external uploader tool.
	 * @throws Exception 
	 */
	public function actionUploadExternal()
	{
		// Create new AR model 
		$report = new CrashReport();		
		
		// Start db transaction
		$transaction = $report->dbConnection->beginTransaction();
		
		// Fill model attributes
		
		try
		{	
			if(isset($_POST['crashrptver']))
				$report->crashrptver = $_POST['crashrptver'];
			
			if(isset($_POST['crashguid']))
				$report->crashguid = $_POST['crashguid'];
							
			if(isset($_POST['appname']))
			{
				$projectName = $_POST['appname'];
				$project = Project::model()->find('name=:name', 
						array(':name'=>$projectName));
				if($project===null)
					throw new Exception('Such a project name not found');
				$report->project_id = $project->id;
			}
			
			// Get file attachment
			if(array_key_exists("crashrpt", $_FILES))
			{				
				$report->fileAttachment = new CUploadedFile(
												$_FILES['crashrpt']['name'], 
												$_FILES['crashrpt']['tmp_name'],
												$_FILES['crashrpt']['type'],
												$_FILES['crashrpt']['size'],
												$_FILES['crashrpt']['error']
												);				
			}
							
			if(isset($_POST['appversion']))
				$report->appversion = $_POST['appversion'];
							
			if(isset($_POST['md5']))
				$report->md5 = $_POST['md5'];
									
			if(isset($_POST['emailfrom']))
				$report->emailfrom = $_POST['emailfrom'];
				
			if(isset($_POST['description']))
				$report->description = $_POST['description'];
				
			if(isset($_POST['exceptionmodule']))
				$report->exceptionmodule = $_POST['exceptionmodule'];
				
			if(isset($_POST['exceptionmodulebase']))
				$report->exceptionmodulebase = $_POST['exceptionmodulebase'];
				
			if(isset($_POST['exceptionaddress']))
				$report->exceptionaddress = $_POST['exceptionaddress'];
						
			// This will create new record in the db table			
			if($report->save())
			{
				// Commit db transaction
				$transaction->commit();
			}
		}
		catch(Exception $e)
		{
			$transaction->rollBack();			
			$report->addError('crashrpt', 'Exception caught: '.$e->getMessage());
		}
				
		$this->renderPartial('_upload', array('model'=>$report));			
	}
	
	/**
	 *  This action generates an image with crash report upload statistics
	 *  for the current project/version. 
	 */
	public function actionUploadStat($w, $h, $period)
	{	
		// Check if user is authorized to perform the action
		$this->checkAuthorization(null);
		
		// Generate an upload statistics graph for current project
		CrashReport::generateUploadStatisticsGraph($w, $h, $period);
	}
	
	public function actionVersionDist($w, $h)
	{	
		// Check if user is authorized to perform the action
		$this->checkAuthorization(null);
		
		// Generate a version distribution graph for currently selected project
		CrashReport::generateVersionDistributionGraph($w, $h);
	}
    
    public function actionOsVersionDist($w, $h)
	{	
		// Check if user is authorized to perform the action
		$this->checkAuthorization(null);
		
		// Generate a version distribution graph for currently selected project
		CrashReport::generateOSVersionDistributionGraph($w, $h);
	}
    
    public function actionGeoLocationDist($w, $h)
	{			
        // Check if user is authorized to perform the action
        $this->checkAuthorization(null);

        // Generate a version distribution graph for currently selected project
        CrashReport::generateGeoLocationDistributionGraph($w, $h);

	}

	/**
	 * Returns the data model based on the primary key given in the GET variable.
	 * If the data model is not found, an HTTP exception will be raised.
	 * @param integer the ID of the model to be loaded
	 */
	public function loadModel($id)
	{
		$model=CrashReport::model()->findByPk($id);
		if($model===null)
			throw new CHttpException(404,'The requested page does not exist.');
		return $model;
	}

	/**
	 * Checks if user is authorized to perform the action.
	 * @param CrashReport $model Authorization object. Can be null.
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
