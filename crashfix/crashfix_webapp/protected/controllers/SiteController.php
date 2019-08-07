<?php

class SiteController extends Controller
{	
	public $layout='//layouts/column2';
		
	/**
	 * Declares class-based actions.
	 */
	public function actions()
	{
		return array(
			// captcha action renders the CAPTCHA image displayed on the contact page
			'captcha'=>array(
				'class'=>'CCaptchaAction',
				'backColor'=>0xFFFFFF,
			),
			// page action renders "static" pages stored under 'protected/views/site/pages'
			// They can be accessed via: index.php?r=site/page&view=FileName
			'page'=>array(
				'class'=>'CViewAction',
			),
		);
	}
	
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
	public function accessRules()
	{
		return array(			
			array('allow', // Allow anybody see an error page
				'actions'=>array('error'),
				'users'=>array('*'),
			),
			array('allow', // Allow not authenticated users to login and recover password
				'actions'=>array('login', 'recoverPassword', 'captcha'),
				'users'=>array('?'),
			),
			array('allow', // Allow authenticated users to index, logout, reset password
				'actions'=>array('index', 'logout', 'resetPassword', 'setCurProject', 'checkDaemon'),
				'users'=>array('@'),
			),			
			array('allow', // Allow admin to access admin panel
				'actions'=>array('admin', 'daemon', 'daemonStatus'),
				'roles'=>array('gperm_access_admin_panel'),
			),			
			array('deny',  // deny all users
				'users'=>array('*'),
			),
		);
	}
	
	/**
	 * This is the default 'index' action that is invoked
	 * when an action is not explicitly requested by users.
	 */
	public function actionIndex()
	{
		// renders the view file 'protected/views/site/index.php'
		// using the default layout 'protected/views/layouts/main.php'
		$this->render('index');
	}

	/**
	 * This is the action to handle external exceptions.
	 */
	public function actionError()
	{
		$this->layout='//layouts/column1';
	    if($error=Yii::app()->errorHandler->error)
	    {
	    	if(Yii::app()->request->isAjaxRequest)
	    		echo $error['message'];
	    	else
	        	$this->render('error', $error);
	    }
	}

	/**
	 * Displays the login page
	 */
	public function actionLogin()
	{
		$model=new LoginForm('RegularLogin');
		
		// Check if user tries to login using one-time access link		
		if(isset($_GET['prt']))
		{			
			$oneTimeAccessToken = $_GET['prt'];
			$model->scenario = 'OneTimeLogin';
			$model->oneTimeAccessToken = $oneTimeAccessToken;
			
			// Validate user input and redirect to the user profile if valid
			if($model->validate() && $model->login())
				$this->redirect(array('site/resetPassword'));
			
			// If there was an error, throw an exception
			throw new CHttpException(403, 'Invalid request');			
			return;
		}
		
		// if it is ajax validation request
		if(isset($_POST['ajax']) && $_POST['ajax']==='login-form')
		{
			echo CActiveForm::validate($model);
			Yii::app()->end();
		}

		// collect user input data
		if(isset($_POST['LoginForm']))
		{
			$model->attributes=$_POST['LoginForm'];
			// validate user input and redirect to the previous page if valid
			if($model->validate() && $model->login())
			{
				$check = Yii::app()->user->checkAccess('gperm_access_admin_panel');
				Yii::log('check='.$check, 'info');
		
				// Check if user needs to reset password
				$user = User::model()->findByPk(Yii::app()->user->getId());
				if($user->isPasswordResetted())
				{
					// Redirect to Reset Password page
					$this->layout = '//layouts/column1';
					$this->redirect(array('site/resetPassword'));
				}
				else					
				{											
					// Change return url
					$user =	Yii::app()->user->loadModel();
					$defaultTab = $user->group->default_sidebar_tab;
					$returnUrl = array('site/index');
					if($defaultTab=='CrashReports')
						$returnUrl = array('crashReport/index');
					else if($defaultTab=='CrashGroups')
						$returnUrl = array('crashGroup/index');
					else if($defaultTab=='Bugs')
						$returnUrl = array('bug/index');
					else if($defaultTab=='DebugInfo')
						$returnUrl = array('debugInfo/index');
					else if($defaultTab=='Administer')
						$returnUrl = array('site/admin');						
					
					if(Yii::app()->user->returnUrl==Yii::app()->baseUrl);
						Yii::app()->user->setReturnUrl($returnUrl);					
					
					// Redirect to return URL
					$this->redirect(Yii::app()->user->returnUrl);
				}
			}
		}
		// display the login form
		$this->layout = '//layouts/column1';
		$this->render('login',array('model'=>$model));
	}
	
	/**
	 * Displays the Reset Password page.
	 */
	public function actionResetPassword()
	{
		$model=new ResetPasswordForm;

		// Collect user input data
		if(isset($_POST['ResetPasswordForm']))
		{
			$model->attributes=$_POST['ResetPasswordForm'];
			
			// Validate user input 
			if($model->validate())
			{				
				// Search {{user}} table for current user
				$user = User::model()->find('id='.Yii::app()->user->getId());
				if($user==Null)
					throw new CHttpException(403, "Invalid request.");
				
				// Update user's password and salt
				$user->password = $model->password;				
				// Store password as a hashed string with salt
				$user->protectPassword();
				// Reset the flag to avoid resetting the second time
				$user->flags &= ~User::FLAG_PASSWORD_RESETTED;
				// Update user record
				if(!$user->save())
					throw new CHttpException(403, "Couldn't update user info.");				
				
				// Redirect to user's profile page
				$this->redirect(array('user/view', 'id'=>Yii::app()->user->getId()));
			}
		}
		
		// Display the Recover Password form
		$this->layout = '//layouts/column1';
		$this->render('resetPassword', array('model'=>$model));
	}
	
