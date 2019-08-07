<?php
$this->breadcrumbs=array(
	'Users'=>array('index'),
	CHtml::encode($model->username)=>array('view','id'=>$model->id),
	'Update',
);

$this->menu=array(
	array('label'=>'List User', 'url'=>array('index')),
	array('label'=>'Create User', 'url'=>array('create')),
	array('label'=>'View User', 'url'=>array('view', 'id'=>$model->id)),
	array('label'=>'Manage User', 'url'=>array('admin')),
);
?>

<div class="form wide">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'user-form',
	'enableAjaxValidation'=>false,
)); ?>

	<?php echo $form->errorSummary($model); ?>

	<?php echo CHtml::hiddenField('id', $model->id); ?>
	
	<div class="row">
		<?php echo $form->label($model,'username'); ?>
		<?php echo CHtml::encode($model->username); ?>		
	</div>
		
	<div class="row">
	<?php echo $form->label($model,'usergroup'); ?>
	<?php 
		if(!$model->isStandard())
		{
			$models = UserGroup::model()->findAll(); 
			$list = CHtml::listData($models, 'id', 'name');
			echo CHtml::dropDownList('User[usergroup]', array('selected'=>$model->usergroup), $list); 
		}
		else
		{
			echo CHtml::encode($model->group->name);
		}
	?>
	</div>
	
	<div class="row">
		<?php echo $form->labelEx($model,'email'); ?>
		<?php echo $form->textField($model,'email',array('size'=>60,'maxlength'=>128)); ?>
		<?php echo $form->error($model,'email'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton('Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->