<?php
$this->pageTitle=Yii::app()->name . ' - View Collection #'.CHtml::encode($model->id);
$this->breadcrumbs=array(
	'Crash Collections'=>array('index'),
	'Collection #'.$model->id,
);

?>

<!-- Actions Toolbar -->
<div class="span-27 last">
	<div class="div_actions">
		<?php echo CHtml::form(); ?>		
		
		<?php echo CHtml::hiddenField('id', $model->id); ?>		
		
		<?php
			if(Yii::app()->user->checkAccess('pperm_manage_bugs', array('project_id'=>Yii::app()->user->getCurProjectId())))
			{
				echo CHtml::link('Open Bug', $this->createUrl('bug/create', 
						array('crashgroup'=>$model->id,)) ); 
			}
		?>
		
		<?php
			if(Yii::app()->user->checkAccess('pperm_manage_crash_reports', array('project_id'=>Yii::app()->user->getCurProjectId())))
			{				
				echo CHtml::linkButton('Delete Collection', 
						array(
							'submit'=>Yii::app()->createUrl('crashGroup/delete'),						
							'confirm'=>"Are you sure you want to permanently delete the collection #".$model->id."?")
						); 					
			}	
		?>
		
		<?php echo CHtml::endForm(); ?>		
	</div>	
</div>

<!-- Detail View -->
<div class="span-27 last">
<?php 

// Prepare bug list
$bugList = '';
foreach($model->bugs as $bugCrashGroup)
{
	if($bugCrashGroup->bug->status<Bug::STATUS_OPEN_MAX)
		$bugList .= CHtml::link('#'.$bugCrashGroup->bug_id, array('bug/view', 'id'=>$bugCrashGroup->bug_id)).' ';	
}

if($bugList=='')
	$bugList = 'None';

$this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(		
		'title',
		array(  
            'name'=>'created',
            'type'=>'datetime',            
        ),		
		array(  
			'label'=>'Crash Report Count',
            'name'=>'crashReportCount',			
        ),
		array(  
			'label'=>'Open Bugs',
			'type'=>'raw',
            'value'=>$bugList,			
        ),
	),
)); ?>
</div>

<div class="span-27 last prepend-top subheader">Crash Reports Belonging this Collection:</div>

<?php $this->renderPartial('/crashReport/_reportList', 
				array(
					'route'=>array('crashGroup/view', 'id'=>$model->id),
                    'groupid'=>$model->id,
					'model'=>$crashReportModel,
					'dataProvider'=>$crashReportDataProvider,
				)
			); 
?>

<div class="span-27 last footnote">
	<?php 
		$curVer = "";
		$totalFileSize = 0;
		$percentOfQuota = 0;		
		$count = $model->getCrashReportCount(
				$curVer, $totalFileSize, $percentOfQuota);		
		$totalFileSizeStr = MiscHelpers::fileSizeToStr($totalFileSize);
		$percentOfQuotaStr = sprintf("%.0f", $percentOfQuota);
		echo "This collection contains total $totalFileSizeStr in $count file(s)";
        if($percentOfQuota>=0)
            echo " ($percentOfQuotaStr% of max count).";
	?> 	
</div>
