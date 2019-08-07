<?php

class CrashReportForm extends CActiveRecord
{
	public $md5;
	public $crashguid;
	public $appname;
	public $appversion;
	public $emailsubject;
	public $emailfrom;
	public $description;

	/**
	 * Declares the validation rules.
	 * The rules state that username and password are required,
	 * and password needs to be authenticated.
	 */
	public function rules()
	{
		return array(
			// username and password are required
			array('crashguid, appname, appversion', 'required')			
		);
	}
	
	

}
