<?php
$this->pageTitle=Yii::app()->name . ' - Browse Crash Report Collections';
$this->breadcrumbs=array(
	'Crash Report Collections',
	);

?>

<?php if(count(Yii::app()->user->getMyProjects())==0): ?>

You have no projects assigned.

<?php else: ?>

<!-- Project Selection Form -->
<div class="span-27 last" id="div_proj_selection">	
	<?php echo CHtml::beginForm(array('site/setCurProject'), 'get', array('id'=>'proj_form')); ?>
	<div class="span-27 last">
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

<!-- Simple Search Form -->
<div class="span-27 last" id="div_simple_search">	
	<?php echo CHtml::beginForm(array('crashGroup/index'), 'get', array('id'=>'proj_form')); ?>
	<div class="span-27 last"><p id="stat_filter">Search collections by title:</p></div>
	<div class="span-7"><div id="dropdown_search">
		<?php 
			$statuses = array(												
				'all'=>'All collections', 
				'open'=>'Nonempty c. with bug(s) unassigned or open',								
			);			
			echo CHtml::dropDownList('status', array('selected'=>isset($model->bugStatusFilter)?CHtml::encode($model->bugStatusFilter):"open"), $statuses); 
		?></div>
	</div>
	<div class="span-18">
		<?php echo CHtml::textField('q', isset($model->filter)?CHtml::encode($model->filter):"", array('id'=>'text_filter')); ?>		
	</div>
	<div class="span-2 last">
		<?php echo CHtml::submitButton('Search', array('id'=>'btn_filter')); ?>
	</div>	
	<?php echo CHtml::endForm(); ?>
</div>

<div class="span-27 last" >
<?php echo CHtml::beginForm(array('crashGroup/deleteMultiple'), 'post', 
		array('name'=>'del_form')); 
?>
	
<!-- Actions Toolbar -->
<div class="span-27 last">
	<div class="div_actions">				
		<?php 
			if(Yii::app()->user->checkAccess('pperm_manage_crash_reports',  
                    array('project_id'=>Yii::app()->user->getCurProjectId())))
				echo CHtml::linkButton("Delete Selected", 
						array(
							'id'=>'delete_selected', 
							'form'=>'del_form', 
							'confirm'=>"Are you sure you want to permanently delete selected collection(s) with all crash reports they contain?"
						)
				); 
		?>	
	</div>	
</div>

<!-- Grid view -->
<div class="span-27 last">
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$dataProvider,
	  'selectableRows'=>null,
      'columns'=>array(		  
		  array(            
              'class'=>'CCheckBoxColumn',			  			  
			  'id'=>'DeleteRows',
			  'selectableRows'=>2, // allow multiple rows to select
          ),
		  array(            
              'name' => 'id',
			  'type' => 'text',
			  'value' => '$data->id',
			  'cssClassExpression' => '"column-right-align"',			  
          ),		  
		  array(                          			  
			  'name'=>'title',
			  'type' => 'raw',
			  'value'=>'CHtml::link(MiscHelpers::addEllipsis($data->title, 120), \'view/\'.$data->id)',			  
          ),
		  'crashReportCount',
          array(                          			  
			  'name'=>'distinctIPs',
			  'header'=>'Distinct IPs',
			  'type'=>'text',
			  'value'=>'$data->getDistinctIPs()',			  
          ),
		  array(                          			  
			  'name'=>'bugCount',
			  'header'=>'Bug(s)',
			  'type'=>'raw',
			  'value'=>'$data->formatBugListStr()',			  
          ),
          array(            // display 'dateuploaded' using an expression
              'name'=>'created',
              'value'=>'date("d/m/y H:i", $data->created)',
          ),          		  
      ),
 )); 
  
 ?>

<?php echo CHtml::endForm(); ?>
</div>

<?php 
 $script = <<<SCRIPT

$(":checkbox").live('click', function(e)
{	
	var totalSelected = 0;
	$("input[name='DeleteRows\[\]']").each(function() {if($(this).attr('checked')) totalSelected++;});
	
	if(totalSelected==0)
		$("#delete_selected").css('visibility', 'hidden');
	else
		$("#delete_selected").css('visibility', 'visible');
	
});
   
$("#proj, #ver").bind('change', function(e)
{	
	$("#proj_form").submit();
});

SCRIPT;
 
 Yii::app()->getClientScript()->registerScript(
		 "CrashGroup", 
		 $script, 
		 CClientScript::POS_READY); 
 ?>

<?php endif; ?>