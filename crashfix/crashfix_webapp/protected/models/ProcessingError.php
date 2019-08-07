<?php

/**
 * This is the model class for table "{{processingerror}}".
 *
 * The followings are the available columns in table '{{processingerror}}':
 * @property integer $id
 * @property string $crashreport_id
 * @property string $message
 */
class ProcessingError extends CActiveRecord
{
	// Processing error types
	const TYPE_CRASH_REPORT_ERROR = 1; // This error is associated with a crash report.
	const TYPE_DEBUG_INFO_ERROR   = 2; // This error is associated with a debug info.
	
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return ProcessingError the static model class
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
		return '{{processingerror}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('type, srcid', 'required'),
			array('srcid', 'checkSrcId'),
			array('message', 'length', 'max'=>512),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, type, srcid, message', 'safe', 'on'=>'search'),
		);
	}
	
	/**
	 * This validator method checks if srcid is a valid crash report ID or
	 * debug info ID. 
	 */
	public function checkSrcId()
	{
		if($this->type==ProcessingError::TYPE_CRASH_REPORT_ERROR)
		{
			$crashReport = CrashReport::model()->find('id='.$this->srcid);
			if($crashReport===Null)
			{
				$this->addError('srcid', 'SrcID must be a valid crash report ID.');
				return false;
			}
		}
		else if($this->type==ProcessingError::TYPE_DEBUG_INFO_ERROR)
		{
			$debugInfo = DebugInfo::model()->find('id='.$this->srcid);
			if($debugInfo===Null)
			{
				$this->addError('srcid', 'SrcID must be a valid debug info ID.');
				return false;
			}
		}			
		
		return true;
	}
}