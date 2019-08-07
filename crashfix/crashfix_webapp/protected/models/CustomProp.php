<?php

/**
 * This is the model class for table "{{customprop}}".
 *
 * The followings are the available columns in table '{{customprop}}':
 * @property integer $id
 * @property integer $crashreport_id
 * @property string $name
 * @property string $value
 */
class CustomProp extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return CustomProp the static model class
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
		return '{{customprop}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('crashreport_id, name, value', 'required'),
			array('crashreport_id', 'numerical', 'integerOnly'=>true),
			array('name', 'length', 'max'=>128),
			array('value', 'length', 'max'=>512),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, crashreport_id, name, value', 'safe', 'on'=>'search'),
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
		);
	}
}