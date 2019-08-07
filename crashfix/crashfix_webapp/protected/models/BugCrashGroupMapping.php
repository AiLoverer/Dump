<?php

/**
 * This is the model class for table "{{bug_crashgroup}}".
 *
 * The followings are the available columns in table '{{bug_crashgroup}}':
 * @property integer $id
 * @property integer $bug_id
 * @property integer $crashgroup_id
 */
class BugCrashGroupMapping extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return BugCrashGroupMapping the static model class
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
		return '{{bug_crashgroup}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('bug_id, crashgroup_id', 'required'),
			array('bug_id, crashgroup_id', 'numerical', 'integerOnly'=>true),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, bug_id, crashgroup_id', 'safe', 'on'=>'search'),
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
			'bug'=>array(self::BELONGS_TO, 'Bug', 'bug_id'),
			'crashGroup'=>array(self::BELONGS_TO, 'CrashGroup', 'crashgroup_id'),
		);
	}	
}