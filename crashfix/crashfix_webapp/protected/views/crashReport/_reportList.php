<!-- Simple Search Form -->
<div class="span-27 last" id="div_simple_search" style="display:<?php echo !$model->isAdvancedSearch?'inherit':'none'?>">	
	<?php echo CHtml::beginForm($route, 'get', array('id'=>'proj_form')); ?>
	<div class="span-27 last"><p id="stat_filter">Search by IP Address/E-mail/Description or use <a href="#" id="link_advanced_search">advanced search</a>:</p></div>
	<div class="span-25">
		<?php echo CHtml::textField('q', isset($model->filter)?CHtml::encode($model->filter):"", array('id'=>'text_filter')); ?>		
	</div>
	<div class="span-2 last">
		<?php echo CHtml::submitButton('Search', array('id'=>'btn_filter')); ?>
	</div>	
	<?php echo CHtml::endForm(); ?>
</div>

<!-- Advanced Search Form -->
<div class="span-27 last" id="div_advanced_search" style="display:<?php echo $model->isAdvancedSearch?'inherit':'none'?>">	
	<div class="wide form" id="form_advanced_search">
	<?php echo CHtml::beginForm($route, 'post'); ?>
	
	<h4>Advanced Search:</h4>
	<?php echo CHtml::errorSummary($model); ?>
	<div class="row">
		<?php echo CHtml::activeLabel($model, 'ipaddress'); ?>
		<?php echo CHtml::activeTextField($model, 'ipaddress', 
				array('value'=>$model->ipaddress)); ?>		
	</div>
	<div class="row">
		<?php echo CHtml::activeLabel($model, 'emailfrom'); ?>
		<?php echo CHtml::activeTextField($model, 'emailfrom', 
				array('value'=>$model->emailfrom)); ?>		
	</div>
	<div class="row">
		<?php echo CHtml::activeLabel($model, 'description'); ?>
		<?php echo CHtml::activeTextField($model, 'description', 
				array('value'=>$model->description)); ?>		
	</div>
	<div class="row">
	<?php echo CHtml::activeLabel($model,'status'); ?>	
	<?php 
		$statuses = Lookup::items('CrashReportStatus', $model->status);
		$statuses = array(-1=>'<any>')+$statuses;
		echo CHtml::dropDownList('CrashReport[status]', array('selected'=>$model->status), $statuses); 
	?>
	</div>
	<div class="row">
	<?php echo CHtml::activeLabel($model,'received'); ?>	
	From:
	<?php 
		$this->widget('zii.widgets.jui.CJuiDatePicker', array(
			'name'=>'CrashReport[receivedFrom]',
			'value'=>$model->receivedFrom,
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
			'name'=>'CrashReport[receivedTo]',
			'value'=>$model->receivedTo,
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
	<?php echo CHtml::submitButton('Search', array('id'=>'btn_filter')); ?>
	</div>
	
	<?php echo CHtml::endForm(); ?>
	</div>
</div>

<div class="span-27 last" >
<?php echo CHtml::beginForm(array('crashReport/deleteMultiple'), 'post', 
		array('name'=>'del_form')); 
?>

<!-- Actions Toolbar -->
<div class="span-27 last">
	<div class="div_actions">
		<?php 
			if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', array('project_id'=>Yii::app()->user->getCurProjectId())))
				echo CHtml::link('Upload New File', 
						$this->createUrl('crashReport/uploadFile')); 
		?>
		    
        <?php if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', array('project_id'=>Yii::app()->user->getCurProjectId()))): ?>
        <ul class="dropdown-menu">
            <li><a href="#">Other Actions...</a>
                <ul>
                    <?php 
                        if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', array('project_id'=>Yii::app()->user->getCurProjectId())))
                            echo '<li>'.CHtml::linkButton("Delete Selected Reports", 
                                    array(
                                        'submit'=>$this->createUrl('crashReport/deleteMultiple', isset($groupid)?array('groupid'=>$groupid):array()),
                                        'form'=>'del_form',
                                        'confirm'=>"Are you sure you want to permanently delete selected crash report(s)?"
                                    )).'</li>'; 
                    ?>	
                    
                    <?php 
                        if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', array('project_id'=>Yii::app()->user->getCurProjectId())))
                            echo '<li>'.CHtml::linkButton("Reprocess Selected Reports", 
                                    array(
                                        'submit'=>$this->createUrl('crashReport/reprocessMultiple', isset($groupid)?array('groupid'=>$groupid):array()),
                                        'form'=>'del_form',
                                        'confirm'=>"Are you sure you want to reprocess selected crash report(s)?"
                                    )).'</li>'; 
                    ?>	
                    
                    <?php 
                        if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', array('project_id'=>Yii::app()->user->getCurProjectId())))
                        {
                            $text = isset($groupid)?'in this collection':'in the currently selected project version';
                            echo '<li>'.CHtml::linkButton("Reprocess All Reports", 
                                    array(
                                        'submit'=>$this->createUrl('crashReport/reprocessAll', isset($groupid)?array('groupid'=>$groupid):array()),
                                        'form'=>'del_form',
                                        'confirm'=>"WARNING! This action may take a long time. Are you sure you want to reprocess all crash reports ".$text."?"
                                    )).'</li>'; 
                        }
                    ?>	
                </ul>
            </li>            
        </ul>
        <?php endif; ?>
        
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
			  'type' => 'raw',
			  'value' => 'CHtml::link($data->id, array(\'crashReport/view/\', \'id\'=>$data->id))',	  			  			 
			  'cssClassExpression' => '"column-right-align"',
          ),		  
		  array(                          			  
			  'name'=>'filesize',
			  'value'=>'MiscHelpers::fileSizeToStr($data->filesize, 1)',			  
			  'cssClassExpression' => '"column-right-align"',
          ),
		  array(                          			  
			  'name'=>'status',
			  'value'=>'Lookup::item(\'CrashReportStatus\', $data->status)',
			  'cssClassExpression' => '$data->status==CrashReport::STATUS_INVALID?"status-invalid":""',		
          ),
          array(            // display 'dateuploaded' using an expression
              'name'=>'received',
              'value'=>'date("d/m/y H:i", $data->received)',
          ),          
		  'ipaddress',
		  'emailfrom',
		  array(            
              'name'=>'description',
              'value'=>'MiscHelpers::addEllipsis($data->description, 60)',
          ),  
      ),
 )); 
  
 ?>

 <?php echo CHtml::endForm(); ?>
 </div>
</div>
		
		
<?php 
 $script = <<<SCRIPT

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
 
 Yii::app()->getClientScript()->registerScript(
		 "CrashReport", 
		 $script, 
		 CClientScript::POS_READY); 
 ?>