	/**
	 * Displays the Recover Password page.
	 */
	public function actionRecoverPassword()
	{
		$model=new RecoverPasswordForm;

		// Collect user input data
		if(isset($_POST['RecoverPasswordForm']))
		{
			$model->attributes=$_POST['RecoverPasswordForm'];
			
			// Validate user input 
			if($model->validate())
			{
				// Generate a temporary password reset token
				$pwdResetToken = md5(uniqid(rand(), 1));
				
				// Search {{user}} table for existing user with such a name and email
				$user = User::model()->find(
						'username=:username AND email=:email', 
						array(':username'=>$model->username, ':email'=>$model->email));
				if($user==Null)
					throw new CHttpException(403, "Invalid request.");
				
				$user->pwd_reset_token = $pwdResetToken;
				if(!$user->save())
					throw new CHttpException(403, "Couldn't update user info.");				
				
				// Send an E-mail with password reset link
				$emailFrom = "no-reply@".Yii::app()->request->serverName;
				$emailSubject = "CrashFix Account Password Recovery";
				$emailText = "This message has been sent to you, because someone requested\r\n";
				$emailText .= "to recover lost password of your CrashFix account.\r\n\r\n";
				$emailText .= "IMPORTANT: If you did not request to recover your lost password,\r\n";
				$emailText .= "tell your administrator about this letter.\r\n\r";
				$emailText .= "If you did request to recover your lost password, then please follow\r\n";
				$emailText .= "this link to login into your CrashFix account and reenter your password:\r\n";
				$emailText .= $this->createAbsoluteUrl('site/login', array('prt'=>$pwdResetToken));
				$emailText .= "\r\n";				
				$headers="From: {$emailFrom}\r\nReply-To: {$emailFrom}";				
				if(@mail($model->email, $emailSubject, $emailText, $headers))
				{
					Yii::app()->user->setFlash('recoverPassword', 'An E-mail has been sent to your E-mail address with password recovery information. Please visit your mailbox.');
				}
				else
				{
					Yii::app()->user->setFlash('recoverPassword', 'There was an error when trying to send an E-mail to user\'s mailbox. Please contact your CrashFix administrator for further assistance.');
				}
					
				$this->refresh();
			}
		}
		
		// Display the Recover Password form
		$this->layout = '//layouts/column1';
		$this->render('recoverPassword',array('model'=>$model));
	}

	/**
	 * Logs out the current user and redirect to homepage.
	 */
	public function actionLogout()
	{
		Yii::app()->user->logout();
		$this->redirect(Yii::app()->homeUrl);
	}
	
	public function actionAdmin()
	{	
		if(!Yii::app()->request->isAjaxRequest)
		{
			$this->sidebarActiveItem = 'Administer';
			$this->adminMenuItem='General';
		
			$this->render('admin');
		}
		else
		{
			// Retrieve license info
			$licenseInfo = Yii::app()->daemon->getLicenseInfo();	
				
			// Render partial view
			$this->renderPartial('_licenseInfo', 
				array('licenseInfo'=>$licenseInfo));					
			
			// Done
			Yii::app()->end();
		}
	}
	
	/**
	 *  This action is used to display daemon status and recent operations. 
	 */
	public function actionDaemon()
	{		
		$this->sidebarActiveItem = 'Administer';
		$this->adminMenuItem='Daemon';
						
		$dataProvider = new CActiveDataProvider('Operation', array(
                'pagination'=>array(
                    'pageSize'=>30
                ),
                'sort'=>array(
                    'defaultOrder'=>'timestamp DESC'
                )
            )
        );
		
        $this->render('daemon', array('dataProvider'=>$dataProvider));		
	}
	
	/**
	 *   Ajax only. Retrieves daemon status as a string.
	 */
	public function actionDaemonStatus()
	{
		if(Yii::app()->request->isAjaxRequest)
		{			
			// Check daemon status
			$daemonResponce = "";
			$daemonRetCode = Yii::app()->daemon->getDaemonStatus($daemonResponce);
			
			// Split string
			$list = preg_split('#;#', $daemonResponce);	
            
			$this->renderPartial('_daemonStatus', 
					array('daemonRetCode'=>$daemonRetCode, 'list'=>$list));
			            
            Yii::app()->end();
        }
	}
	
	/**
	 * This action is executed for every page requested by user to check
	 * if daemon is currently active. 
	 */
	public function actionCheckDaemon()
	{	
		$realCheck = false;
        $errorMsg = 'Unspecified error';
		$retCode = Yii::app()->daemon->checkDaemon($realCheck, $errorMsg);
		
		if($realCheck && $retCode!=Daemon::DAEMON_CHECK_OK)
        {
            $this->renderPartial('_daemonCheck', 
                    array('retCode'=>$retCode,'errorMsg'=>$errorMsg));
        }
	}
	
	/**
	 * This action sets current project for user currently being logged in.
	 */
	public function actionSetCurProject($proj, $ver)
	{	
		// Update current project
		Yii::app()->user->setCurProjectId($proj);
		
		// Update current project version
		Yii::app()->user->setCurProjectVer($ver);
		
		// Refresh the refferer page
		$urlReferrer = Yii::app()->request->urlReferrer;
		$this->redirect($urlReferrer);
	}
}