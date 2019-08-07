<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'project-form',
	'enableAjaxValidation'=>false,
)); ?>

	<p class="note">Fields with <span class="required">*</span> are required.</p>

	<?php echo $form->errorSummary($model); ?>

	<div class="subheader">General Info:</div>
	
	<div class="row idented">
		<?php echo $form->labelEx($model,'name'); ?>
		<?php echo $form->textField($model,'name',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'name'); ?>
	</div>
	
	<div class="row idented">
		<?php echo $form->labelEx($model, 'description'); ?>
		<?php echo $form->textField($model,'description',array('size'=>60,'maxlength'=>128)); ?>
		<?php echo $form->error($model,'description'); ?>
	</div>
	
	<div class="subheader">Project Quotas:</div>

	<div class="row idented">
		<?php echo $form->labelEx($model, 'crash_reports_per_group_quota'); ?>
		<?php echo $form->textField($model,'crash_reports_per_group_quota',array('size'=>10,'maxlength'=>10)); ?>
		<?php echo $form->error($model,'crash_reports_per_group_quota'); ?>
	</div>
	
	<div class="row idented">
		<?php echo $form->labelEx($model, 'crash_report_files_disc_quota'); ?>
		<?php echo $form->textField($model,'crash_report_files_disc_quota',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'crash_report_files_disc_quota'); ?>		
	</div>
        
        <div class="row idented">
		<?php echo $form->labelEx($model, 'bug_attachment_files_disc_quota'); ?>
		<?php echo $form->textField($model,'bug_attachment_files_disc_quota', array('size'=>32, 'maxlength'=>32)); ?>
		<?php echo $form->error($model,'bug_attachment_files_disc_quota'); ?>		
	</div>
	
	<div class="row idented">
		<?php echo $form->labelEx($model, 'debug_info_files_disc_quota'); ?>
		<?php echo $form->textField($model,'debug_info_files_disc_quota',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'debug_info_files_disc_quota'); ?>		
	</div>

	<div class="subheader">Misc Options:</div>
    
    <div class="row idented">
		<?php echo $form->labelEx($model, 'require_exact_build_age'); ?>
		<?php echo $form->checkBox($model,'require_exact_build_age'); ?>
		<?php echo $form->error($model,'require_exact_build_age'); ?>		
	</div>
	
	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->