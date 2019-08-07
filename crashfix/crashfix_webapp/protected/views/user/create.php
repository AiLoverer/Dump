<h3>Add New User</h3>

<div class="form wide">

	<?php $form=$this->beginWidget('CActiveForm', array(
		'id'=>'user-form',
		'enableAjaxValidation'=>false,
	)); ?>

	<p class="note">Fields with <span class="required">*</span> are required.</p>

	<?php echo $form->errorSummary($model); ?>

	<div class="row">
		<?php echo $form->labelEx($model,'username'); ?>
		<?php echo $form->textField($model,'username',array('size'=>60,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'username'); ?>
	</div>
		
	<div class="row">
	<?php echo $form->labelEx($model,'usergroup'); ?>
	<?php 		
		$models = UserGroup::model()->findAll(); 
        $list = CHtml::listData($models, 'id', 'name');
		echo CHtml::dropDownList('User[usergroup]', array('selected'=>$model->usergroup), $list); 
	?>
	</div>
	
	<div class="row">
		<?php echo $form->labelEx($model,'email'); ?>
		<?php echo $form->textField($model,'email',array('size'=>60,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'email'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->

