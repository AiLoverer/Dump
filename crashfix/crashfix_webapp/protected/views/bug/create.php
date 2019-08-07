<?php
$this->breadcrumbs=array(
	'Bugs'=>array('index'),
	'Open New Bug',
);

?>

<div class="form span-14 last">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'bug-form',
	'enableAjaxValidation'=>false,
)); ?>

	<?php echo $form->errorSummary($model); ?>

	<div class="row">
		<?php echo $form->labelEx($model,'crashgroups'); ?>
		<?php echo $form->textField($model,'crashgroups',array('size'=>60,'maxlength'=>256)); ?>
		<?php echo $form->error($model,'crashgroups'); ?>
	</div>
	
	<div class="row">
		<?php echo $form->labelEx($model,'crashreports'); ?>
		<?php echo $form->textField($model,'crashreports',array('size'=>60,'maxlength'=>256)); ?>
		<?php echo $form->error($model,'crashreports'); ?>
	</div>

	
	<div class="row">
		<?php echo $form->labelEx($model,'summary'); ?>
		<?php echo $form->textField($model,'summary',array('size'=>60,'maxlength'=>256)); ?>
		<?php echo $form->error($model,'summary'); ?>
	</div>
		
	<div class="row">
		<?php echo $form->labelEx($model,'description'); ?>
		<?php echo $form->textArea($model, 'description', array('cols'=>48, 'rows'=>10)); ?>
		<?php echo $form->error($model,'description'); ?>
	</div>
	
	<div class="row">
		<?php echo $form->labelEx($model,'status'); ?>
		<?php 
			$items = Lookup::items('BugStatus', Bug::STATUS_OPEN_MAX);			
			echo $form->dropDownList($model, 'status', $items); 
		?>
		<?php echo $form->error($model,'status'); ?>
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
				$items[$user->id] = $user->user->username;
			
			echo $form->dropDownList($model,'assigned_to', $items); 
		?>
		<?php echo $form->error($model,'assigned_to'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->