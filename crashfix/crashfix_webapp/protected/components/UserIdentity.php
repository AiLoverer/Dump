<?php

/**
 * UserIdentity represents the data needed to identity a user.
 * It contains the authentication method that checks if the provided
 * data can identity the user.
 */
class UserIdentity extends CUserIdentity
{
	public $oneTimeAccessToken; // Used for one-time access when recovering lost password
	
	private $_id;		
	
	public function authenticate()
	{
		$user = null;
		
		// Check if one-time access token is set
		if(isset($this->oneTimeAccessToken))
		{
			$user = User::model()->find('pwd_reset_token=:pwd_reset_token', array('pwd_reset_token'=>$this->oneTimeAccessToken));
			if($user===null)
			{
				// Not found such a user
				$this->errorCode=self::ERROR_USERNAME_INVALID;
			}
			else if($user->pwd_reset_token==$this->oneTimeAccessToken)
			{
				// Reset access token
				$user->pwd_reset_token = ""; 
				if(!$user->save())
					$this->errorCode=self::ERROR_UNKNOWN_IDENTITY;
				else
					$this->errorCode=self::ERROR_NONE;					
			}
		}
		else
		{
			$username = strtolower($this->username);
			$user = User::model()->find('LOWER(username)=?', array($username));
		
			if($user===null)
			{
				// Not found such a user
				$this->errorCode=self::ERROR_USERNAME_INVALID;
			}
			else if(!$user->validatePassword($this->password))
			{
				// Password is invalid
				$this->errorCode=self::ERROR_PASSWORD_INVALID;
			}
			else
			{
				// Password valid
				$this->errorCode=self::ERROR_NONE;
			}			
		}
		
		// Check user status (do not allow disabled users to login)
		if($user==null || $user->status==User::STATUS_DISABLED)
		{
			// Do not allow this user to login.
			$this->errorCode=self::ERROR_USERNAME_INVALID;
		}
		
		// Check authentication result
		if($this->errorCode==self::ERROR_NONE)
		{
			// Success
			$this->_id = $user->id;
			$this->username = $user->username;	
						
			// Assign user roles
			$auth = Yii::app()->authManager;
			
            // Create new operations only if not created previously
            $ops = $auth->getOperations();
            
            if(!array_key_exists('gperm_access_admin_panel', $ops))
                $auth->createOperation('gperm_access_admin_panel','Access admin panel');

            if(!array_key_exists('pperm_browse_some_crash_reports', $ops))
                $auth->createOperation('pperm_browse_some_crash_reports', 'Browse some crash reports');
            
            if(!array_key_exists('pperm_browse_some_bugs', $ops))
                $auth->createOperation('pperm_browse_some_bugs', 'Manage some bugs');
            
            if(!array_key_exists('pperm_browse_some_debug_info', $ops))
                $auth->createOperation('pperm_browse_some_debug_info', 'Browse some debug info files');
               
            if(!array_key_exists('pperm_manage_some_crash_reports', $ops))
                $auth->createOperation('pperm_manage_some_crash_reports', 'Manage some crash reports');
                
            if(!array_key_exists('pperm_manage_some_bugs', $ops))    
                $auth->createOperation('pperm_manage_some_bugs', 'Manage some bugs');
            
            if(!array_key_exists('pperm_manage_some_debug_info', $ops))    
                $auth->createOperation('pperm_manage_some_debug_info', 'Manage some debug info files');

            $bizRule='return Yii::app()->user->isMyProject($params[\'project_id\']);';			
            
            if(!array_key_exists('pperm_browse_crash_reports', $ops))    
                $auth->createOperation('pperm_browse_crash_reports', 'Browse crash reports for assigned project', $bizRule);
            
            if(!array_key_exists('pperm_browse_bugs', $ops))    
                $auth->createOperation('pperm_browse_bugs', 'Browse bugs for assigned project', $bizRule);
            
            if(!array_key_exists('pperm_browse_debug_info', $ops))    
                $auth->createOperation('pperm_browse_debug_info', 'Browse debug info files for assigned project', $bizRule);
            
            if(!array_key_exists('pperm_manage_crash_reports', $ops))                
                $auth->createOperation('pperm_manage_crash_reports', 'Manage crash reports for assigned project', $bizRule);
            
            if(!array_key_exists('pperm_manage_bugs', $ops))                
                $auth->createOperation('pperm_manage_bugs', 'Manage bugs for assigned project', $bizRule);
            
            if(!array_key_exists('pperm_manage_debug_info', $ops))                
                $auth->createOperation('pperm_manage_debug_info', 'Manage debug info files for assigned project', $bizRule);
            			
            $bizRule='return Yii::app()->user->id==$params[\'user_id\'];';
            if(!array_key_exists('gperm_view_own_profile', $ops))                
                $auth->createOperation('gperm_view_own_profile', 'View own profile', $bizRule);
            
			if($user->group->gperm_access_admin_panel!=0 && !$auth->isAssigned('gperm_access_admin_panel', $this->_id))			
				$auth->assign('gperm_access_admin_panel', $this->_id);
			
			if($user->group->pperm_browse_crash_reports!=0)
			{
                if(!$auth->isAssigned('pperm_browse_some_crash_reports', $this->_id))
                    $auth->assign('pperm_browse_some_crash_reports', $this->_id);
                
                if(!$auth->isAssigned('pperm_browse_crash_reports', $this->_id))
                    $auth->assign('pperm_browse_crash_reports', $this->_id);
			}
			
			if($user->group->pperm_browse_bugs!=0)
			{
                if(!$auth->isAssigned('pperm_browse_some_bugs', $this->_id))
                    $auth->assign('pperm_browse_some_bugs', $this->_id);
                
                if(!$auth->isAssigned('pperm_browse_bugs', $this->_id))
                    $auth->assign('pperm_browse_bugs', $this->_id);
			}
			
			if($user->group->pperm_browse_debug_info!=0)
			{
                if(!$auth->isAssigned('pperm_browse_some_debug_info', $this->_id))
                    $auth->assign('pperm_browse_some_debug_info', $this->_id);
                
                if(!$auth->isAssigned('pperm_browse_debug_info', $this->_id))
                    $auth->assign('pperm_browse_debug_info', $this->_id);
			}
			
			if($user->group->pperm_manage_crash_reports!=0)
			{
                if(!$auth->isAssigned('pperm_manage_some_crash_reports', $this->_id))
                    $auth->assign('pperm_manage_some_crash_reports', $this->_id);
                
                if(!$auth->isAssigned('pperm_manage_crash_reports', $this->_id))
                    $auth->assign('pperm_manage_crash_reports', $this->_id);
			}
			
			if($user->group->pperm_manage_bugs!=0)
			{
                if(!$auth->isAssigned('pperm_manage_some_bugs', $this->_id))
                    $auth->assign('pperm_manage_some_bugs', $this->_id);
				
                if(!$auth->isAssigned('pperm_manage_bugs', $this->_id))
                    $auth->assign('pperm_manage_bugs', $this->_id);
			}
			
			if($user->group->pperm_manage_debug_info!=0)
			{
                if(!$auth->isAssigned('pperm_manage_some_debug_info', $this->_id))
                    $auth->assign('pperm_manage_some_debug_info', $this->_id);	
                
                if(!$auth->isAssigned('pperm_manage_debug_info', $this->_id))
                    $auth->assign('pperm_manage_debug_info', $this->_id);	
			}
			
            if(!$auth->isAssigned('gperm_view_own_profile', $this->_id))
                $auth->assign('gperm_view_own_profile', $this->_id);	
			
			$auth->save();
		}
			
		return $this->errorCode==self::ERROR_NONE;
	}
	
	/**
	 *  Returns user Id.
	 */
	public function getId()
	{
		return $this->_id;
	}
}
