<?php
$this->breadcrumbs=array(
	'Bugs'=>array('index'),
	'View Bug #'.$model->id,
);

?>

<!-- Actions Toolbar -->
<div class="span-27 last">
	<div class="div_actions">
		
			<?php echo CHtml::form(); ?>		
		
			<?php echo CHtml::hiddenField('id', $model->id); ?>			
		
			<?php
			
				if(Yii::app()->user->checkAccess('pperm_manage_bugs', array('project_id'=>Yii::app()->user->getCurProjectId())))
				{
					echo CHtml::link('Comment/Change', '#', array('id'=>'link-make-changes')); 
		
					echo CHtml::linkButton('Delete Bug', 
							array(
								'submit'=>Yii::app()->createUrl('bug/delete'),						
								'confirm'=>"Are you sure you want to permanently delete the bug #".$model->id."?")
							); 					
				}
			?>	
		
			<?php echo CHtml::endForm(); ?>		
	</div>	
</div>

<div class="span-27 last" id="<?php if($model->status<=Bug::STATUS_OPEN_MAX) echo 'bug-header'; else echo 'bug-header-closed'; ?>">

<div class="span-27 last">
	<div id="bug-summary">		
		<?php echo CHtml::encode($model->summary); ?>
	</div>
</div>
<div class="span-27 last">
	<div id="bug-subheader">
		<div>
			Reported by <b><?php echo CHtml::encode($model->reporter->username); ?></b>
			on <?php echo date('j F Y, G:i', $model->date_created); ?>		
		</div>
		<div>
			Status: <?php echo CHtml::encode(Lookup::item('BugStatus', $model->status)); ?>
		</div>
		
		<div>
			Priority: <?php echo CHtml::encode(Lookup::item('BugPriority', $model->priority)); ?>
		</div>
		
		<div>
			Reproducibility: <?php echo CHtml::encode(Lookup::item('BugReproducability', $model->reproducability)); ?>
		</div>
		<div>
			Owner: <?php echo $model->assigned_to<0?'nobody':(isset($model->owner)?CHtml::encode($model->owner->username):'user not found'); ?>
		</div>
	</div>	
</div>
<div class="span-27 last">
	<div id="bug-group-associations">		
		<?php
			$bugCrashGroups = $model->crashGroups;			
			if($bugCrashGroups!=null)
			{
				echo 'Collection ID(s) associated with this bug: ';
				foreach($bugCrashGroups as $mapping)
				{
					echo CHtml::link('#'.$mapping->crashgroup_id, array('crashGroup/view', 'id'=>$mapping->crashgroup_id));					
					echo '&nbsp;';
				}
			}
		?>
	</div>
	<div id="bug-report-associations">
		<?php
			$bugCrashReports = $model->crashReports;
			if($bugCrashReports!=null)
			{
				echo 'Individual crash report ID(s) associated with this bug: ';
				foreach($bugCrashReports as $mapping)
				{
					echo CHtml::link('#'.$mapping->crashreport_id, array('crashReport/view', 'id'=>$mapping->crashreport_id));					
					echo '&nbsp;';
				}
			}
		?>
	</div>
</div>
<div class="span-27 last">
	<div id="bug-description">
		<?php echo CHtml::encode(str_replace("<br>", "\n", $model->description));  ?>				
	</div>

    <?php
	
	$changeNum = 0;
	foreach($model->bugchanges as $bugChange)
	{
		if($changeNum==0){ $changeNum++; continue;}
		
		$this->renderPartial('_bugChange', array('changeNum'=>$changeNum++, 'model'=>$bugChange));				
	}
    ?>

</div>

 </div>

