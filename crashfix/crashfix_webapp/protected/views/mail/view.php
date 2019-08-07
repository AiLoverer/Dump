<?php
$this->pageTitle = Yii::app()->name . ' - Browse Outgoing Mail';

$this->breadcrumbs = array(
    'Administer',
    'Mail' => array('mail/index'),
    'View Mail #' . CHtml::encode($model->id));
?>

<!-- Flash messages -->
<div class="span-18 last">
<?php
foreach (Yii::app()->user->getFlashes() as $key => $message) {
    echo '<div class="flash-' . $key . '">' . $message . "</div>\n";
}
?>
</div>

<!-- Actions Toolbar -->
<div class="span-18 last">
    <div class="div_actions">

        <?php echo CHtml::form(); ?>		

        <?php echo CHtml::hiddenField('id', $model->id); ?>			

        <?php
        
            if($model->status!=MailQueue::STATUS_PENDING)
            {
                echo CHtml::linkButton('Resend Mail', array(
                'submit' => Yii::app()->createUrl('mail/resetStatus'),
                'confirm' => "Are you sure you want to reset status of mail #" . $model->id . "?"
                    )
            );
            }
        
            echo CHtml::linkButton('Delete Mail', array(
                'submit' => Yii::app()->createUrl('mail/delete'),
                'confirm' => "Are you sure you want to permanently delete mail #" . $model->id . "?"
                    )
            );
        ?>	

        <?php echo CHtml::endForm(); ?>		
    </div>	
</div>


<!-- Detail View -->
<div class="span-18 last">
<?php
$this->widget('zii.widgets.CDetailView', array(
    'data' => $model,
    'attributes' => array(
        'id',
        'recipient',
        'email_subject',
        array(
            'name' => 'status',
            'value' => $model->getStatusStr()
        ),
    ),
));
?>

<textarea id="body" readonly="readonly" rows="30" cols="82">
<?php echo CHtml::encode($model->email_body); ?>
</textarea>
</div>



