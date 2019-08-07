<?php
$this->breadcrumbs=array(	
	'Administer', 
	'Groups',
);

?>

<!-- Flash messages -->
<div class="span-18 last">
<?php	
    foreach(Yii::app()->user->getFlashes() as $key => $message) {
        echo '<div class="flash-' . $key . '">' . $message . "</div>\n";
    }
?>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
	<div class="div_actions">
		<?php echo CHtml::link('Add New Group', $this->createUrl('userGroup/create')); ?>		
	</div>	
</div>

<!-- Grid view -->
<div class="span-18 last">
    <?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$dataProvider,
	  'selectableRows'=>null,
      'columns'=>array(		  
          'id',                       
		  array(            
              'name' => 'name',
			  'type' => 'raw',
			  'value' => 'CHtml::link($data->name, \'view/\'.$data->id)',	  			  			  
          ),
		  array(                          			  
			  'name'=>'status',
			  'value'=>'Lookup::item(\'UserGroupStatus\', $data->status)',			  
          ),
		  'description',       
      ),
 )); 
  
 ?>
</div>