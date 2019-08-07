<?php

/**
 * This is the model class for table "{{fileitem}}".
 *
 * The followings are the available columns in table '{{fileitem}}':
 * @property integer $id
 * @property integer $crashreport_id
 * @property string $filename
 * @property string $description
 */
class FileItem extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return FileItem the static model class
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
		return '{{fileitem}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('crashreport_id', 'required'),
			array('crashreport_id', 'numerical', 'integerOnly'=>true),
			array('filename, description', 'length', 'max'=>256),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, crashreport_id, filename, description', 'safe', 'on'=>'search'),
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

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'crashreport_id' => 'Crash Report',
			'filename' => 'File Name',
			'description' => 'Description',
		);
	}	
}