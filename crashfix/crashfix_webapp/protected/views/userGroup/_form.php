<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'user-group-form',
	'enableAjaxValidation'=>false,
)); ?>

	<p class="note">Fields with <span class="required">*</span> are required.</p>

	<?php echo $form->errorSummary($model); ?>

	<div class="row prepend-top">
		<h4>General:</h4>
	</div>
	
	<?php echo $form->hiddenField($model, 'id'); ?>
	
	<div class="row">
		<?php echo $form->labelEx($model,'name'); ?>
		<?php echo $form->textField($model,'name',array('size'=>32,'maxlength'=>32)); ?>
		<?php echo $form->error($model,'name'); ?>
	</div>
	
	<div class="row">
		<?php echo $form->labelEx($model, 'description'); ?>
		<?php echo $form->textField($model, 'description', array('size'=>64,'maxlength'=>128)); ?>
		<?php echo $form->error($model, 'description'); ?>
	</div>	

	<div class="row prepend-top">
		<h4>Permissions:</h4>
	</div>
		
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'pperm_browse_crash_reports'); ?>		
		<?php echo $form->labelEx($model, 'pperm_browse_crash_reports'); ?>
		<?php echo $form->error($model, 'pperm_browse_crash_reports'); ?>
	</div>	
	
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'pperm_browse_bugs'); ?>		
		<?php echo $form->labelEx($model, 'pperm_browse_bugs'); ?>
		<?php echo $form->error($model, 'pperm_browse_bugs'); ?>
	</div>	
	
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'pperm_browse_debug_info'); ?>		
		<?php echo $form->labelEx($model, 'pperm_browse_debug_info'); ?>
		<?php echo $form->error($model, 'pperm_browse_debug_info'); ?>
	</div>	
	
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'pperm_manage_crash_reports'); ?>		
		<?php echo $form->labelEx($model, 'pperm_manage_crash_reports'); ?>
		<?php echo $form->error($model, 'pperm_manage_crash_reports'); ?>
	</div>	
	
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'pperm_manage_bugs'); ?>		
		<?php echo $form->labelEx($model, 'pperm_manage_bugs'); ?>
		<?php echo $form->error($model, 'pperm_manage_bugs'); ?>
	</div>	
	
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'pperm_manage_debug_info'); ?>		
		<?php echo $form->labelEx($model, 'pperm_manage_debug_info'); ?>
		<?php echo $form->error($model, 'pperm_manage_debug_info'); ?>
	</div>	
	
	<div class="row rememberMe">		
		<?php echo $form->checkBox($model, 'gperm_access_admin_panel'); ?>		
		<?php echo $form->labelEx($model, 'gperm_access_admin_panel'); ?>
		<?php echo $form->error($model, 'gperm_access_admin_panel'); ?>
	</div>
	
	<div class="row prepend-top">
		<h4>UI Preferences:</h4>
	</div>
	
	<div class="row">
		<?php echo $form->labelEx($model, 'default_sidebar_tab'); ?>
		<?php echo $form->dropDownList($model, 'default_sidebar_tab', $model->getSidebarItemList()); ?>
		<?php echo $form->error($model, 'default_sidebar_tab'); ?>
	</div>	
	
	<div class="row">
		<?php echo $form->labelEx($model, 'default_bug_status_filter'); ?>
		<?php echo $form->dropDownList($model, 'default_bug_status_filter', $model->getBugStatusFilterItemList()); ?>
		<?php echo $form->error($model, 'default_bug_status_filter'); ?>
	</div>	
	
	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord?'Create':'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->