<?php
$this->breadcrumbs=array(
	'User Groups'=>array('index'),
	$model->name=>array('view','id'=>$model->id),
	'Update',
);

$this->menu=array(
	array('label'=>'List UserGroup', 'url'=>array('index')),
	array('label'=>'Create UserGroup', 'url'=>array('create')),
	array('label'=>'View UserGroup', 'url'=>array('view', 'id'=>$model->id)),
	array('label'=>'Manage UserGroup', 'url'=>array('admin')),
);
?>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>