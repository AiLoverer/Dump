<?php
$this->pageTitle = 'Admin - CrashFix';

$this->breadcrumbs=array(
	'Administer', 
	'General',
);
?>

<div class="subheader">License Information:</div>

<div class="span-18 last">
<div id="license_info" class="div_license loading">
	<i>Querying license information ...</i>
</div>
</div>

<?php 
 
 $url = Yii::app()->createAbsoluteUrl('site/admin');

 $script = <<<SCRIPT

	$.ajax({		
		url: "$url",
		data: ""
	}).done(function( msg ) {
		$("#license_info").replaceWith(msg);
	});

 
SCRIPT;
 
 Yii::app()->getClientScript()->registerScript("DebugInfo", $script, CClientScript::POS_READY); ?>