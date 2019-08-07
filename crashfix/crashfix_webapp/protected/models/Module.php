<?php

/**
 * This is the model class for table "{{module}}".
 *
 * The followings are the available columns in table '{{module}}':
 * @property integer $id
 * @property integer $crashreport_id
 * @property string $name
 * @property integer $sym_load_status
 * @property string $loaded_pdb_name
 * @property string $loaded_image_name
 */
class Module extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return Module the static model class
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
		return '{{module}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('crashreport_id, name, sym_load_status', 'required'),
			//array('crashreport_id, sym_load_status', 'numerical', 'integerOnly'=>true),
			array('name', 'length', 'max'=>256),
			//array('loaded_pdb_name, loaded_image_name', 'length', 'max'=>512),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, crashreport_id, name, sym_load_status, loaded_pdb_name, loaded_image_name', 'safe', 'on'=>'search'),
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
			'debuginfo'=>array(self::BELONGS_TO, 'DebugInfo', 'loaded_debug_info_id'),
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'id' => 'ID',
			'crashreport_id' => 'CrashReport',
			'name' => 'Module Name',
			'sym_load_status' => 'Symbol Status',
			'loaded_pdb_name' => 'Symbol File',
			'loaded_image_name' => 'Loaded Image Name',
            'matching_pdb_guid' => 'PDB GUID+Age',
		);
	}
}