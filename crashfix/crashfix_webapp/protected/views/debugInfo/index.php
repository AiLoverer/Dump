<?php
$this->pageTitle=Yii::app()->name . ' - Browse Debug Info';
$this->breadcrumbs=array(
	'Debug Info Files',
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
<div class="span-27 last" style="display:<?php echo !$model->isAdvancedSearch?'inherit':'none'?>" id="div_simple_search">
	<?php echo CHtml::beginForm(array('debugInfo/index'), 'get'); ?>
	<div class="span-27 last"><p id="stat_filter">Search by file name/GUID or use <a href="#" id="link_advanced_search">advanced search</a>:</p></div>
	<div class="span-25">
		<?php echo CHtml::textField('q', isset($model->filter) ? CHtml::encode($model->filter):"", array('id'=>'text_filter')); ?>
	</div>
	<div class="span-2 last">
		<?php echo CHtml::submitButton('Search', array('id'=>'btn_filter')); ?>
	</div>
	<?php echo CHtml::endForm(); ?>
</div>

<!-- Advanced Search Form -->
<div class="span-27 last" id="div_advanced_search" style="display:<?php echo $model->isAdvancedSearch?'inherit':'none'?>">	
	<div class="wide form" id="form_advanced_search">
	<?php echo CHtml::beginForm(array('debugInfo/index'), 'post'); ?>
	
	<h4>Advanced Search:</h4>
	<?php echo CHtml::errorSummary($model); ?>
	<div class="row">
	<?php echo CHtml::activeLabel($dataProvider->model,'filename'); ?>
	<?php echo CHtml::activeTextField($dataProvider->model,'filename', array('value'=>$model->filename)); ?>
	</div>
	<div class="row">
	<?php echo CHtml::activeLabel($dataProvider->model,'guid'); ?>
	<?php echo CHtml::activeTextField($dataProvider->model,'guid', array('value'=>$model->guid)); ?>
	</div>
	<div class="row">
	<?php echo CHtml::activeLabel($dataProvider->model,'status'); ?>	
	<?php 
		$statuses = $dataProvider->model->getStatusList();
		$statuses = array(-1=>'<any>')+$statuses;
		echo CHtml::dropDownList('DebugInfo[status]', array('selected'=>$model->status), $statuses); 
	?>
	</div>
	<div class="row">
	<?php echo CHtml::activeLabel($dataProvider->model,'dateuploaded'); ?>	
	From:
	<?php 
		$this->widget('zii.widgets.jui.CJuiDatePicker', array(
			'name'=>'DebugInfo[dateFrom]',
			'value'=>$model->dateFrom,
			// additional javascript options for the date picker plugin
			'options'=>array(
				'showAnim'=>'fold',
			),
			'htmlOptions'=>array(
				//'style'=>'height:20px;'
			),
		  ));
	?>
	To:
	<?php 
		$this->widget('zii.widgets.jui.CJuiDatePicker', array(
			'name'=>'DebugInfo[dateTo]',
			'value'=>$model->dateTo,
			// additional javascript options for the date picker plugin
			'options'=>array(
				'showAnim'=>'fold',
			),
			'htmlOptions'=>array(
				//'style'=>'height:20px;'
			),
		  ));
	?>
	</div>
	<div class="row buttons">
	<?php echo CHtml::submitButton('Filter', array('id'=>'btn_filter')); ?>
	</div>
	
	<?php echo CHtml::endForm(); ?>
	</div>
</div>

<div class="span-27 last" >
<?php echo CHtml::beginForm(array('debugInfo/deleteMultiple'), 'post', array('name'=>'del_form')); ?>

<!-- Actions Toolbar -->
<div class="span-27 last">
	<div class="div_actions">
		<?php echo CHtml::link('Upload New File', $this->createUrl('debugInfo/uploadFile')); ?>
		
		<?php 
			echo CHtml::linkButton("Delete Selected", 
					array(
						'id'=>'delete_selected', 
						'form'=>'del_form',
						'confirm'=>"Are you sure you want to permanently delete selected debug info file(s)?"
					)); 
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
			  'cssClassExpression' => '"column-right-align"',			  
          ),
          array(            
              'name' => 'filename',
			  'type' => 'raw',
			  'value' => 'CHtml::link($data->filename, \'view/\'.$data->id)',	  			  			  
          ),
		  array(            // display 'filesize' using a formatted string			  
			  'header'=>'Size',			  
              'name'=>'filesize',
              'value'=>'MiscHelpers::fileSizeToStr($data->filesize)',
			  'cssClassExpression' => '"column-right-align"',
          ),
		  array(                          
			  'header'=>'Status',
			  'name'=>'status',
			  'value'=>'$data->getStatusStr()',			  
			  'cssClassExpression' => '$data->status==DebugInfo::STATUS_INVALID?"status-invalid":""',	
          ),
          array(            
              'name'=>'guid',
              'value'=>'(isset($data->guid) && substr($data->guid, 0, 4)!="tmp_")?$data->guid:"n/a"',
          ),
          array(            // display 'dateuploaded' using an expression
              'name'=>'dateuploaded',
              'value'=>'date("d/m/y H:i", $data->dateuploaded)',              
          ),          
      ),
 )); 
  
 ?>
 </div>
  
 <?php echo CHtml::endForm(); ?>
 </div>

<div class="span-27 last footnote">
	<?php 
		$curVer = "";
		$totalFileSize = 0;
		$percentOfQuota = 0;		
		$count = Yii::app()->user->getCurProject()->getDebugInfoCount(
						$totalFileSize, $percentOfQuota);		
		$totalFileSizeStr = MiscHelpers::fileSizeToStr($totalFileSize);
		$percentOfQuotaStr = sprintf("%.0f", $percentOfQuota);
		echo "This project contains total $totalFileSizeStr in $count file(s)";
        if($percentOfQuota>=0)
            echo " ($percentOfQuotaStr% of disc quota).";
	?> 	
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

$("#link_advanced_search").bind('click', function(e)
{	
	$("#div_simple_search").fadeToggle("fast");			
	$("#div_advanced_search").fadeToggle("fast");		
});
SCRIPT;
 
 Yii::app()->getClientScript()->registerScript("DebugInfo", $script, CClientScript::POS_READY); ?>


<?php endif; ?> 
