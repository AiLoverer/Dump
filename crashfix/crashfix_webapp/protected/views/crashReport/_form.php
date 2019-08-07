<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'crash-report-form',
	'enableAjaxValidation'=>false,
)); ?>

	<p class="note">Fields with <span class="required">*</span> are required.</p>

	<?php echo $form->errorSummary($model); ?>

	<div class="row">
		<?php echo $form->labelEx($model,'received'); ?>
		<?php echo $form->textField($model,'received',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'received'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'md5'); ?>
		<?php echo $form->textField($model,'md5',array('size'=>16,'maxlength'=>16)); ?>
		<?php echo $form->error($model,'md5'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'crashguid'); ?>
		<?php echo $form->textField($model,'crashguid',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'crashguid'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'appname'); ?>
		<?php echo $form->textField($model,'appname',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'appname'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'appversion'); ?>
		<?php echo $form->textField($model,'appversion',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'appversion'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'emailfrom'); ?>
		<?php echo $form->textField($model,'emailfrom',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'emailfrom'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'description'); ?>
		<?php echo $form->textField($model,'description',array('size'=>60,'maxlength'=>128)); ?>
		<?php echo $form->error($model,'description'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->