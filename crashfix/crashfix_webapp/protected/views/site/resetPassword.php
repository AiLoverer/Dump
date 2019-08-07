<?php
$this->pageTitle=Yii::app()->name . ' - Reset Password';
?>

<div class="prepend-10 span-15 last prepend-top">
	<h3>Enter New Password</h3>
	
	<p>Please enter new password:</p>
	
	<div class="form">
	<?php $form=$this->beginWidget('CActiveForm', array(
		'id'=>'recover-password-form',				
	)); ?>

		<?php echo $form->errorSummary($model); ?>
	
		<p class="note">Fields with <span class="required">*</span> are required.</p>

		<div class="row">
			<?php echo $form->labelEx($model,'password'); ?>
			<?php echo $form->passwordField($model,'password'); ?>
			<?php echo $form->error($model,'password'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'passwordConfirmation'); ?>
			<?php echo $form->passwordField($model,'passwordConfirmation'); ?>
			<?php echo $form->error($model,'passwordConfirmation'); ?>			
		</div>

		<div class="row buttons">
			<?php echo CHtml::submitButton('Submit'); ?>
		</div>

	<?php $this->endWidget(); ?>
	</div><!-- form -->	
</div>
