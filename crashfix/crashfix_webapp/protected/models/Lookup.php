<?php

class Lookup extends CActiveRecord
{
	/**
	 * The followings are the available columns in table 'tbl_lookup':
	 * @var integer $id
	 * @var string $object_type
	 * @var integer $code
	 * @var string $name_en
	 * @var string $name_fr
	 * @var integer $sequence
	 * @var integer $status
	 */

	private static $_items=array();

	/**
	 * Returns the static model of the specified AR class.
	 * @return CActiveRecord the static model class
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
		return '{{lookup}}';
	}

	/**
	 * Returns the items for the specified type.
	 * @param string item type (e.g. 'PostStatus').
	 * @return array item names indexed by item code. The items are order by their position values.
	 * An empty array is returned if the item type does not exist.
	 */
	public static function items($type, $maxCode=null)
	{
		if(!isset(self::$_items[$type]))
			self::loadItems($type, $maxCode);
		return self::$_items[$type];
	}

	/**
	 * Returns the item name for the specified type and code.
	 * @param string the item type (e.g. 'PostStatus').
	 * @param integer the item code (corresponding to the 'code' column value)
	 * @return string the item name for the specified the code. False is returned if the item type or code does not exist.
	 */
	public static function item($type,$code)
	{
		if(!isset(self::$_items[$type]))
			self::loadItems($type);
		return isset(self::$_items[$type][$code]) ? self::$_items[$type][$code] : false;
	}

	/**
	 * Loads the lookup items for the specified type from the database.
	 * @param string the item type
	 */
	private static function loadItems($type, $maxCode = null)
	{		
		self::$_items[$type]=array();
		
		$criteria = new CDbCriteria();
		$criteria->condition = 'type=:type';
		$criteria->params = array(':type'=>$type);
		$criteria->order = 'position';
		
		if($maxCode!=null)
			$criteria->addCondition('code <= '.$maxCode);
		
		$models=self::model()->findAll($criteria);
		foreach($models as $model)
			self::$_items[$type][$model->code]=$model->name;
	}
	
	/**
	 * Clears loaded items.
	 */
	public static function reset()
	{
		self::$_items = array();
	}
}

