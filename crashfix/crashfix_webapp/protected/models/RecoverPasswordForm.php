<?php

/**
 * RecoverPasswordForm class.
 * RecoverPasswordForm is the data structure for keeping
 * user name and email form data. It is used by the 'recoverPassword' action of 'SiteController'.
 */
class RecoverPasswordForm extends CFormModel
{
	public $username;
	public $email;
	public $verifyCode;

	/**
	 * Declares the validation rules.
	 * The rules state that username and password are required,
	 * and password needs to be authenticated.
	 */
	public function rules()
	{
		return array(
			// username and email are required
			array('username, email, verifyCode', 'required'),			
			array('username', 'checkUserAndEmail'),	
			array('verifyCode', 'captcha'),			
		);
	}

	/**
	 * Declares attribute labels.
	 */
	public function attributeLabels()
	{
		return array(
			'username'=>'User Name',
			'email'=>'E-mail',			
		);
	}

	/**
	 * This validator method checks if username and E-mail are real.	 
	 */
	public function checkUserAndEmail($attribute,$params)
	{
		if($this->hasErrors())
			return false;
			
		// Search {{user}} table for existing user with such a name and email
		$user = User::model()->find(
						'username=:username AND email=:email', 
						array(':username'=>$this->username, ':email'=>$this->email));
		if($user==Null)
		{
			$this->addError('username', 'The provided user name and/or E-mail address do not belong to any user.');
			return false; // Not found such a user
		}
		
		// User match.
		return true;
	}
	
}
