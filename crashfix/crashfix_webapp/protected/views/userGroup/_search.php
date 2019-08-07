<div class="wide form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'action'=>Yii::app()->createUrl($this->route),
	'method'=>'get',
)); ?>

	<div class="row">
		<?php echo $form->label($model,'id'); ?>
		<?php echo $form->textField($model,'id'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'name'); ?>
		<?php echo $form->textField($model,'name',array('size'=>32,'maxlength'=>32)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_browse_crash_reports'); ?>
		<?php echo $form->textField($model,'can_browse_crash_reports'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_browse_bugs'); ?>
		<?php echo $form->textField($model,'can_browse_bugs'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_browse_debug_info'); ?>
		<?php echo $form->textField($model,'can_browse_debug_info'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_browse_sys_info'); ?>
		<?php echo $form->textField($model,'can_browse_sys_info'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_manage_crash_reports'); ?>
		<?php echo $form->textField($model,'can_manage_crash_reports'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_manage_bugs'); ?>
		<?php echo $form->textField($model,'can_manage_bugs'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_manage_debug_info'); ?>
		<?php echo $form->textField($model,'can_manage_debug_info'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_manage_users'); ?>
		<?php echo $form->textField($model,'can_manage_users'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'can_manage_quotas'); ?>
		<?php echo $form->textField($model,'can_manage_quotas'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton('Search'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- search-form -->