<?php

$this->breadcrumbs=array(
	'Debug Info Files'=>array('debugInfo/index'),
	'Upload New File'
);

?>

<!-- Project Selection Form -->
<div class="span-26 last" id="div_proj_selection">	
	<?php echo CHtml::beginForm(array('site/setCurProject'), 'get', array('id'=>'proj_form')); ?>
	<div class="span-18 last">
		Current Project:
		<?php 		
			$models = Yii::app()->user->getMyProjects();
			$projects = CHtml::listData($models, 'id', 'name');			
			echo CHtml::dropDownList('proj', array('selected'=>Yii::app()->user->getCurProjectId()), $projects); 			
		?>					
		Version:
		<?php 		
			$selVer = -1;
			$versions = Yii::app()->user->getCurProjectVersions($selVer);			
			echo CHtml::dropDownList('ver', array('selected'=>$selVer), $versions); 
		?>		
	</div>
	<?php echo CHtml::endForm(); ?>		
</div>

<div class="span-18 prepend-top last">
<?php 
	if($submitted && !$model->hasErrors())
	{
		echo "<p>File <em>".$model->fileAttachment->getName()."</em> uploaded successfully.</p>";
		echo "<p>Upload another file?</p>";
	}
		
?>
 
<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'crash-group-form',
	'enableAjaxValidation'=>false,
	'htmlOptions'=>array('enctype'=>'multipart/form-data')
)); ?>

	<?php echo $form->errorSummary($model); ?>

	<?php echo $form->hiddenField($model, 'guid'); ?>
	
	<div class="row">
		<?php echo $form->label($model, 'fileAttachment'); ?>
		<?php echo $form->fileField($model,'fileAttachment'); ?>
		<?php echo $form->error($model, 'fileAttachment'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton('Upload'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->

</div>