<?php

/**
 * This is the model class for table "{{thread}}".
 *
 * The followings are the available columns in table '{{thread}}':
 * @property integer $id
 * @property string $thread_id
 * @property integer $crashreport_id
 */
class Thread extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return Thread the static model class
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
		return '{{thread}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('thread_id, crashreport_id', 'required'),
			array('crashreport_id', 'numerical', 'integerOnly'=>true),
			array('thread_id', 'length', 'max'=>10),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, thread_id, crashreport_id', 'safe', 'on'=>'search'),
		);
	}

	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
			'crashreport'=>array(self::BELONGS_TO, 'CrashReport', 'crashreport_id'),
			'stackFrames'=>array(self::HAS_MANY, 'StackFrame', 'thread_id'),
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'thread_id' => 'Thread ID',
			'crashreport_id' => 'Crash Report',
		);
	}
	
	/**
	 * Scans stack trace for this thread and retrieves the lowermost frame 
	 * with symbol information. The information is considered as function 
	 * where exception has occurred. 	 
	 */
	public function getExceptionStackFrameTitle()
	{
		// Look for uppermost stack frame.
		$criteria=new CDbCriteria;
		$criteria->compare('thread_id', $this->id);		
		$criteria->addCondition('und_symbol_name IS NOT NULL');
		$criteria->order = 'id ASC';		
		$stackFrames = StackFrame::model()->findAll($criteria);
		
		// Not found such a stack frame.
		if($stackFrames===null)
			return false;
		
		$title = '';
		
		// Walk through stack frames
		foreach($stackFrames as $stackFrame)
		{
			$title = $stackFrame->title;
			
            if(isset($stackFrame->module))
            {
                // Ingore this stack frame if it belongs to CrashRpt module
                if(0!=preg_match('/^CrashRpt([0-9]{4})(d{0,1}){0,1}\.dll$/', $stackFrame->module->name))
                    continue;	
            }
			
			break;
		}
		
		// Return stack frame title.
		return $title;		
	}
	
	/**
	 * Scans stack trace for this thread and retrieves the uppermost frame 
	 * with symbol information. The information is considered as thread entry 
	 * point function name. 
	 */
	public function getThreadFuncName()
	{
		// Look for uppermost stack frame.
		$criteria=new CDbCriteria;
		$criteria->compare('thread_id', $this->id);		
		$criteria->addCondition('symbol_name IS NOT NULL');
		$criteria->order = 'id DESC';
		$stackFrame = StackFrame::model()->find($criteria);
		if($stackFrame===null)
			return 'n/a';
		else
		{
			if(isset($stackFrame->und_symbol_name))
				return $stackFrame->und_symbol_name;
			else 
				return $stackFrame->symbol_name;
		}
	}
	
	/**
	 * This method is called before the AR is deleted.
	 * @return boolean true on success. 
	 */
	protected function beforeDelete()
	{
		// Call base class
		if(!parent::beforeDelete())
			return false;
		
		// Delete all related stack frames
		foreach($this->stackFrames as $stackFrame)
		{
			$stackFrame->delete();
		}
		
		// OK
		return true;
	}
	
}