<?php
$this->breadcrumbs=array(
	'Administer', 
	'Projects'=>array('index'),
	$model->name,
);

?>

<!-- Flash messages -->
<div class="span-18 last">
<?php
foreach (Yii::app()->user->getFlashes() as $key => $message) {
    echo '<div class="flash-' . $key . '">' . $message . "</div>\n";
}
?>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
	<div class="div_actions">
		
			<?php echo CHtml::form(); ?>		
		
			<?php echo CHtml::hiddenField('id', $model->id); ?>			
		
			<?php
				
				echo CHtml::linkButton('Update Project', 
							array('submit'=>Yii::app()->createUrl('project/update', array('id'=>$model->id)))
							);			
				
				if($model->status==Project::STATUS_ACTIVE)
				{
					echo CHtml::linkButton('Disable Project', 
							array('submit'=>Yii::app()->createUrl('project/disable'))
							);			
				}
				else
				{
					echo CHtml::linkButton('Enable Project', 
							array('submit'=>Yii::app()->createUrl('project/enable'))
							);			
				}
				
				echo CHtml::linkButton('Delete Project', 
							array('submit'=>Yii::app()->createUrl('project/delete'),
								'confirm'=>"Are you sure you want to permanently delete this project?")						
							);			
			?>	
		
			<?php echo CHtml::endForm(); ?>		
	</div>	
</div>

<div class="span-18 last">
<div class="subheader">Project Info:</div>

<!-- Detail View -->
<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		'name',
		'description',
		array(  
            'name'=>'status',
            'type'=>'text',
            'value'=>Lookup::item('ProjectStatus', $model->status),		
        ),		
	),
)); ?>
</div>

<div class="span-18 last">
<div class="subheader">Project Quotas:</div>

<!-- Detail View -->
<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(				
		'crash_reports_per_group_quota',
		'crash_report_files_disc_quota',
		'bug_attachment_files_disc_quota',
		'debug_info_files_disc_quota'
	),
)); ?>
</div>

<div class="span-18 last">
<div class="subheader">Misc Options:</div>

<!-- Detail View -->
<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(	
        array(  
            'name'=>'require_exact_build_age',		
            'type'=>'text',
            'value'=>$model->require_exact_build_age!=0?'Yes (recommended)':'No'),		
        ),		
	)
); ?>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
<p></p>

<h4>Users Participating in this Project:</h4>

    <div class="div_actions">
		
			<?php echo CHtml::form(); ?>		
		
			<?php echo CHtml::hiddenField('id', $model->id); ?>			
		
			<?php
			
                echo CHtml::linkButton('Add User(s)', 
                            array('submit'=>Yii::app()->createUrl('project/addUser'))
                            );			
								
				echo CHtml::link("Remove Selected Users", "#", array('id'=>'delete_selected'));
			?>	
		
			<?php echo CHtml::endForm(); ?>		
	</div>	
</div>

<?php echo CHtml::beginForm(array('project/deleteUser'), 'post', array('name'=>'del_form')); ?>

<?php echo CHtml::hiddenField('project_id', $model->id); ?>

<!-- Grid view -->
<div class="span-18 last">
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$dataProvider,
	  'selectableRows'=>null,
      'columns'=>array(		            
		  array(                          
              'class'=>'CCheckBoxColumn',			  			  
			  'id'=>'DeleteRows',
			  'selectableRows'=>2, // allow multiple rows to select
          ),
		  array(     
			  'name'=>'user_id',
			  'type' => 'raw',			  
			  'value' => 'CHtml::link($data->user->username, array(\'user/view\', \'id\'=>$data->user_id))',	  			  			  
          ),	
		  array(     
			  'name'=>'usergroup_id',
			  'type' => 'raw',			  
			  'value' => 'CHtml::link($data->usergroup->name, array(\'userGroup/view\', \'id\'=>$data->usergroup_id))',	  			  			  
          ),
      ),
 )); 
  
 ?>

</div>

<?php echo CHtml::endForm(); ?>

<?php 
 $script = <<<SCRIPT
$(":checkbox").live('click', function(e)
{		
	var totalSelected = 0;
	$("input[name='DeleteRows\[\]']").each(function() {if($(this).attr('checked')) totalSelected++;});
	
	if(totalSelected==0)
		$("#delete_selected").css('visibility', 'hidden');
	else
		$("#delete_selected").css('visibility', 'visible');
	
});

$("#delete_selected").bind('click', function(e)
{	
	$("[name=del_form]").submit();
});

SCRIPT;
 
 Yii::app()->getClientScript()->registerScript("DebugInfo", $script, CClientScript::POS_READY); ?>