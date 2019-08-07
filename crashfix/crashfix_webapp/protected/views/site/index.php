<?php $this->pageTitle=Yii::app()->name; ?>

<?php

$this->breadcrumbs=array(	
	'Digest',
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

<div class="span-27 prepend-top last">	
	
	<?php if(Yii::app()->user->checkAccess('pperm_browse_crash_reports', 
			array('project_id'=>Yii::app()->user->getCurProjectId()))): 
	?>
	
	<div class="span-18">	
		<div class="digest-pane" style="height:240px;">
			<div class="subheader" id="header_crash_report_uploads" title="Displays how many crash report files were uploaded recently and their upload dynamics">Crash Report Uploads</div>
            <div class="loading digest-pane-image" id="crashreport-upload-stat" style="width:685px;height:200px;"></div>
			<a id="link_week" href="javascript:;">Week</a>
			<a id="link_month" href="javascript:;">Month</a>
			<a id="link_year" href="javascript:;">Year</a>
		</div>
	</div>
        
    <div class="span-9 last">
		<div class="digest-pane" style="height:240px;">
			<div class="subheader" title="Displays crash reports count for current project and version, or for all versions of current project">Crash Report Totals</div>
			<ul  class="totals">
			<?php 
				$selVer = Yii::app()->user->getCurProjectVer();
				if($selVer!=-1):
			?>
			<li>Current version: 
			<ul><li>
			<?php 
				$totalFileSize = 0;
				$percentOfDiskQuota = 0;
				$curProjectVer = Yii::app()->user->getCurProjectVer();
				$reportCount = Yii::app()->user->getCurProject()->getCrashReportCount($totalFileSize, $percentOfDiskQuota, $curProjectVer); 
				$percentOfDiskQuotaStr = sprintf("%.0f", $percentOfDiskQuota);
				echo $reportCount;
			?>
				crash reports 
			(<?php echo MiscHelpers::fileSizeToStr($totalFileSize); ?>)</li>
			</ul>
			<?php endif; ?>
			<li>Entire project, all versions: 
			<ul><li>
			<?php 
				$totalFileSize = 0;
				$percentOfDiskQuota = 0;
				$reportCount = Yii::app()->user->getCurProject()->getCrashReportCount($totalFileSize, $percentOfDiskQuota); 
				$percentOfDiskQuotaStr = sprintf("%.0f", $percentOfDiskQuota);
				echo $reportCount.' crash reports ('.MiscHelpers::fileSizeToStr($totalFileSize);
                if($percentOfDiskQuota>0)
                    echo ', '.$percentOfDiskQuotaStr.'% of disk quota';
                echo ')';
			?>
			
			<?php $this->widget('zii.widgets.jui.CJuiProgressBar', array(
						'value'=>$percentOfDiskQuota,
						// additional javascript options for the progress bar plugin
						'options'=>array(							
						),
						'htmlOptions'=>array(
							'style'=>'width:230px;height:20px;margin:5px'
						),
					));
			?>
			</li>
			</ul>
			</ul>
		</div>
	</div>		
    
    <div class="span-18">
    
        <div class="span-18 prepend-top last">	
            <div class="digest-pane"  style="height:240px;">
                <div class="subheader" title="Displays how many crash reports were uploaded for each version of current project">Crash Reports per Project Version</div>
                <div class="loading digest-pane-image" id="crashreport-version-distrib" style="width:685px;height:200px;"></div>
            </div>
        </div>
        
        <div class="span-18 prepend-top last">
            <div class="digest-pane" style="height:240px;">
                <div class="subheader" title="Displays operating system names where crashes occurred">OS Version Distribution</div>
                <div class="loading digest-pane-image" id="crashreport-os-version-distrib" style="width:685px;height:200px;"></div>			
            </div>
        </div>
       
    </div>    
       
    <div class="span-9 prepend-top last">
		<div class="digest-pane" style="height:522px;">
			<div class="subheader" title="Displays from what countries the crash reports were uploaded">Geographic Locations</div>
			<div class="loading digest-pane-image" id="crashreport-geo-locations" style="width:325px;height:480px;"></div>
		</div>
	</div>	
	        	        
	<div class="span-27 prepend-top">
		<div class="digest-pane">
		<div class="subheader" title="Displays top collections containing the majority of crash reports">Top Crash Collections</div>				
		<?php
			$curProjectVer = Yii::app()->user->getCurProjectVer();
			$topCollections = Yii::app()->user->getCurProject()->getTopCrashGroups($curProjectVer);			
			if(count($topCollections)==0)
			{
				echo '<ul class="totals"><li>No data available<li></ul>';
			}
			else
			{	                
				echo '<ul id="top_collections">';
                
                $percentOfDiskQuota = 0;
                $curProjectVer = Yii::app()->user->getCurProjectVer();
                $reportCount = Yii::app()->user->getCurProject()->getCrashReportCount($totalFileSize, $percentOfDiskQuota, $curProjectVer); 
                foreach($topCollections as $collection)
				{					
					$percent = sprintf("%.0f", $reportCount!=0?100*$collection->crashReportCount/$reportCount:0);
					echo '<li>'.$collection->crashReportCount.' reports ('.$percent.'%) in '.CHtml::link(CHtml::encode(MiscHelpers::addEllipsis($collection->title, 110)), array('crashGroup/view', 'id'=>$collection->id), array('class'=>'top-collection-title')).'</li>';
				}				
				echo '</ul>';
			}
		?>				
		</div>
	</div>
    	
	<?php endif;?>
	
	<?php if(Yii::app()->user->checkAccess('pperm_browse_bugs',
			array('project_id'=>Yii::app()->user->getCurProjectId()))): 
	?>
	
	<div class="span-18 prepend-top">	
		<div class="digest-pane" style="height:240px;">
			<div class="subheader" title="Displays how bug statuses changed recently">Bug Dynamics</div>						
			<div class="loading digest-pane-image" id="bug-dynamics-stat" style="width:685px;height:200px;"></div>
			<a id="link_bug_week" href="javascript:;">Week</a>
			<a id="link_bug_month" href="javascript:;">Month</a>
			<a id="link_bug_year" href="javascript:;">Year</a>
		</div>
	</div>
    
    <div class="span-9 prepend-top last">
		<div class="digest-pane" style="height:240px;">
			<div class="subheader" title="Displays how many bugs (in percent) are open or closed">Bug Statuses</div>
			<div class="loading digest-pane-image" id="bug-status-dist" style="width:325px;height:200px;"></div>
		</div>
	</div>
		
	<div class="span-27 last prepend-top">	
		<div class="digest-pane">
			<div class="subheader" title="Displays what bug changes have been performed recently">Recent Bug Changes</div>			
			<?php
				$curProjectVer = Yii::app()->user->getCurProjectVer();
				$recentBugChanges = Yii::app()->user->getCurProject()->getRecentBugChanges($curProjectVer);
				if(count($recentBugChanges)==0)
				{
					echo '<ul class="totals"><li>No data available<li></ul>';
				}
				else
				{
					foreach($recentBugChanges as $bugChange)
					{
						$this->renderPartial('_bugChange', array('model'=>$bugChange));
					}
				}
			?>			
		</div>
	</div>
		
	<?php endif;?>
	
	<?php 
		if(Yii::app()->user->checkAccess('pperm_browse_debug_info', 
				array('project_id'=>Yii::app()->user->getCurProjectId()))): 
	?>
	
	<div class="span-9 loud prepend-top">
		<div class="digest-pane" style="height:240px;">
		<div class="subheader" title="Displays how many debug info files were uploaded for this project">Debug Info Totals (All Versions)</div>
			<ul class="totals">
                <li>
                <?php 
                    $totalFileSize = 0;
                    $percentOfDiskQuota = 0;
                    $fileCount = Yii::app()->user->getCurProject()->getDebugInfoCount($totalFileSize, $percentOfDiskQuota); 
                    $percentOfDiskQuotaStr = sprintf("%.0f", $percentOfDiskQuota);
                    echo $fileCount.' files ('.MiscHelpers::fileSizeToStr($totalFileSize);
                    if($percentOfDiskQuota>0)
                        echo ', '.$percentOfDiskQuotaStr.'% of disk quota';
                    echo ')';
                ?>
                
                <?php $this->widget('zii.widgets.jui.CJuiProgressBar', array(
                            'value'=>$percentOfDiskQuota,
                            // additional javascript options for the progress bar plugin
                            'options'=>array(							
                            ),
                            'htmlOptions'=>array(
                                'style'=>'height:20px;margin:5px'
                            ),
                        ));
                ?>
                </li>
            </ul>
		</div>
	</div>
	
	<div class="span-18 last prepend-top">	
		<div class="digest-pane" style="height:240px;">
			<div class="subheader" title="Displays recent debug info file upload dynamics for all version of this project">Debug Info Uploads (All Versions)</div>	
			<div class="loading digest-pane-image" id="debuginfo-upload-stat" style="width:685px;height:200px;"></div>
		</div>
	</div>	
	
	<?php endif;?>
	
	<p/>
	
</div>

<?php 

$urlVersionDist = Yii::app()->createAbsoluteUrl('crashReport/versionDist', array('w'=>684, 'h'=>200));
$urlOSVersionDist = Yii::app()->createAbsoluteUrl('crashReport/osVersionDist', array('w'=>684, 'h'=>200));
$urlCrashReportUploads7 = Yii::app()->createAbsoluteUrl('crashReport/uploadStat', array('w'=>685, 'h'=>200, 'period'=>7));
$urlCrashReportUploads30 = Yii::app()->createAbsoluteUrl('crashReport/uploadStat', array('w'=>685, 'h'=>200, 'period'=>30));
$urlCrashReportUploads365 = Yii::app()->createAbsoluteUrl('crashReport/uploadStat', array('w'=>685, 'h'=>200, 'period'=>365));

$urlBugDynamicsStat7 = Yii::app()->createAbsoluteUrl('bug/statusDynamics', array('w'=>684, 'h'=>200, 'period'=>7));
$urlBugDynamicsStat30 = Yii::app()->createAbsoluteUrl('bug/statusDynamics', array('w'=>684, 'h'=>200, 'period'=>30));
$urlBugDynamicsStat365 = Yii::app()->createAbsoluteUrl('bug/statusDynamics', array('w'=>684, 'h'=>200, 'period'=>365));

$urlDebugInfoUploads7 = Yii::app()->createAbsoluteUrl('debugInfo/uploadStat', array('w'=>685, 'h'=>200, 'period'=>7));
$urlDebugInfoUploads30 = Yii::app()->createAbsoluteUrl('debugInfo/uploadStat', array('w'=>685, 'h'=>200, 'period'=>30));
$urlDebugInfoUploads365 = Yii::app()->createAbsoluteUrl('debugInfo/uploadStat', array('w'=>685, 'h'=>200, 'period'=>365));

$urlBugStatusDist = Yii::app()->createAbsoluteUrl('bug/statusDist', array('w'=>324, 'h'=>200));

$geoLocationsUrl = Yii::app()->createAbsoluteUrl('crashReport/geoLocationDist', array('w'=>325, 'h'=>480));

$script = <<<SCRIPT

function loadCrashReportUploads(period)
{	
	$("#crashreport-upload-stat").empty();			
	$("#crashreport-upload-stat").addClass("loading");			

	var url = "$urlCrashReportUploads7";
	if(period==30)
		url = "$urlCrashReportUploads30";
	if(period==365)
		url = "$urlCrashReportUploads365";
		
	var img = $("<img />").attr('src', url)
		.load(function() {
			if (!this.complete) {
				$("#crashreport-upload-stat").removeClass("loading");			
			} else {				
				$("#crashreport-upload-stat").html(img);
				$("#crashreport-upload-stat").removeClass("loading");			
			}
		});
}

function loadDebugInfoUploads(period)
{	
	$("#debuginfo-upload-stat").empty();			
	$("#debuginfo-upload-stat").addClass("loading");			

	var url = "$urlDebugInfoUploads7";
	if(period==30)
		url = "$urlDebugInfoUploads30";
	if(period==365)
		url = "$urlDebugInfoUploads365";
		
	var img = $("<img />").attr('src', url)
		.load(function() {
			if (!this.complete) {
				$("#debuginfo-upload-stat").removeClass("loading");			
			} else {				
				$("#debuginfo-upload-stat").html(img);
				$("#debuginfo-upload-stat").removeClass("loading");			
			}
		});
}

function loadBugDynamics(period)
{	
	$("#bug-dynamics-stat").empty();			
	$("#bug-dynamics-stat").addClass("loading");			

	var url = "$urlBugDynamicsStat7";
	if(period==30)
		url = "$urlBugDynamicsStat30";
	if(period==365)
		url = "$urlBugDynamicsStat365";
		
	var img = $("<img />").attr('src', url)
		.load(function() {
			if (!this.complete) {
				$("#bug-dynamics-stat").removeClass("loading");			
			} else {				
				$("#bug-dynamics-stat").html(img);
				$("#bug-dynamics-stat").removeClass("loading");			
			}
		});
}


$("#proj, #ver").bind('change', function(e)
{	
	$("#proj_form").submit();
});

$("#link_week").bind('click', function(e)
{	
	loadCrashReportUploads(7);
});

$("#link_month").bind('click', function(e)
{		
	loadCrashReportUploads(30);
});

$("#link_year").bind('click', function(e)
{	
	loadCrashReportUploads(365);
});

$("#link_bug_week").bind('click', function(e)
{	
	loadBugDynamics(7);
});

$("#link_bug_month").bind('click', function(e)
{		
	loadBugDynamics(30);
});

$("#link_bug_year").bind('click', function(e)
{	
	loadBugDynamics(365);
});


loadCrashReportUploads(7);
loadDebugInfoUploads(7);
loadBugDynamics(7);

var img2 = $("<img />").attr('src', "$urlVersionDist")
	.load(function() {
		if (!this.complete) {
			$("#crashreport-version-distrib").removeClass("loading");			
		} else {
			$("#crashreport-version-distrib").html(img2);			
			$("#crashreport-version-distrib").removeClass("loading");			
		}
	});

var img3 = $("<img />").attr('src', "$urlOSVersionDist")
	.load(function() {
		if (!this.complete) {
			$("#crashreport-os-version-distrib").removeClass("loading");			
		} else {
			$("#crashreport-os-version-distrib").html(img3);			
			$("#crashreport-os-version-distrib").removeClass("loading");			
		}
	});

var img4 = $("<img />").attr('src', "$urlBugStatusDist")
	.load(function() {
		if (!this.complete) {
			$("#bug-status-dist").removeClass("loading");			
		} else {
			$("#bug-status-dist").html(img4);			
			$("#bug-status-dist").removeClass("loading");			
		}
	});

$.ajax({		
		url: "$geoLocationsUrl",
		data: ""
	}).done(function( msg ) {
		$("#crashreport-geo-locations").replaceWith(msg);        
	});


SCRIPT;
 
 Yii::app()->getClientScript()->registerScript(
		 "DebugInfo", 
		 $script, 
		 CClientScript::POS_READY); 
  
 ?>

<?php endif; ?>

