<?php
$this->breadcrumbs=array(
	'Administer', 
	'Projects'=>array('project/index'),
	CHtml::encode($project->name),
	'Add Project Members'
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

        <?php echo CHtml::hiddenField('id', $project->id); ?>

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
                    'checked'=> function($data,$row) use ($project) {return $data->isInProject($project->id);}
                ),
                array(     
                    'header'=>'User Name',
                    'name'=>'username',
                    'type' => 'raw',			  
                    'value' =>'CHtml::link($data->username, array(\'user/view\', \'id\'=>$data->id))',	  			  			  
                ),	
                array(     
                    'name'=>'usergroup',
                    'header'=>'Role',
                    'type'=>'raw',
                    'id'=>'User[][usergroup]',
                    'value'=>function($data,$row) use ($list, $project){ return CHtml::dropDownList("User[".$data->id."][usergroup]", array('selected'=>$data->suggestProjectRole($project->id)), $list); },              
                    ),
            ),
        )); 
        ?>

        <?php echo CHtml::submitButton('Save', array('id'=>'add_role')); ?>

        <?php $this->endWidget(); ?>

        </div>
        
    </div>
</div>