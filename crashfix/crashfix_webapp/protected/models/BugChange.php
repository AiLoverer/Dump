<?php

/**
 * This is the model class for table "{{bug_change}}".
 *
 * The followings are the available columns in table '{{bug_change}}':
 * @property integer $id
 * @property integer $bug_id
 * @property integer $timestamp
 * @property integer $user_id
 * @property integer $staus_change_id
 * @property integer $comment_id
 * @property integer $attachment_id
 */
class BugChange extends CActiveRecord
{
	const FLAG_INITIAL_CHANGE = 1; // This change is the initial change (on bug creation).
	
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return BugChange the static model class
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
		return '{{bug_change}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('bug_id, timestamp, user_id', 'required'),
			array('bug_id, timestamp, user_id, status_change_id, comment_id', 'numerical', 'integerOnly'=>true),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, bug_id, timestamp, user_id, status_change_id, comment_id', 'safe', 'on'=>'search'),
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
			'user'=>array(self::BELONGS_TO, 'User', 'user_id'),
			'comment'=>array(self::BELONGS_TO, 'BugComment', 'comment_id'),
			'attachments'=>array(self::HAS_MANY, 'BugAttachment', 'bug_change_id'),
			'statuschange'=>array(self::BELONGS_TO, 'BugStatusChange', 'status_change_id'),
		);
	}

	/**
	 *   This method is called before deleting AR model from database table.
	 *   In this method, we delete file attachment.
	 */
	protected function beforeDelete()
	{
		// First call the base class
		if(!parent::beforeDelete())
			return false;
		
		// Remove related comment		
		if($this->comment)
		{
			$this->comment->delete();
		}
		
		// Delete related file attachments
		foreach($this->attachments as $attachment)
		{
			$attachment->delete();
		}
        
        // Delete related bug status change
		if(isset($this->statuschange))
		{
			$this->statuschange->delete();
		}
		
		return true;
	}
		
}