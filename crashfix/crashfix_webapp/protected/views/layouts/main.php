<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<meta name="language" content="en" />

	<!-- blueprint CSS framework -->
	<link rel="stylesheet" type="text/css" href="<?php echo Yii::app()->request->baseUrl; ?>/css/screen.css" media="screen, projection" />
	<link rel="stylesheet" type="text/css" href="<?php echo Yii::app()->request->baseUrl; ?>/css/print.css" media="print" />
	<!--[if lt IE 8]>
	<link rel="stylesheet" type="text/css" href="<?php echo Yii::app()->request->baseUrl; ?>/css/ie.css" media="screen, projection" />
	<![endif]-->
	
	<link rel="stylesheet" type="text/css" href="<?php echo Yii::app()->request->baseUrl; ?>/css/main.css" />
	<link rel="stylesheet" type="text/css" href="<?php echo Yii::app()->request->baseUrl; ?>/css/form.css" />

	<title><?php echo CHtml::encode($this->pageTitle); ?></title>
</head>

<body>

<div class="container" id="page">

	<div id="daemon-check">		
	</div>
	
	<div class="span-32" id="header">
		<div class="span-3">
			<div id="logo_image">
				<a href="<?php echo Yii::app()->baseUrl.'/index.php'?>"><?php echo CHtml::image(Yii::app()->baseUrl.'/images/logo.png'); ?></a>
			</div>
		</div>				
		<div class="span-27">
			<div id="user_name" style="visibility: <?php echo Yii::app()->user->isGuest?'hidden':'visible'; ?>">
				<?php if(!Yii::app()->user->isGuest){ echo 'Logged in as '.CHtml::link(Yii::app()->user->getName(), array('user/view', 'id'=>Yii::app()->user->getId())); } ?>&nbsp;
			</div>
		</div>
		<div class="span-2 last" id="login">						
			<?php 			
				if(!Yii::app()->user->isGuest)  
				{					
					echo CHtml::link('Log out', $this->createUrl('site/logout'));
				}					
			?>
		</div>		
	</div><!-- header -->

	<?php echo $content; ?>

	<div class="span-32">
		<div id="footer">
			CrashFix v.<?php echo Yii::app()->params['version']; ?>. Copyright &copy; <?php echo date('Y'); ?> by Oleg Krivtsov.	All Rights Reserved.
		</div>
	</div><!-- footer -->

</div><!-- page -->

</body>
</html>

<?php 

if(!Yii::app()->user->isGuest)
{

	$url = Yii::app()->createAbsoluteUrl('site/checkDaemon');

	$script = <<<SCRIPT

$.ajax({		
		url: "$url",
		data: ""
	}).done(function( msg ) {		
        $("#daemon-check").replaceWith(msg);			
	});
	
$("#btn_close").live('click', function(e)
{	
	$("#daemon-check").fadeToggle("fast");				
});

SCRIPT;

	Yii::app()->getClientScript()->registerScript(
		"MainDaemonCheck", 
		$script, 
		CClientScript::POS_READY); 
}
 ?>
