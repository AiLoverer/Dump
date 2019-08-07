<?php
$this->breadcrumbs=array(
	'Administer', 
	'Users',

);

?>

<!-- Simple Search Form -->
<div class="span-18 last" id="div_simple_search">
	<?php echo CHtml::beginForm(Yii::app()->createUrl($this->route), 'post'); ?>
	<div class="span-18 last"><p id="stat_filter">Search by user name/E-mail address:</p></div>
	<div class="span-4"><div id="dropdown_search">
		<?php 
			$user_statuses = array(												
				 1=>'Active users',
				 2=>'Disabled users',
				-1=>'All users',
			);			
			echo CHtml::dropDownList('User[status]', array('selected'=>isset($model->status)?CHtml::encode($model->status):1), $user_statuses); 
		?></div>
	</div>
	<div class="span-11">
		<?php echo CHtml::textField('User[username]', isset($model->username) ? CHtml::encode($model->username):"", array('id'=>'text_filter')); ?>
	</div>
	<div class="span-2 last">
		<?php echo CHtml::submitButton('Search', array('id'=>'btn_filter')); ?>
	</div>
	<?php echo CHtml::endForm(); ?>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
	<div class="div_actions">
		<?php echo CHtml::link('Add New User', $this->createUrl('user/create')); ?>		
	</div>	
</div>

<!-- Grid view -->
<div class="span-18 last">
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$model->search(),
	  'selectableRows'=>null,
      'columns'=>array(		  
          'id',             
          array(            
              'name' => 'username',
			  'type' => 'raw',
			  'value' => 'CHtml::link($data->username, \'view/\'.$data->id)',	  			  			  
          ),		  
		  array(
			  'header'=>'Group',			  
              'name'=>'usergroup',
			  'type' => 'raw',
              'value'=>'CHtml::link($data->group->name, array(\'userGroup/view/\', \'id\'=>$data->usergroup))',	  			  			  
          ),
		  array(
			  'header'=>'Status',			  
              'name'=>'status',
              'value'=>'$data->getEffectiveStatusStr()',
          ),		  		  
		  'email',       
      ),
 )); 
  
 ?>
</div>
