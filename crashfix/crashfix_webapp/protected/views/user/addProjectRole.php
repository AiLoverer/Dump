<?php
$this->breadcrumbs=array(
	'Administer'=>array('site/admin'), 
	'Users',
	CHtml::encode($user->username),
	'Add Project Roles'
);

?>

<div class="span-18 last">

    <div class="form">

    <div class="hint">
        Check the boxes next to the names of projects in which the user must 
        participate, and then click the Save button to apply your changes.
    </div>


    <?php $form=$this->beginWidget('CActiveForm', array(
        'id'=>'add-project-role-form',
        'enableAjaxValidation'=>false,
    )); ?>

    <?php echo CHtml::hiddenField('id', $user->id); ?>

    <!-- Grid view -->
    <div class="span-18 last">
    <?php     

        $models = UserGroup::model()->findAll(); 
        $list = CHtml::listData($models, 'id', 'name');

        $this->widget('zii.widgets.grid.CGridView', array(
        'dataProvider'=>$dataProvider,
        'selectableRows'=>null,
        'summaryText'=>'',
        'columns'=>array(		            
            array(                          
                'class'=>'CCheckBoxColumn',			  			  
                'id'=>'check',
                'selectableRows'=>2, // allow multiple rows to select                            
                'checked'=> function($data,$row) use ($user) {return $user->isInProject($data->id);}
            ),
            array(     
                'header'=>'Project Name',
                'name'=>'name',
                'type' => 'raw',			  
                'value' =>'CHtml::link($data->name, array(\'project/view\', \'id\'=>$data->id))',	  			  			  
            ),	
            array(     			  
                'header'=>'Role',
                'type'=>'raw',              
                'value'=>function($data,$row) use ($user, $list){ return CHtml::dropDownList("Project[".$data->id."][role]", array('selected'=>$user->suggestProjectRole($data->id)), $list); },              
                ),
        ),
    )); 
    ?>

    <?php $this->endWidget(); ?>

    <div class="row">
    <?php echo CHtml::submitButton('Save'); ?>
    </div>

    </div>
    
</div>