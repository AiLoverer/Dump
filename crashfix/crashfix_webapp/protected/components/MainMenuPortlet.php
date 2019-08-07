<?php

Yii::import('zii.widgets.CPortlet');
 
class MainMenuPortlet extends CPortlet
{   
	public $activeItem;
	
	public function init()
    {
        $this->title='Main Menu';		
		
        parent::init();		
    }
	
    protected function renderContent()
    {
        // Menu
					
		$this->widget('zii.widgets.CMenu', array(
			'activateItems'=>false,
			'items'=>array(
				array(
					'label'=>'Digest', 
					'url'=>array('site/index'), 
					'active'=>$this->activeItem=='Digest'
				),			        
				array(
					'label'=>'Crash Reports', 
					'url'=>array('crashReport/index'), 
					'active'=>$this->activeItem=='CrashReports',
					'visible'=>Yii::app()->user->checkAccess('pperm_browse_some_crash_reports')
				),			        
				array(
					'label'=>'Collections', 
					'url'=>array('crashGroup/index'), 
					'active'=>$this->activeItem=='CrashGroups',
					'visible'=>Yii::app()->user->checkAccess('pperm_browse_some_crash_reports')
				),			        
				array(
					'label'=>'Bugs', 
					'url'=>array('bug/index'), 
					'active'=>$this->activeItem=='Bugs', 
					'visible'=>Yii::app()->user->checkAccess('pperm_browse_some_bugs')
				),			        
				array(
					'label'=>'Debug Info', 
					'url'=>array('debugInfo/index'), 
					'active'=>$this->activeItem=='DebugInfo', 
					'visible'=>Yii::app()->user->checkAccess('pperm_browse_some_debug_info')
				),			        								
				array(
					'label'=>'Administer', 
					'url'=>array('site/admin'), 
					'active'=>$this->activeItem=='Administer', 
					'visible'=>Yii::app()->user->checkAccess('gperm_access_admin_panel')
				),
			),
		));
    }
}

