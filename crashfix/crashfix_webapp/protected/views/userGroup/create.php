<?php
$this->breadcrumbs=array(
	'Administer'=>array('site/admin'), 
	'Groups'=>array('userGroup/view', 'id'=>$model->id),
	'Add New Group'
);

?>

<?php

$this->renderPartial('_form', array('model'=>$model));

?>