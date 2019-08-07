<?php

/**
 * This is the model class for table "{{stackframe}}".
 *
 * The followings are the available columns in table '{{stackframe}}':
 */
class StackFrame extends CActiveRecord
{
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return StackFrame the static model class
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
		return '{{stackframe}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			//array('thread_id, title', 'required'),			
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id', 'safe', 'on'=>'search'),
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
			'module'=>array(self::BELONGS_TO, 'Module', 'module_id'),		
		);
	}
	
	/**
	 * Formats the title of this stack frame.
	 */
	public function getTitle()
	{
		$title = "";
		
		// Check if this is a special frame [UnwindInfoNotAvail]
		if($this->addr_pc==0 && $this->module_id==0)
		{
			$title = '[WARNING: Stack unwind information not available. Frames below may be wrong.]';
			return $title;
		}
		
		// Check if symbol name available
		if(isset($this->symbol_name) || isset($this->und_symbol_name))
		{
			if(isset($this->module_id) && isset($this->module))
			{
				$moduleName = $this->module->name;
				$title = $title.$moduleName.'! ';			
			}
			
			if(isset($this->und_symbol_name))
				$title = $title.$this->und_symbol_name.' ';
			else if(isset($this->symbol_name))
				$title = $title.$this->symbol_name.' ';
			$title.='+0x'.dechex($this->offs_in_symbol).' ';

			if(isset($this->src_file_name) && isset($this->src_line))
			{			
				$title .= '['.basename($this->src_file_name).': '.$this->src_line.']';
			}
            else if(isset($this->src_line) && $this->src_line>=0)
			{			
				$title .= '[line '.$this->src_line.']';
			}
		}
		else // Symbol name not available
		{		
			if(isset($this->module_id) && isset($this->module))
			{
                $moduleName = $this->module->name;
                $title = $title.$moduleName.'!+0x'.dechex($this->offs_in_module);			                                				
			}
			else
			{
				$title = '0x'.dechex($this->addr_pc);			
			}
		}		
		
		return $title;
	}
}