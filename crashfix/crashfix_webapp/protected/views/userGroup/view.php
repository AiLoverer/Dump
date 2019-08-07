<?php
$this->breadcrumbs=array(
	'User Groups'=>array('index'),
	$model->name,
);

?>

<div class="span-18 last ">
<!-- Flash messages -->
<?php	
    foreach(Yii::app()->user->getFlashes() as $key => $message) {
        echo '<div class="flash-' . $key . '">' . $message . "</div>\n";
    }
?>
</div>

<div class="span-18 last ">
	<h4>General Info:</h4>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
	<div class="div_actions">
		<?php echo CHtml::form(); ?>	
		<?php echo CHtml::hiddenField('id', $model->id); ?>	
		
		<?php
		if($model->canUpdate())
		{
			echo CHtml::linkButton('Update Group', 
							array(
								'submit'=>Yii::app()->createUrl('userGroup/update', array('id'=>$model->id)))													
							);	
		}

		if($model->status==UserGroup::STATUS_ACTIVE && $model->canDisable())
		{
			echo CHtml::linkButton('Disable Group', 
							array(
								'submit'=>Yii::app()->createUrl('userGroup/disable'),						
								'confirm'=>"Are you sure you want to disable this group?")
							);							
		}
		else if($model->status==UserGroup::STATUS_DISABLED)
		{
			echo CHtml::linkButton('Enable Group', 
							array(
								'submit'=>Yii::app()->createUrl('userGroup/enable'),						
								'confirm'=>"Are you sure you want to enable this group?")
							);				
		}
		
		if(!$model->isStandard())
		{
			echo CHtml::linkButton('Delete Group', 
							array(
								'submit'=>Yii::app()->createUrl('userGroup/delete'),						
								'confirm'=>"Are you sure you want to permanently delete this group?")
							);							
		}
		?>
		
		<?php echo CHtml::endForm(); ?>	
	</div>	
</div>

<!-- Detail View -->
<div class="span-18 last">
    <?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		'name',
		'description',
		array(  
            'label'=>'Status',			
            'type'=>'text',
            'value'=>Lookup::item('UserGroupStatus', $model->status),
        ),		
	),
)); ?>
</div>

<div class="span-9">

<div class="prepend-top span-9">

<h4>Global Permissions:</h4>

<!-- Detail View -->
<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		array(  
			'name'=>'gperm_access_admin_panel',            
            'type'=>'text',
            'value'=>$model->gperm_access_admin_panel!=0?'Yes':'No',
        ),				
	),
)); ?>

</div>

<div class="prepend-top span-9">
<h4>Project Permissions:</h4>

<!-- Detail View -->
<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		array(  
			'name'=>'pperm_browse_crash_reports',            
            'type'=>'text',
            'value'=>$model->pperm_browse_crash_reports!=0?'Yes':'No',
        ),		
		array(  
			'name'=>'pperm_browse_bugs',            
            'type'=>'text',
            'value'=>$model->pperm_browse_bugs!=0?'Yes':'No',
        ),
		array(  
			'name'=>'pperm_browse_debug_info',            
            'type'=>'text',
            'value'=>$model->pperm_browse_debug_info!=0?'Yes':'No',
        ),
		array(  
			'name'=>'pperm_manage_crash_reports',            
            'type'=>'text',
            'value'=>$model->pperm_manage_crash_reports!=0?'Yes':'No',
        ),
		array(  
			'name'=>'pperm_manage_bugs',            
            'type'=>'text',
            'value'=>$model->pperm_manage_bugs!=0?'Yes':'No',
        ),
		array(  
			'name'=>'pperm_manage_debug_info',            
            'type'=>'text',
            'value'=>$model->pperm_manage_debug_info!=0?'Yes':'No',
        ),				
	),
)); ?>

</div>
</div>

<div class="prepend-top span-9 last">

<h4>UI Preferences:</h4>

<!-- Detail View -->
<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		array(  
            'label'=>'Default Sidebar Tab',			
            'type'=>'text',
            'value'=>$model->getDefaultSidebarTabLabel(),
        ),
		array(  
            'label'=>'Default Bug Status Filter',			
            'type'=>'text',
            'value'=>$model->getDefaultBugStatusFilterLabel(),
        ),		
	),
)); ?>

</div>

<div class="span-18 prepend-top last">

<h4>Users Belonging this Group:</h4>

<!-- Grid view -->
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$dataProvider,
	  'selectableRows'=>null,
      'columns'=>array(		  
          'id',                       
		  array(            
              'name' => 'username',
			  'type' => 'raw',
			  'value' => 'CHtml::link($data->username, array(\'user/view\', \'id\'=>$data->id))',	  			  			  
          ),
		  array(                          			  
			  'name'=>'status',
			  'value'=>'Lookup::item(\'UserStatus\', $data->status)',			  
          ),
		  
      ),
 )); 
  
 ?>

</div>