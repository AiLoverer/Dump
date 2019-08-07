<?php

/**
 * ResetPasswordForm class.
 * ResetPasswordForm is the data structure for keeping
 * password form data. It is used by the 'resetPassword' action of 'SiteController'.
 */
class ResetPasswordForm extends CFormModel
{	
	public $password;
	public $passwordConfirmation;

	/**
	 * Declares the validation rules.
	 */
	public function rules()
	{
		return array(			
			array('password, passwordConfirmation', 'required'),			
			array('password', 'length', 'min'=>6, 'max'=>48),
			array('passwordConfirmation', 'compare', 'compareAttribute'=>'password'),			
		);
	}

	/**
	 * Declares attribute labels.
	 */
	public function attributeLabels()
	{
		return array(
			'username'=>'User Name',
			'password'=>'Password',			
			'passwordConfirmation'=>'Confirm Password',			
		);
	}
}
