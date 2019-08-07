<?php
$this->breadcrumbs=array(
	'Administer',
	'Projects'=>array('index'),
	$model->name=>array('view','id'=>$model->id),
	'Update',
);

?>

<?php echo $this->renderPartial('_form', array('model'=>$model)); ?>