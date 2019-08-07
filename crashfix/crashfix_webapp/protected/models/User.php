<?php

/**
 * This is the model class for table "{{user}}".
 * 
 * A user is an identity that can login to the site and perform some actions.
 * A user belongs to one user group.
 * 
 * A user may be active or disabled. Active users can login, 
 * while disabled ones cant.
 * 
 * A user may be standard (like root). A standard user cant be deleted, 
 * disabled or updated. Non-standard users can be.
 */
class User extends CActiveRecord
{
	// User statuses.
	const STATUS_ACTIVE  = 1;  // This user is an active user.
	const STATUS_DISABLED = 2; // This user is a retired user.
	
	// Flags.
	const FLAG_STANDARD_USER           = 0x1;  // This user is a standard account.	
	const FLAG_PASSWORD_RESETTED       = 0x2;  // This user must change his password on login.
	
    public $project_member = false; // Used in ProjectController::addUser action
    
	/**
	 * Returns the static model of the specified AR class.
	 * @return User the static model class
	 */
	public static function model($className=__CLASS__)
	{
		return parent::model($className);
	}

	/**
	 * @return string the associated database table name
	 */
	public function tableName()
	{
		return '{{user}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('username, usergroup, email', 'required', 'on'=>'create, update'),
			array('username', 'length', 'max'=>32, 'on'=>'create, update'),
			//array('email', 'email', 'on'=>'create'),
			array('username', 'checkUserNameFree', 'on'=>'create'),
			// The following rule is used by search().			
			array('username, email, status', 'safe', 'on'=>'search'),
		);
	}

	/**
	 *   This validator method checks if provided user name is free or not.
	 *   @return boolean true if user doesn't exist, otherwise false.
	 */
	public function checkUserNameFree($attribute, $params)
	{
		if($this->hasErrors())
			return false;
			
		$user = User::model()->find('username=:username', array(':username'=>$this->username));
		if($user==Null)
			return true;
		
		// Such a user name already exists.
		$this->addError('username', 'Such a user name already exists.');
		return false;
	}
	
	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
			'group'=>array(self::BELONGS_TO, 'UserGroup', 'usergroup'),
            'projects'=>array(self::HAS_MANY, 'UserProjectAccess', 'user_id'),
		);
	}

	
	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'username' => 'User Name',
			'salt' => 'Salt',
			'password' => 'Password',
			'usergroup' => 'User Group',
			'email' => 'Email',
			'profile' => 'Profile'
		);
	}

	/**
	 * Retrieves a list of models based on the current search/filter conditions.
	 * @return CActiveDataProvider the data provider that can return the models based on the search/filter conditions.
	 */
	public function search()
	{
		$criteria=new CDbCriteria;		
			
		// Fill search criteria
		if(isset($this->username))
		{
			$criteria->compare('username', $this->username, true, 'OR');		
			$criteria->compare('email', $this->username, true, 'OR');	
		}				
		
		$status = self::STATUS_ACTIVE;
		if(isset($this->status))
			$status = $this->status;
		
		if($status!=-1)
			$criteria->addCondition('t.status='.$status, 'AND');
			
		$criteria->join = 'LEFT JOIN {{usergroup}} g ON t.usergroup=g.id';
			
		if($status==self::STATUS_ACTIVE)			
			$criteria->addCondition('g.status='.UserGroup::STATUS_ACTIVE, 'AND');								
			
		if($status==self::STATUS_DISABLED)			
			$criteria->addCondition('g.status='.UserGroup::STATUS_DISABLED, 'OR');								
				
		return new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
		));
	}
	
	/**
	 * This method compares the hashed password stored in the database with the hash
	 * of the given password string. If they match, returns true.
	 * @return true if password match.
	 */
	public function validatePassword($password)
	{
		return $this->hashPassword($password, $this->salt)==$this->password;		
	}
				
	/**
	 * This callback is executed before saving the AR to database.
	 * In this method, we set some internally used attributes of the model.
	 * @return True on success.
	 */
    public function beforeSave()
    {
		// Check if this is a new record
        if($this->isNewRecord) 
		{
			// Set FLAG_PASSWORD_RESETTED flag, so user will be asked to 
			// change password on first login
			$this->flags |= self::FLAG_PASSWORD_RESETTED;
			
			// Generate one-time access token for account activation
			$this->pwd_reset_token = $this->randomSalt();		
			
			// Set status to Active
			$this->status = self::STATUS_ACTIVE;
			
			// Set default random password
			$this->password = $this->randomSalt();
			
			// We need to protect the password before saving it.
            $this->protectPassword();			
        }		
		
		// Done
        return true;
    }
	
	/**
	 * This callback is executed before saving the AR to database.
	 * @return void
	 */
    public function beforeDelete()
	{
		// Check if this is a standard user - 
		// do not allow to delete standard users.
		if($this->isStandard())
			return false;
		
		// Find all roles of this user
		$models = UserProjectAccess::model()->findAll('user_id='.$this->id);
		
		// Delete each role associated with this user
		foreach($models as $model)
		{
			$model->delete();
		}
		
		return true;
	}
	
	/**
	 * This method calculates password hash based on raw password and 
	 * random salt value and saves the hashed password and salt into 
	 * the database.
	 * @return void
	 */
	public function protectPassword()
	{
		// Generate random salt
		$salt = self::randomSalt();
		
		// Calculate password hash and store it in database instead of raw password
        $this->password = self::hashPassword($this->password, $salt);
		
		// Save salt in the database along with hashed password
        $this->salt = $salt;
	}
	
	/**
	 * This helper method calculates password hash based on raw password 
	 * text and salt. 
	 * @return string MD5 hash of the password.
	 */
	private function hashPassword($password, $salt)
	{
		return md5($salt.$password);
	}
		
	/**
	 *  This helper method generates a random salt value.
	 *  @param integer $length Desired length of salt string.
	 *  @return string Salt string
	 */
    private function randomSalt($length=32)
    {
        $chars = "abcdefghijkmnopqrstuvwxyz023456789";
        srand((double)microtime()*1000000);
        $i = 1;
        $salt = '' ;

        while ($i <= $length)
        {
            $num = rand() % 33;
            $tmp = substr($chars, $num, 1);
            $salt .= $tmp;
            $i++;
        }
        return $salt;
    } 
		
	/**
	 *  This method adds an email to pending mail queue. The email contains
	 *  new user account activation link.
	 *  @return boolean True on success.
	 */
	public function sendEmailWithAccountActivationLink()
	{	
		// Send an E-mail with account activation link
		$emailFrom = "no-reply@".Yii::app()->request->serverName;
		$emailSubject = "[CrashFix] Account Activation";
		$emailText = "This message has been sent to you, because someone has created\r\n";
		$emailText .= "a CrashFix account for you.\r\n\r\n";
		$emailText .= "IMPORTANT: If you did not request to create a CrashFix account for you,\r\n";
		$emailText .= "tell your administrator about this letter.\r\n\r";
		$emailText .= "If you did request to create an account for you, then please follow\r\n";
		$emailText .= "this link to login into your CrashFix account and enter your new password:\r\n";
		$emailText .= Yii::app()->createAbsoluteUrl('site/login', array('prt'=>$this->pwd_reset_token));
		$emailText .= "\r\n";				
		$headers="From: {$emailFrom}\r\nReply-To: {$emailFrom}";	
		
		// Add an email to mail queue.
		return MailQueue::addMail($this->email, $emailSubject, $emailText);		
	}
	
	/**
	 * Sets user status to 'Disabled'. 
	 * Can't disable standard user (root).
	 * @return boolean True if successfull.
	 */
	public function retire()
	{
		// Check if user is a standard user (root).
		if($this->isStandard())
			return false; // Cant disable standard user (root).
			
		// Change status to disabled.
		$this->status = self::STATUS_DISABLED;
		
		// Apply changes.
		return $this->save();
	}
	
	/**
	 *  Determines effective user's status (depending on user's group status).
	 *  For example, when user's group is disabled, user also becomes disabled.
	 *  @return integer Effective status.
	 */
	public function getEffectiveStatus()
	{				
		if($this->status != self::STATUS_DISABLED)
		{
			// Get status of group this user belong to
			$group = UserGroup::model()->find('id='.$this->usergroup);
			if($group===Null)
				throw CHttpException(403, 'Unexpected database state');
		
			// If group is disabled, than user is also disabled
			if($group->status == UserGroup::STATUS_DISABLED)
				return User::STATUS_DISABLED;				
		}
		
		// Return status
		return $this->status;
	}
	
	/**
	 * Formats an effective status string ('Active', 'Disabled' or 
	 * 'Disabled (group)').
	 * @return string Status string
	 */
	public function getEffectiveStatusStr()
	{
		if($this->isGroupDisabled())
			return 'Disabled (group)';
		
		return Lookup::item('UserStatus', $this->status);
	}
	
	/**
	 *  Determines if the group of this user is disabled. 
	 *  @return boolean True if user's group is disabled.
	 */
	public function isGroupDisabled()
	{
		return ($this->group->status == UserGroup::STATUS_DISABLED);
	}
		
	/**
	 *  Sets user status to 'Active'.
	 *  @return boolean True on success.
	 */
	public function resurrect()
	{
		$this->status = self::STATUS_ACTIVE;
		return $this->save();
	}
	
	/**
	 *  Determines if this user is a standard user (like root).
	 *  @return boolean True if user is a standard user.
	 */
	public function isStandard()
	{
		return 0!=($this->flags & self::FLAG_STANDARD_USER);		
	}
	
	/**
	 *  This method returns true if the user needs to reset his/her password 
	 *  on login.
	 *  @return boolean True if password has been resetted.
	 */
	public function isPasswordResetted()
	{
		return 0!=($this->flags&self::FLAG_PASSWORD_RESETTED);
	}

	/**
	 *  This method determines if there are any projects 
	 *  this user can be added to. 
	 *  @return boolean True if there are such projects.
	 */
	public function canAddProjectRole()
	{
		$projects = $this->findAllProjectsWithoutUser($this->id);
		if(count($projects)==0)
			return false; // There are no projects for this user
		
		// There is a project this user can be added to
		return true;
	}
	
	/**
	 * Selects all users not participating the given project.
	 * @param integer $projectId Project ID.
	 * @return array Array of User models.
	 */
	public static function findAllNotInProject($projectId)
	{
		$criteria = new CDbCriteria();
		$criteria->select = '*';
		$criteria->condition = 'id NOT IN (SELECT user_id FROM {{user_project_access}} WHERE {{user_project_access}}.project_id = :project_id)';
		$criteria->params = array(':project_id'=>$projectId);		
		return User::model()->findAll($criteria);
	}	
    
    /**
     * Checks if user belongs to a project.
     */
    public function isInProject($projectId)
	{
		$criteria = new CDbCriteria();		
		$criteria->compare('user_id', $this->id);
		$criteria->compare('project_id', $projectId);
		return (UserProjectAccess::model()->count($criteria)==1?true:false);
	}	
    
    /**
     * Suggests project role for this user.
     * @param integer $projectId Project ID.
     * @return integer Usergroup ID. 
     */
    public function suggestProjectRole($projectId)
	{
        // Check if user already has a role in the given project.
		$criteria = new CDbCriteria();		
		$criteria->compare('user_id', $this->id);
		$criteria->compare('project_id', $projectId);
		$userRole =  UserProjectAccess::model()->find($criteria);
        
        if($userRole!=null)
        {
            // Suggest the existing role
            return $userRole->usergroup_id;
        }
        
        return $this->usergroup; // else suggest this user's group as default role for this project
	}
	
	/**
	 * Selects all projects where the given user has no roles.
	 * @param type $userId User ID.
	 * @return array array of Project models.
	 */
	public function findAllProjectsWithoutUser()
	{
		$criteria = new CDbCriteria();
		$criteria->select = '*';
		$criteria->condition = 'id NOT IN (SELECT project_id FROM {{user_project_access}} WHERE {{user_project_access}}.user_id = :user_id)';
		$criteria->params = array(':user_id'=>$this->id);		
		return Project::model()->findAll($criteria);
	}
}