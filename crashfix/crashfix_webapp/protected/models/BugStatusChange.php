<?php

/**
 * This is the model class for table "{{bug_status_change}}".
 *
 * The followings are the available columns in table '{{bug_status_change}}':
 * @property integer $id
 * @property integer $bug_id
 * @property integer $status
 * @property integer $assigned_to
 * @property integer $priority
 * @property integer $reproducability
 * @property integer $merged_into
 */
class BugStatusChange extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return BugStatusChange the static model class
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
		return '{{bug_status_change}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(			
			array('status, assigned_to, priority, reproducability, merged_into', 'numerical', 'integerOnly'=>true),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, bug_id, status, assigned_to, priority, reproducability, merged_into', 'safe', 'on'=>'search'),
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
			'owner'=>array(self::BELONGS_TO, 'User', 'assigned_to'),
            'bugChange'=>array(self::HAS_ONE, 'BugChange', 'status_change_id'),
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'bug_id' => 'Bug',
			'status' => 'Status',
			'assigned_to' => 'Assigned To',
			'priority' => 'Priority',
			'reproducability' => 'Reproducibility',
			'merged_into' => 'Merged Into',
		);
	}	    
}