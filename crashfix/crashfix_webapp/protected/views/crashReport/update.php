<?php
$this->breadcrumbs=array(
	'Crash Reports'=>array('index'),
	$model->id=>array('view','id'=>$model->id),
	'Update',
);

$this->menu=array(
	array('label'=>'List CrashReport', 'url'=>array('index')),
	array('label'=>'Create CrashReport', 'url'=>array('create')),
	array('label'=>'View CrashReport', 'url'=>array('view', 'id'=>$model->id)),
	array('label'=>'Manage CrashReport', 'url'=>array('admin')),
);
?>

<h1>Update CrashReport <?php echo $model->id; ?></h1>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>