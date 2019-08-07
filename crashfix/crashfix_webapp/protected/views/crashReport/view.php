<?php
$this->pageTitle=Yii::app()->name . ' - View Report #'.CHtml::encode($model->id);
$this->breadcrumbs=array(
	'Crash Reports'=>array('index'),
	'View Report #'.$model->id,
);

?>

<!-- Top Menu -->
<div class="span-27 last" id="adminmenu">
<?php
$this->widget('zii.widgets.CMenu', array(
			'activateItems'=>false,
			'items'=>array(				
				array(
                    'label'=>'Summary', 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=Summary'), 
                    'active'=>$activeItem=='Summary'
                    ),
                array(
                    'label'=>'Files', 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=Files'), 
                    'active'=>$activeItem=='Files',                    
                    ),
				array(
                    'label'=>'Custom Props'.($customProps->totalItemCount>0?' ('.$customProps->totalItemCount.')':''), 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=CustomProps'), 
                    'active'=>$activeItem=='CustomProps',
                    'linkOptions'=>array('class'=>($customProps->totalItemCount>0?'':'menu-item-grayed')),
                    ),
				array(
                    'label'=>'Screenshots'.($screenshots->totalItemCount>0?' ('.$screenshots->totalItemCount.')':''), 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=Screenshots'), 
                    'active'=>$activeItem=='Screenshots',
                    'linkOptions'=>array('class'=>($screenshots->totalItemCount>0?'':'menu-item-grayed')),
                    ),
                array(
                    'label'=>'Videos'.($videos->totalItemCount>0?' ('.$videos->totalItemCount.')':''), 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=Videos'), 
                    'active'=>$activeItem=='Videos',
                    'linkOptions'=>array('class'=>($videos->totalItemCount>0?'':'menu-item-grayed')),
                    ),
				array(
                    'label'=>'Threads'.($threads->totalItemCount>0?' ('.$threads->totalItemCount.')':''), 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=Threads'), 
                    'active'=>$activeItem=='Threads',
                    'linkOptions'=>array('class'=>($threads->totalItemCount>0?'':'menu-item-grayed')),
                    ),
				array(
                    'label'=>'Modules'.($modules->totalItemCount>0?' ('.$modules->totalItemCount.')':''), 
                    'url'=>array('crashReport/view/'.$model->id.'?tab=Modules'), 
                    'active'=>$activeItem=='Modules',
                    'linkOptions'=>array('class'=>($modules->totalItemCount>0?'':'menu-item-grayed')),
                    ),								
              )
		));
?>
</div>

<div class="span-27 prepend-top last">

<?php
$params = array('model'=>$model);
$params['stackTrace'] = isset($stackTrace)?$stackTrace:null;
$params['threadModel'] = isset($threadModel)?$threadModel:null;

if($activeItem=='Summary')
	$this->renderPartial('_viewSummary', $params);
else if($activeItem=='CustomProps')
	$this->renderPartial('_viewCustomProps', $params);
else if($activeItem=='Screenshots')
	$this->renderPartial('_viewScreenshots', $params);
else if($activeItem=='Videos')
	$this->renderPartial('_viewVideos', $params);
else if($activeItem=='Threads')
	$this->renderPartial('_viewThreads', $params);
else if($activeItem=='Modules')
	$this->renderPartial('_viewModules', $params);
else if($activeItem=='Files')
	$this->renderPartial('_viewFiles', $params);
?>
	
</div>