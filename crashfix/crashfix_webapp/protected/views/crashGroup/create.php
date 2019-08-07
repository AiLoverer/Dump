<?php
$this->breadcrumbs=array(
	'Crash Groups'=>array('index'),
	'Create',
);

$this->menu=array(
	array('label'=>'List CrashGroup', 'url'=>array('index')),
	array('label'=>'Manage CrashGroup', 'url'=>array('admin')),
);
?>

<h1>Create CrashGroup</h1>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>