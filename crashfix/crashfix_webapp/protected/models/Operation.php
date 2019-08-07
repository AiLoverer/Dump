<?php

class Operation extends CActiveRecord
{	
	// Operation status
	const STATUS_STARTED      = 1; // The daemon is being executing the operation.
	const STATUS_SUCCEEDED    = 2; // The operation succeeded.
	const STATUS_FAILED       = 3; // The operation has failed.
	
	// Operation types
	const OPTYPE_IMPORTPDB            = 1;  // The PDB import operation.
	const OPTYPE_PROCESS_CRASH_REPORT = 2;  // Crash report processing operation.
	const OPTYPE_DELETE_DEBUG_INFO    = 3;  // Debug info file deletion operation.
    
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
		return '{{operation}}';
	}

	/**
	 *   Formats a string description of operation. 
	 */
	public function getOperationDescStr()
	{
		$desc = "";
		
		if($this->optype==self::OPTYPE_IMPORTPDB)
		{
			$desc = "Import debug info file ".basename($this->operand1);
		}
		else if($this->optype==self::OPTYPE_PROCESS_CRASH_REPORT)
		{
			$desc = "Process crash report file ".basename($this->operand3);
		}        
        else if($this->optype==self::OPTYPE_DELETE_DEBUG_INFO)
		{
			$desc = "Delete debug info file ".basename($this->operand1);
		}
		
		return $desc;
	}
	
	/**
	 * This method deletes all completed operations not in top count.
	 * @param type $topCount Count of recent operations to keep.
	 * @return void
	 */
	public static function deleteOldOperations($topCount=1000)
	{
		// Get total count of records
		$totalCount = Operation::model()->count();
		$limitCount = $totalCount - $topCount;
		if($limitCount<=0)
			return; // Do nothing
		
		// Select the oldest operation records for deleting
		$criteria = new CDbCriteria();	
		$criteria->select = '*';
        $criteria->compare('status', '<>'.self::STATUS_STARTED);
		$criteria->order = 'timestamp DESC';
		$criteria->limit = $limitCount;				
		$ops = Operation::model()->findAll($criteria);		
		foreach($ops as $op)
		{
			$op->delete();
		}
	}	
	
	/**
	 * Retrieves a list of models based on the current search/filter conditions.
	 * @return CActiveDataProvider the data provider that can return the models based on the search/filter conditions.
	 */
	public function search()
	{	
		// Validate model
		$this->scenario = 'search';
		if(!$this->validate())
			return Null;
				
		// Perform search
		$dataProvider = new CActiveDataProvider($this);
		
		return $dataProvider;
	}
}