<a id="bug-change-form"></a>
<div class="span-27 last" id="div-changes" style="display:<?php echo $model->hasErrors()?'display':'none'?>">
	<div id="bug-change-subheader">Add a comment and make changes:</div>

	<div class="form wide">

		<?php $form=$this->beginWidget('CActiveForm', array(
			'id'=>'bug-form',
			'enableAjaxValidation'=>false,
			'htmlOptions'=>array('enctype' => 'multipart/form-data'),
		)); ?>

		<?php echo $form->errorSummary($model); ?>

		<div class="row">
			<?php echo $form->labelEx($model,'comment'); ?>
			<?php echo $form->textArea($model, 'comment', array('maxlength'=>1024, 'cols'=>68, 'rows'=>7)); ?>
			<?php echo $form->error($model,'comment'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'fileAttachment', array('label'=>'Attach a file')); ?>
			<?php echo $form->fileField($model,'fileAttachment'); ?>
			<?php echo $form->error($model,'fileAttachment'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'crashgroups', array('label'=>'Collection(s)')); ?>
			<?php echo $form->textField($model,'crashgroups',array('size'=>60,'maxlength'=>256)); ?>
			<?php echo $form->error($model,'crashgroups'); ?>
		</div>

		<div class="row">
			<?php echo $form->label($model,'crashreports', array('label'=>'Report(s)')); ?>
			<?php echo $form->textField($model,'crashreports',array('size'=>60,'maxlength'=>256)); ?>
			<?php echo $form->error($model,'crashreports'); ?>
		</div>

		<div class="row">
			<?php echo $form->label($model,'summary'); ?>
			<?php echo $form->textField($model,'summary',array('size'=>60,'maxlength'=>256)); ?>
			<?php echo $form->error($model,'summary'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'status'); ?>
			<?php 
				$items = Lookup::items('BugStatus', Bug::STATUS_OPEN_MAX);			
				echo $form->dropDownList($model, 'status', $items, array('id'=>'dropdown_status')); 
			?>				
			<?php echo $form->error($model,'status'); ?>		
		</div>

		<div class="row" id="div_merged_into" style="display:<?php echo $model->status==Bug::STATUS_DUPLICATE?'': 'none'; ?>">
			<?php echo $form->labelEx($model,'merged_into', array('label'=>'Merge into')); ?>
			<?php echo $form->textField($model,'merged_into',array('size'=>5,'maxlength'=>5)); ?>
			<?php echo $form->error($model,'merged_into'); ?>
		</div>		

		<div class="row">
			<?php echo $form->labelEx($model,'priority'); ?>
			<?php 
				$items = Lookup::items('BugPriority');			
				echo $form->dropDownList($model, 'priority', $items); 
			?>
			<?php echo $form->error($model,'priority'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'reproducability'); ?>
			<?php 
				$items = Lookup::items('BugReproducability');			
				echo $form->dropDownList($model, 'reproducability', $items); 
			?>
			<?php echo $form->error($model,'reproducability'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'assigned_to'); ?>
			<?php 
				$curProject = Yii::app()->user->getCurProject();
				$projectUsers = $curProject->users;
				$items = array();
				$items[-1] = '<nobody>';
				foreach($projectUsers as $user)
					$items[$user->user_id] = $user->user->username;

				echo $form->dropDownList($model,'assigned_to', $items); 
			?>
			<?php echo $form->error($model,'assigned_to'); ?>
		</div>

		<div class="row buttons">
			<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save Changes'); ?>
		</div>

	<?php $this->endWidget(); ?>

	</div><!-- form -->

</div>

<?php 
 $script = <<<SCRIPT

$("#link-make-changes").bind('click', function(e)
{		
	$("#div-link-changes").fadeOut("fast");			
	$("#div-changes").fadeIn("fast");			
	$('html,body').animate({scrollTop: $("#div-changes").offset().top});
});

$('#dropdown_status').change(function() {
	if($(this).val()==103)
		$("#div_merged_into").show();
	else
		$("#div_merged_into").hide();
});

SCRIPT;
 
 Yii::app()->getClientScript()->registerScript(
		 "BugView", 
		 $script, 
		 CClientScript::POS_READY); 
 ?>


