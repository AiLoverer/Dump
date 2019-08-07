<?php

$this->pageTitle=Yii::app()->name . ' - Browse Daemon Status';

$this->breadcrumbs=array(
	'Administer', 
	'Daemon',
);

?>

<div class="subheader">Daemon Status:</div>

<div class="span-18 last">
<div id="daemon_status" class="flash-success">
    <ul class="daemon-status-list">
        <li class="daemon-status-list-item">
            Querying daemon status...
        </li>
    </ul>
</div>
</div>

<div class="span-18 last">
<div class="subheader">Recent Daemon Operations:</div>

<!-- Grid view -->
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$dataProvider,
	  'selectableRows'=>null,
      'columns'=>array(		            		  
		  array(  
			'header'=>'Operation',
            'name'=>'optype',            
            'value'=>'$data->getOperationDescStr()',
		  ),
		  array(                          
			  'header'=>'Status',
			  'name'=>'status',
			  'value'=>'Lookup::item(\'OperationStatus\', $data->status)',			  
          ),		  
		  array(  
            'name'=>'timestamp',
            'type'=>'text',
            'value'=>'date("d/m/y H:i", $data->timestamp)',
		  ),		  
      ),
 )); 
  
 ?>
</div>

<?php 
 
 $url = Yii::app()->createAbsoluteUrl('site/daemonStatus');

 $script = <<<SCRIPT

	$.ajax({		
		url: "$url",
		data: ""
	}).done(function( msg ) {
		$("#daemon_status").replaceWith(msg);
	});

 
SCRIPT;
 
 Yii::app()->getClientScript()->registerScript("DebugInfo", $script, CClientScript::POS_READY); ?>