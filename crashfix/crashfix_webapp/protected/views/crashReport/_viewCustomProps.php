<div class="span-18 last">

<!-- Grid view -->
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$model->searchCustomProps(),
	  'selectableRows'=>null,
      'columns'=>array(
		  'name',
		  'value'
      ),
 )); 
  
 ?>

</div>