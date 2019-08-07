<?php
$this->pageTitle=Yii::app()->name . ' - Recover Password';
?>

<div class="prepend-10 span-15 last prepend-top">
	<h3>Recover Lost Password</h3>

	<?php if(Yii::app()->user->hasFlash('recoverPassword')): ?>

	<div class="flash-success">
		<?php echo Yii::app()->user->getFlash('recoverPassword'); ?>
	</div>

	<?php else: ?>
	
	<p>Please enter your login and E-mail address:</p>
	
	<div class="form">
	<?php $form=$this->beginWidget('CActiveForm', array(
		'id'=>'recover-password-form',				
	)); ?>

		<?php echo $form->errorSummary($model); ?>
	
		<p class="note">Fields with <span class="required">*</span> are required.</p>

		<div class="row">
			<?php echo $form->labelEx($model,'username'); ?>
			<?php echo $form->textField($model,'username'); ?>
			<?php echo $form->error($model,'username'); ?>
		</div>

		<div class="row">
			<?php echo $form->labelEx($model,'email'); ?>
			<?php echo $form->textField($model,'email'); ?>
			<?php echo $form->error($model,'email'); ?>			
		</div>

		<?php if(CCaptcha::checkRequirements()): ?>
			<div class="row">
				<?php echo $form->labelEx($model,'verifyCode'); ?>
				<div>
				<?php $this->widget('CCaptcha'); ?>
				<?php echo $form->textField($model,'verifyCode'); ?>
				</div>
				<div class="hint">Please enter the letters as they are shown in the image above.
				<br/>Letters are not case-sensitive.</div>
				<?php echo $form->error($model,'verifyCode'); ?>
			</div>
		<?php endif; ?>
		
		<div class="row buttons">
			<?php echo CHtml::submitButton('Submit'); ?>
		</div>

	<?php $this->endWidget(); ?>
	</div><!-- form -->
	
	<?php endif; ?>
</div>
