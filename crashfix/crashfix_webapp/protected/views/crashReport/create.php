<?php
$this->breadcrumbs=array(
	'Crash Reports'=>array('index'),
	'Create',
);

$this->menu=array(
	array('label'=>'List CrashReport', 'url'=>array('index')),
	array('label'=>'Manage CrashReport', 'url'=>array('admin')),
);
?>

<h1>Create CrashReport</h1>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>