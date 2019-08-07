<?php
$this->pageTitle=Yii::app()->name . ' - Browse Crash Reports';
$this->breadcrumbs=array(	
	'Crash Reports'=>array('crashReport/index'),
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
	if($form->submitted('upload', false) && !$form->model->hasErrors())
	{
		echo "<p>File <em>".$form->model->fileAttachment->getName()."</em> uploaded successfully.</p>";
		echo "<p>Upload another file?</p>";
	}
    ?>

    
    <div class="form">
        <?php echo $form; ?>
    </div>
    
</div>

<?php 
 $script = <<<SCRIPT

$("#proj, #ver").bind('change', function(e)
{	
	$("#proj_form").submit();
});

SCRIPT;
 
 Yii::app()->getClientScript()->registerScript(
		 "uploadFile", 
		 $script, 
		 CClientScript::POS_READY); 
 ?>