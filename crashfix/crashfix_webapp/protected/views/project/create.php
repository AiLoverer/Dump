<?php
$this->breadcrumbs=array(
	'Projects'=>array('index'),
	'Add New Project',
);

$this->menu=array(
	array('label'=>'List Project', 'url'=>array('index')),
	array('label'=>'Manage Project', 'url'=>array('admin')),
);
?>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>