<?php
$this->pageTitle=Yii::app()->name . ' - Browse Bugs';
$this->breadcrumbs = array(
    'Bugs',
);
?>

<?php if (count(Yii::app()->user->getMyProjects()) == 0): ?>

    You have no projects assigned.

<?php else: ?>

    <!-- Project Selection Form -->
    <div class="span-27 last" id="div_proj_selection">	
        <?php echo CHtml::beginForm(array('site/setCurProject'), 'get', array('id' => 'proj_form')); ?>
        <div class="span-27 last">
            Current Project:
            <?php
                $models = Yii::app()->user->getMyProjects();
                $projects = CHtml::listData($models, 'id', 'name');
                echo CHtml::dropDownList('proj', array('selected' => Yii::app()->user->getCurProjectId()), $projects);
            ?>					
            Version:
            <?php
                $selVer = -1;
                $versions = Yii::app()->user->getCurProjectVersions($selVer);
                echo CHtml::dropDownList('ver', array('selected' => $selVer), $versions);
            ?>					
        </div>
        <?php echo CHtml::endForm(); ?>
    </div>

    <!-- Simple Search Form -->
    <div class="span-27 last" id="div_simple_search" style="display:<?php echo!$model->isAdvancedSearch ? 'inherit' : 'none' ?>">	
        <?php echo CHtml::beginForm('index', 'get', array('id' => 'proj_form')); ?>
        <div class="span-27 last"><p id="stat_filter">Search by Symmary/Reporter/Owner or use <a href="#" id="link_advanced_search">advanced search</a>:</p></div>
        <div class="span-6">
            <div id="dropdown_search">
                <?php
                $bug_statuses = array(
                    'all' => 'All bugs',
                    'open' => 'Open bugs',
                    'owned' => 'Open and owned by me',
                    'reported' => 'Open and reported by me',
                    'verify' => 'Bugs to verify',
                    'closed' => 'Closed bugs',
                );
                echo CHtml::dropDownList('status', array('selected' => isset($model->bugStatusFilter) ? CHtml::encode($model->bugStatusFilter) : "open"), $bug_statuses);
                ?>
            </div>
        </div>
        <div class="span-19">
            <?php echo CHtml::textField('q', isset($model->filter) ? CHtml::encode($model->filter) : "", array('id' => 'text_filter')); ?>		
        </div>
        <div class="span-2 last">
            <?php echo CHtml::submitButton('Search', array('id' => 'btn_filter')); ?>
        </div>	
        <?php echo CHtml::endForm(); ?>
    </div>

    <!-- Advanced Search Form -->
    <div class="span-27 last" id="div_advanced_search" style="display:<?php echo $model->isAdvancedSearch ? 'inherit' : 'none' ?>">	
        <div class="wide form" id="form_advanced_search">
            <?php echo CHtml::beginForm('index', 'post'); ?>

            <h4>Advanced Search:</h4>
            <?php echo CHtml::errorSummary($model); ?>
            <div class="row">
                <?php echo CHtml::activeLabel($model, 'summary'); ?>
                <?php echo CHtml::activeTextField($model, 'summary', array('value' => $model->summary));
                ?>		
                <?php echo CHtml::error($model, 'summary'); ?>
            </div>	
            <div class="row">
                <?php echo CHtml::activeLabel($model, 'status'); ?>	
                <?php
                $statuses = Lookup::items('BugStatus');
                $statuses = array(-1 => '<any>') + $statuses;
                echo CHtml::dropDownList('Bug[status]', array('selected' => $model->status), $statuses);
                ?>
            </div>
            <div class="row">
                <?php echo CHtml::activeLabel($model, 'priority'); ?>
                <?php
                $items = Lookup::items('BugPriority');
                $items = array(-1 => '<any>') + $items;
                echo CHtml::activeDropDownList($model, 'priority', $items);
                ?>
                <?php echo CHtml::error($model, 'priority'); ?>
            </div>	
            <div class="row">
                <?php echo CHtml::activeLabel($model, 'date_created'); ?>	
                From:
                <?php
                $this->widget('zii.widgets.jui.CJuiDatePicker', array(
                    'name' => 'Bug[createdFrom]',
                    'value' => $model->createdFrom,
                    // additional javascript options for the date picker plugin
                    'options' => array(
                        'showAnim' => 'fold',
                    ),
                    'htmlOptions' => array(
                    //'style'=>'height:20px;'
                    ),
                ));
                ?>
                To:
                <?php
                $this->widget('zii.widgets.jui.CJuiDatePicker', array(
                    'name' => 'Bug[createdTo]',
                    'value' => $model->createdTo,
                    // additional javascript options for the date picker plugin
                    'options' => array(
                        'showAnim' => 'fold',
                    ),
                    'htmlOptions' => array(
                    //'style'=>'height:20px;'
                    ),
                ));
                ?>	
            </div>
            <div class="row">
                <?php echo CHtml::activeLabel($model, 'reported_by'); ?>
                <?php echo CHtml::activeTextField($model, 'reported_by', array('value' => $model->reported_by));
                ?>		
                <?php echo CHtml::error($model, 'reported_by'); ?>
            </div>	
            <div class="row">
                <?php echo CHtml::activeLabel($model, 'assigned_to'); ?>
                <?php echo CHtml::activeTextField($model, 'assigned_to', array('value' => $model->assigned_to));
                ?>		
                <?php echo CHtml::error($model, 'assigned_to'); ?>
            </div>	
            <div class="row buttons">
                <?php echo CHtml::submitButton('Search', array('id' => 'btn_filter')); ?>
            </div>

            <?php echo CHtml::endForm(); ?>
        </div>
    </div>

    <!-- Actions Toolbar -->
    <div class="span-27 last">
        <div class="div_actions">				
            <?php
            if (Yii::app()->user->checkAccess('pperm_manage_bugs', array('project_id'=>Yii::app()->user->getCurProjectId()))) {
                echo CHtml::link('Open New Bug', $this->createUrl('bug/create'));
            }
            ?>	
        </div>	
    </div>

            
    <div class="span-27 last">
        <?php
        $this->widget('zii.widgets.grid.CGridView', array(
            'dataProvider' => $dataProvider,
            'selectableRows' => null,
            'columns' => array(
                array(            
                    'class'=>'CCheckBoxColumn',			  			  
                    'id'=>'DeleteRows',
                    'selectableRows'=>2, // allow multiple rows to select
                ),    
                array(
                    'name' => 'id',
                    'type' => 'raw',
                    'value' => 'CHtml::link($data->id, \'view/\'.$data->id)',
                    'cssClassExpression' => '"column-right-align"',
                ),
                array(
                    'name' => 'status',
                    'value' => 'Lookup::item(\'BugStatus\', $data->status)',
                ),
                array(
                    'name' => 'priority',
                    'value' => 'Lookup::item(\'BugPriority\', $data->priority)',
                ),
                array(
                    'name' => 'date_created',
                    'value' => 'date("d/m/y H:i", $data->date_created)',
                ),
                array(
                    'name' => 'reported_by',
                    'value' => '$data->reporter->username',
                ),
                array(
                    'name' => 'assigned_to',
                    'value' => '$data->owner?$data->owner->username:""',
                ),
                array(
                    'header' => 'Repro',
                    'name' => 'reproducability',
                    'value' => 'Lookup::item(\'BugReproducability\', $data->reproducability)',
                ),
                array(
                    'name' => 'summary',
                    'type' => 'text',
                    'value' => 'MiscHelpers::addEllipsis($data->summary, 100)',
                ),
            ),
        ));
        ?>
    </div>

    <div class="span-27 last footnote">
        <?php
        $curVer = "";
        $totalFileSize = 0;
        $percentOfQuota = 0;
        $count = Yii::app()->user->getCurProject()->getBugAttachmentCount(
                $totalFileSize, $percentOfQuota);
        $totalFileSizeStr = MiscHelpers::fileSizeToStr($totalFileSize);
        $percentOfQuotaStr = sprintf("%.0f", $percentOfQuota);
        echo "This project contains total $totalFileSizeStr in $count bug file attachment(s)";
        if($percentOfQuota>=0)
            echo " ($percentOfQuotaStr% of disc quota).";
        ?> 	
    </div>

    <?php
    $script = <<<SCRIPT

$("#proj, #ver").bind('change', function(e)
{	
	$("#proj_form").submit();
});

$("#link_advanced_search").bind('click', function(e)
{	
	$("#div_simple_search").fadeToggle("fast");			
	$("#div_advanced_search").fadeToggle("fast");		
});
SCRIPT;

    Yii::app()->getClientScript()->registerScript(
            "Bug", $script, CClientScript::POS_READY);
    ?>


<?php endif; ?>