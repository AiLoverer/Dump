<?php

if(Yii::app()->user->checkAccess('gperm_access_admin_panel'))
{
	$this->breadcrumbs=array(
		'Administer', 
		'Users'=>array('user/index'),
		CHtml::encode($model->username));
}
else
{
	$this->breadcrumbs=array(		
		'Users',
		CHtml::encode($model->username));
}

?>

<!-- Flash messages -->
<div class="span-18 last ">
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
		
			<?php 
				if(Yii::app()->user->getId()==$model->id)
				{
					echo CHtml::linkButton('Change Password', 
						array(
							'submit'=>Yii::app()->createUrl('site/resetPassword'),						
							'confirm'=>"Are you sure you want to change your password?")
						);					
				}
			?>	
		
			<?php echo CHtml::hiddenField('id', $model->id); ?>			
		
			<?php
			
				if(Yii::app()->user->checkAccess('gperm_access_admin_panel'))
				{
			
					echo CHtml::linkButton('Update User', 
								array('submit'=>Yii::app()->createUrl('user/update'))
								);				

					if(!$model->isStandard())
					{

						if($model->getEffectiveStatus()!=User::STATUS_DISABLED) 
						{
							echo CHtml::linkButton('Disable User', 
								array(
									'submit'=>Yii::app()->createUrl('user/retire'),						
									'confirm'=>"Are you sure you want to set status of user ".$model->username." to 'Retired'?")
								);
						}
						else
						{
							echo CHtml::linkButton('Enable User', 
								array(
									'submit'=>Yii::app()->createUrl('user/resurrect'),						
									'confirm'=>"Are you sure you want to set status of user ".$model->username." to 'Active'?")
								);
						}

						echo CHtml::linkButton('Delete User', 
							array(
								'submit'=>Yii::app()->createUrl('user/delete'),						
								'confirm'=>"Are you sure you want to permanently delete user ".$model->username."?")
							); 
					}
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
		'id',
		'username',	
        array(  
			'label'=>'User Group',
            'type'=>'raw',
            'value'=>Yii::app()->user->checkAccess('gperm_access_admin_panel')?CHtml::link(CHtml::encode($model->group->name), array('userGroup/view', 'id'=>$model->usergroup)):CHtml::encode($model->group->name),
        ),
		array(  
            'label'=>'Status',
            'type'=>'text',
            'value'=>$model->getEffectiveStatusStr(),
        ),
		'email',
	),
)); ?>
</div>

<!-- Permissions -->
<div class="prepend-top span-18 last">
	<h4>Project Roles:</h4>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
	<div class="div_actions">
		
			<?php echo CHtml::form(); ?>		
		
			<?php echo CHtml::hiddenField('id', $model->id); ?>			
		
			<?php	
            if(Yii::app()->user->checkAccess('gperm_access_admin_panel'))
            {
					echo CHtml::linkButton('Add Project Role(s)', 
								array('submit'=>Yii::app()->createUrl('user/addProjectRole'))
								);				
				
				echo CHtml::link("Delete Selected Roles", "#", array('id'=>'delete_selected'));
            }
			?>	
		
			<?php echo CHtml::endForm(); ?>		
	</div>	
</div>

<?php echo CHtml::beginForm(array('user/deleteProjectRoles'), 'post', array('name'=>'del_form')); ?>

<?php echo CHtml::hiddenField('user_id', $model->id); ?>

<!-- Grid view -->
<div class="span-18 last">
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$userProjectAccess->search(),
	  'selectableRows'=>null,
      'columns'=>array(		            
		  array(            
              'class'=>'CCheckBoxColumn',			  			  
			  'id'=>'DeleteRows',
			  'selectableRows'=>2, // allow multiple rows to select
          ),
          array(
			  'header'=>'Project',			  
              'name'=>'project_id',
			  'type' => 'raw',
              'value'=>'Yii::app()->user->checkAccess(\'gperm_access_admin_panel\')?CHtml::link($data->project->name, array(\'project/view/\', \'id\'=>$data->project_id)):CHtml::encode($data->project->name)',	  			  			  
          ),
		  array(
			  'header'=>'Project Role',			  
              'name'=>'usergroup_id',
			  'type' => 'raw',
              'value'=>'Yii::app()->user->checkAccess(\'gperm_access_admin_panel\')?CHtml::link($data->usergroup->name, array(\'userGroup/view/\', \'id\'=>$data->usergroup_id)):CHtml::encode($data->usergroup->name)',	  			  			  
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