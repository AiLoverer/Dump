<?php

/**
 * This is the model class for table "{{appversion}}".
 *
 * The followings are the available columns in table '{{appversion}}':
 * @property integer $id
 * @property integer $project_id
 * @property string $version
 */
class AppVersion extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return AppVersion the static model class
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
		return '{{appversion}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('project_id, version', 'required'),
			array('project_id', 'numerical', 'integerOnly'=>true),
			array('version', 'length', 'min'=>1, 'max'=>32),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, project_id, version', 'safe', 'on'=>'search'),
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
	 * This method creates a new db record if such a version does not exist yet.
	 * @param string $appversion Version string.
	 * @return AppVersion On success, returns AppVersion object; otherwise Null.
	 */
	public static function createIfNotExists($appversion, $project_id=null)
	{
		if($project_id==null)
			$project_id = Yii::app()->user->getCurProjectId();
		if($project_id==null)
			return Null;
		
		// Ensure that project_id is a valid project ID
		$project = Project::model()->findByPk($project_id);
		if($project==null)
			return Null;
		
		// Try to find an existing version with such a name
		$criteria = new CDbCriteria;
		$criteria->compare('project_id', $project_id, false, 'AND');
		$criteria->compare('version', $appversion, false, 'AND');
		$model = AppVersion::model()->find($criteria);
		if($model!=Null)
			return $model;
		
		$model = new AppVersion;
		$model->project_id = $project_id;
		$model->version = $appversion;
		
		if(!$model->save())
			return Null;
		
		return $model;
	}	
}