<div class="span-27 last">

<!-- Grid view -->
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$model->searchModules(),
	  'selectableRows'=>null,
      'columns'=>array(
		  'name',
		  array(        
			'name'=>'sym_load_status',
			'type'=>'text',
			'value'=>'Lookup::item(\'SymLoadStatus\', $data->sym_load_status)',
		  ),
		  array(        
			'name'=>'loaded_pdb_name',
			'type'=>'raw',
			'value'=>'$data->debuginfo==null?"n/a":CHtml::link(CHtml::encode($data->debuginfo->filename), array("debugInfo/view", "id"=>$data->debuginfo->id))',
		  ),
		  'file_version',
		  array(  
            'name'=>'timestamp',
			'type'=>'text',
            'value'=>'date("d/m/y H:i", $data->timestamp)',			  
          ),
          array(  
            'name'=>'matching_pdb_guid',
			'type'=>'text',
            'value'=>'$data->matching_pdb_guid!=""?$data->matching_pdb_guid:"n/a"',			  
          ),
      ),
 )); 
  
 ?>

</div>