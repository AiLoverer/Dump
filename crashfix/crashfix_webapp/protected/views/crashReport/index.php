<?php
$this->pageTitle=Yii::app()->name . ' - Browse Crash Reports';
$this->breadcrumbs=array(	
	'Crash Reports',
);

?>

<?php if(count(Yii::app()->user->getMyProjects())==0): ?>

You have no projects assigned.

<?php else: ?>

<!-- Project Selection Form -->
<div class="span-27 last" id="div_proj_selection">	
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

<?php $this->renderPartial('_reportList', 
				array(
					'route'=>array('crashReport/index'),
					'model'=>$model,
					'dataProvider'=>$dataProvider,
				)
			); 
?>

<div class="span-27 last footnote">
	<?php 
		$curVer = "";
		$totalFileSize = 0;
		$percentOfQuota = 0;		
		$count = Yii::app()->user->getCurProject()->getCrashReportCount(
						$totalFileSize, $percentOfQuota);		
		$totalFileSizeStr = MiscHelpers::fileSizeToStr($totalFileSize);
		$percentOfQuotaStr = sprintf("%.0f", $percentOfQuota);
		echo "This project contains total $totalFileSizeStr in $count file(s)";
        if($percentOfQuota>=0) 
            echo " ($percentOfQuotaStr% of disc quota).";
	?> 	
</div>

<?php endif; ?>