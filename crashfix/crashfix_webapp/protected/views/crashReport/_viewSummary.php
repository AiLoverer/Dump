<?php
$processingErrors = $model->getProcessingErrors();
if(count($processingErrors)>0):	
?>


<div class="flash-error">
    There were some processing errors:
<ul class="processing-errors">
    <?php 	
        foreach($processingErrors as $error)
        {
            echo '<li>'.CHtml::encode($error->message).'</li>';
        }
    ?>
</ul>	
</div>

    
<?php endif;?>

<!-- Actions Toolbar -->
<div class="span-27 last">
	<div class="div_actions">
		
			<?php echo CHtml::form(); ?>		
		
			<?php echo CHtml::hiddenField('id', $model->id); ?>			
		
			<?php
				if(Yii::app()->user->checkAccess('pperm_manage_bugs', array('project_id'=>$model->project_id)) &&
					$model->canOpenNewBug())
				{
					echo CHtml::link('Open Bug', $this->createUrl('bug/create', 
							array('crashreport'=>$model->id)) ); 
				}
			?>
		
			<?php
						
				if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', 
						array('project_id'=>$model->project_id)))
				{
					if($model->canResetStatus())
					{
						echo CHtml::linkButton('Process Again', 
								array(
									'submit'=>Yii::app()->createUrl('crashReport/processAgain'),						
									'confirm'=>"Are you sure you want to process the crash report #".$model->id." another time ?")
								); 					
					}
					
					echo CHtml::linkButton('Delete Report', 
							array(
								'submit'=>Yii::app()->createUrl('crashReport/delete'),						
								'confirm'=>"Are you sure you want to permanently delete the crash report #".$model->id."?")
							); 					
				}
			?>	
		
			<?php echo CHtml::endForm(); ?>		
	</div>	
</div>

<!-- Detail View -->
<?php

// Prepare bug list
$bugList = '';
foreach($model->bugs as $bugCrashReport)
{
	if($bugCrashReport->bug->status<Bug::STATUS_OPEN_MAX)
		$bugList .= CHtml::link('#'.$bugCrashReport->bug_id, array('bug/view', 'id'=>$bugCrashReport->bug_id)).' ';
	if($bugList=='')
		$bugList = 'None';
}


$this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		array(  
            'name'=>'received',
            'type'=>'text',
            'value'=>date("d/m/y H:i", $model->received),		
        ),
		array(  
            'name'=>'date_created',
            'type'=>'text',
            'value'=>date("d/m/y H:i", $model->date_created),		
        ),
		array(  
            'name'=>'status',
            'type'=>'text',
            'value'=>Lookup::item('CrashReportStatus', $model->status),
        ),
		array(        
			'name'=>'filesize',
            'type'=>'raw',
            'value'=>CHtml::encode(MiscHelpers::fileSizeToStr($model->filesize)),
        ),		
		array(  
            'name'=>'project_id',
            'type'=>'text',
            'value'=>$model->project->name,
        ),		
		array(  
            'name'=>'appversion_id',
            'type'=>'text',
            'value'=>$model->appVersion->version,
        ),		
		'crashguid',
		array(  
            'name'=>'crashrptver',
            'type'=>'text',
            'value'=>'CrashRpt '.CrashReport::generatorVersionToStr($model->crashrptver),
        ),		
		array(  
            'name'=>'srcfilename',
            'type'=>'raw',
            'value' => CHtml::link($model->srcfilename, array('crashReport/download', 'id'=>$model->id)),
        ),				
		array(        
			'name'=>'groupid',
            'type'=>'raw',
            'value'=>CHtml::link(CHtml::encode($model->collection->title), array('crashGroup/view', 'id'=>$model->groupid)),
        ),
		array(  
			'label'=>'Open Bug(s)',
			'type'=>'raw',
            'value'=>$bugList,			
        ),
	),
)); 
?>

<div class="span-27 last detail-group-caption">Sender Info:</div>

<?php
$this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		array(  
            'name'=>'geo_location',
            'type'=>'text',
            'value'=>CrashReport::geoIdToCountryName($model->geo_location),
        ),
		'ipaddress',
		'emailfrom',
		'description',		
	),
)); 
?>

<div class="span-27 last detail-group-caption">Exception Info:</div>

<?php

$text = $model->exception_thread_id!=0?'0x'.dechex($model->exception_thread_id):null;
$exceptionThread = $model->getExceptionThread();
if($exceptionThread)
{
    $link = CHtml::link(CHtml::encode('View Stack Trace'), array('crashReport/view', 'id'=>$model->id, 'tab'=>'Threads', 'thread'=>$exceptionThread->id));
    $text .= ' ' . $link;
}
                        

$this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		'exception_type',
		array(        
			'name'=>'exceptionaddress',
            'type'=>'text',
            'value'=>($model->exceptionaddress!=0?'0x'.base_convert($model->exceptionaddress, 10, 16):null),
        ),
		array(        
			'name'=>'exception_code',
            'type'=>'text',
            'value'=>($model->exception_code!=0?'0x'.base_convert($model->exception_code, 10, 16):null),
        ),		
		'exe_image',
		'exceptionmodule',
		array(        
			'name'=>'exceptionmodulebase',
            'type'=>'text',
            'value'=>($model->exceptionmodulebase!=0?'0x'.base_convert($model->exceptionmodulebase, 10, 16):null),
        ),		
		array(        
			'name'=>'exception_thread_id',
            'type'=>'raw',
            'value'=>$text,                                
        ),		
	),
)); 

?>

<div class="span-27 last detail-group-caption">Machine Info:</div>

<?php

$this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		'os_name_reg',
		'os_ver_mdmp',
		array(        
			'name'=>'os_is_64bit',
            'type'=>'text',
            'value'=>$model->getOsBittnessStr(),
        ),	
		'product_type',
		'cpu_architecture',
		'cpu_count'
	),
)); 

?>

<div class="span-27 last detail-group-caption">Resource Usage Info:</div>

<?php

$this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		array(        
			'name'=>'memory_usage_kbytes',
            'type'=>'raw',
            'value'=>($model->memory_usage_kbytes!=null?MiscHelpers::fileSizeToStr($model->memory_usage_kbytes*1024):null),
        ),
		'open_handle_count',		
		'gui_resource_count',
	),
)); 

?>
