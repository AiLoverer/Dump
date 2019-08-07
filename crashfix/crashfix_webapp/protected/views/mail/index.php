<?php
$this->pageTitle = Yii::app()->name . ' - Browse Outgoing Mail';

$this->breadcrumbs = array(
    'Administer',
    'Mail',
);
?>

<!-- Flash messages -->
<div class="span-18 last">
<?php
foreach (Yii::app()->user->getFlashes() as $key => $message) {
    echo '<div class="flash-' . $key . '">' . $message . "</div>\n";
}
?>
</div>

<!-- Simple Search Form -->
<div class="span-18 last" id="div_simple_search">
	<?php echo CHtml::beginForm(Yii::app()->createUrl($this->route), 'get'); ?>
	<div class="span-18 last"><p id="stat_filter">Search by subject/E-mail address:</p></div>
	<div class="span-3"><div id="dropdown_search">
		<?php 
			$statuses = array(												
				 1=>'Pending mail',
				 2=>'Sent mail',
                                 3=>'Failed mail',
				-1=>'All mail',
			);			
			echo CHtml::dropDownList('status', array('selected'=>isset($model->statusFilter)?CHtml::encode($model->statusFilter):-1), $statuses); 
		?></div>
	</div>
	<div class="span-13">
		<?php echo CHtml::textField('q', isset($model->searchFilter) ? CHtml::encode($model->searchFilter):"", array('id'=>'text_filter')); ?>
	</div>
	<div class="span-2 last">
		<?php echo CHtml::submitButton('Search', array('id'=>'btn_filter')); ?>
	</div>
	<?php echo CHtml::endForm(); ?>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
	<div class="div_actions">
				
	</div>	
</div>

<!-- Grid view -->
<div class="span-18 last">
<?php
$this->widget('zii.widgets.grid.CGridView', array(
    'dataProvider' => $dataProvider,
    'selectableRows' => null,
    'columns' => array(
        
        array(
            'name' => 'id',
            'cssClassExpression' => '"column-right-align"',
        ),
        array(
            'name' => 'email_subject',
            'type'=>'raw',
            'value' => 'CHtml::link($data->email_subject, array(\'mail/view\', \'id\'=>$data->id))'
        ),
        array(
            'name' => 'recipient',
            'value' => '$data->recipient',
        ),        
        array(
            'name' => 'createTime',
            'type' => 'text',
            'value' => 'date("M j, Y H:i", $data->create_time)'
        ),
        array(
            'name' => 'sent_time',
            'type' => 'text',
            'value' => 'date("M j, Y H:i", $data->sent_time)',
        ),
        array(
            'header' => 'Status',
            'name' => 'status',
            'value' => '$data->getStatusStr()',
        ),     
        
    ),
));
?>
</div>

