<?php
$this->breadcrumbs=array(
	'Crash Groups'=>array('index'),
	$model->title=>array('view','id'=>$model->id),
	'Update',
);

$this->menu=array(
	array('label'=>'List CrashGroup', 'url'=>array('index')),
	array('label'=>'Create CrashGroup', 'url'=>array('create')),
	array('label'=>'View CrashGroup', 'url'=>array('view', 'id'=>$model->id)),
	array('label'=>'Manage CrashGroup', 'url'=>array('admin')),
);
?>

<h1>Update CrashGroup <?php echo $model->id; ?></h1>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>