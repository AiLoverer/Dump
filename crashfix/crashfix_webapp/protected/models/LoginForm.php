<?php

/**
 * LoginForm class.
 * LoginForm is the data structure for keeping
 * user login form data. It is used by the 'login' action of 'SiteController'.
 */
class LoginForm extends CFormModel
{
	public $username;
	public $password;
	public $rememberMe;
	public $oneTimeAccessToken; // Used for one-time access (when recovering forgotten password)

	private $_identity;

	/**
	 * Declares the validation rules.
	 * The rules state that username and password are required,
	 * and password needs to be authenticated.
	 */
	public function rules()
	{
		return array(
			// username and password are required
			array('username, password', 'required', 'on'=>'RegularLogin'),
			// rememberMe needs to be a boolean
			array('rememberMe', 'boolean', 'on'=>'RegularLogin'),
			// password needs to be authenticated
			array('password', 'authenticate', 'on'=>'RegularLogin'),
			array('oneTimeAccessToken', 'checkAccessToken', 'on'=>'OneTimeLogin'),
		);
	}

	/**
	 * Declares attribute labels.
	 */
	public function attributeLabels()
	{
		return array(
			'rememberMe'=>'Remember me next time',
		);
	}

	/**
	 * Authenticates the password.
	 * This is the 'authenticate' validator as declared in rules().
	 */
	public function authenticate($attribute,$params)
	{
		if(!$this->hasErrors())
		{
			$this->_identity=new UserIdentity($this->username,$this->password);			
			if(!$this->_identity->authenticate())
				$this->addError('password','Incorrect username or password.');
		}
	}
	
	/**
	 * This validator method checks the access token when user tries to login to recover lost password.	 
	 */
	public function checkAccessToken($attribute, $params)
	{
		if($this->hasErrors())
			return false;
		
		$this->_identity=new UserIdentity($this->username, $this->password);			
		$this->_identity->oneTimeAccessToken = $this->oneTimeAccessToken;
		if(!$this->_identity->authenticate())
				$this->addError('username','Incorrect access token.');
		
		return true;
	}

	/**
	 * Logs in the user using the given username and password in the model.
	 * @return boolean whether login is successful
	 */
	public function login()
	{		
		if($this->_identity===null)
		{
			$this->_identity=new UserIdentity($this->username,$this->password);
			if($this->oneTimeAccessToken!=Null)
				$this->_identity->oneTimeAccessToken = $this->oneTimeAccessToken;
			$this->_identity->authenticate();
		}
		
		if($this->_identity->errorCode===UserIdentity::ERROR_NONE)
		{
			$duration=$this->rememberMe ? 3600*24*30 : 0; // 30 days
			Yii::app()->user->login($this->_identity, $duration);
			return true;
		}
		else
			return false;
	}
	
	public function getUserId()
	{
		if($this->_identity===null)
			return False;
		else
			return $this->_identity->getId();
	}	
}