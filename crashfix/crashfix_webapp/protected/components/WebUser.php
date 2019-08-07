<?php

class WebUser extends CWebUser
{
	// Store User model to not repeat query.
	private $_model;
			
	/**
	 * Selects all active projects where the current user has any roles.
	 * @return Project The list of models.	 
	 */
	public function getMyProjects()
	{
		$criteria = new CDbCriteria();
		$criteria->select = '*';		
		$criteria->addCondition('id IN (SELECT project_id FROM {{user_project_access}} WHERE {{user_project_access}}.user_id = :user_id)', 'AND');		
		$criteria->params = array(':user_id'=>$this->id);		
		$criteria->addCondition('status='.Project::STATUS_ACTIVE, 'AND');
        $criteria->order = 'name ASC';
        
		return Project::model()->findAll($criteria);
	}	
	
	/**
	 * Checks if user can access certain project.
	 * @param type $projectId Project ID.
	 * @return boolean true if user can access the project.
	 */
	public function isMyProject($projectId)
	{
		$myProjects = $this->getMyProjects();
		foreach($myProjects as $myProject)
		{
			if($myProject->id==$projectId)
				return true; // This user can access this project.
		}
		
		// This user can't access this project.
		return false;
	}
	
	/**
	 * Returns the ID of user's current project or false if there is no such. 
	 * @return integer Project ID.     
	 */
	public function getCurProjectId()
	{
		// Get the list of projects accessible by the current user
		$myProjects = $this->getMyProjects();
		
        $user = $this->loadModel();
        if($user===null)
            return false;
        
        $projectId = $user->cur_project_id;
        
		if($projectId!=null)
		{
			// Check if cur project ID is a real project ID
			foreach($myProjects as $proj)
			{
				if($proj->id==$projectId)
					return $projectId;
			}
		}
		
		// Get the first of my projects and make it current		
		if(count($myProjects)>0)
			$projectId = $myProjects[0]->id;			
		else 
			$projectId = false;
			
		// Save current project ID to database
		$user->cur_project_id = $projectId;
        $user->save();
			
		return $projectId;		
	}
	
	/**
	 * This function returns currently selected project model.
	 * @return Project Project model.
	 */
	public function getCurProject()
	{
		$model = Project::model()->findByPk($this->getCurProjectId());
		return $model;
	}
	
	/**
	 * Sets current project ID. 
	 * @param integer $projectId Integer project ID.
	 * @return boolean True if succeeded, otherwise false.
	 */
	public function setCurProjectId($projectId)
	{
		// Get list of projects current user belongs to.
		$myProjects = $this->getMyProjects();		
        
        $user = Yii::app()->user->loadModel();
        
		// Check if given project ID is in user's project list.
		foreach($myProjects as $proj)		
		{			
			if($proj->id==$projectId)
			{
				// Save current project ID in session
				$user->cur_project_id = $projectId;											
                $user->save();
				// Add a message to log
				Yii::log('Cur project ID set to '.$projectId, 'info');
				//Success
				return true;
			}
		}		
		
		// Failed		
		return false;
	}
	
	/**
	 * Returns the list of app versions available for current project.
	 * @param $selVer integer On output, receives the currently selected version ID.
	 * @return array The list of versions.
	 */
	public function getCurProjectVersions(&$selVer)
	{
		// Check if there is a project currently selected
		if($this->getCurProjectId()==false)
		{
			// There is no project currently selected
			throw new CHttpException(403, 'Invalid request.');
		}
		
		// Select all versions for this project
		$criteria = new CDbCriteria;		
		$criteria->condition = 'project_id='.$this->getCurProjectId();
		$criteria->order = 'version DESC';
		$models = AppVersion::model()->findAll($criteria);
		
		// Prepare the list of versions.
		$versions = array();						
		foreach($models as $model)
		{
			$versions[$model->id] = $model->version;
		}
		
		if(count($versions)==0)
		{
			$versions[Project::PROJ_VER_NOT_SET]='(not set)';
		}
		
		$versions[Project::PROJ_VER_ALL]='(all)';
		
        $user = Yii::app()->user->loadModel();
        
		// Check which version is current
		if($user->cur_appversion_id!=null)
		{
			$selVer = $user->cur_appversion_id;			
			
			// Validate
			$found = false;
			foreach($versions as $key=>$val)
			{
				if($key==$selVer) 
				{
					$found = true; 
					break;
				}
			}
				
			if(!$found)
			{
				// Saved version is invalid				
			
				// Take the first version from list
				reset($versions); // Move array pointer to the first element
				$selVer = key($versions);
				
				// Save current ver
				$user->cur_appversion_id = $selVer;
                $user->save();
			}
		}
		else
		{
			// Take the first version from list
			reset($versions); // Move array pointer to the first element
			$selVer = key($versions);			
		}
		
		return $versions;
	}
	
	/**
	 *  Returns current project version ID. 
     * @return integer Cur version ID.
	 */
	public function getCurProjectVer()
	{
		$selVer = Project::PROJ_VER_NOT_SET;
		$versions = $this->getCurProjectVersions($selVer);
		return $selVer;
	}
	
	/**
	 * Sets current project's app version.
	 * @param $ver integer Version name ID.
	 * @return boolean True on success, otherwise false.
	 */
	public function setCurProjectVer($ver)
	{
		// Get available versions
		$prevVer = false;
		$versions = $this->getCurProjectVersions($prevVer);
		
		// Check if user-passed version is valid
		if(array_key_exists($ver, $versions))
		{
			// Save current ver
            $user = Yii::app()->user->loadModel();
            $user->cur_appversion_id = $ver;
            $user->save();
			
			return true;
		}
		
		// Failed.
		return false;
	}
	
	/**
	 * Loads user model.
	 * @param $id integer User ID.
	 * @return User User model.
	 */
	public function loadModel()
    {
        if($this->_model===null)
        {
            $this->_model=User::model()->findByPk($this->id);
        }
        return $this->_model;
    }
	
}

