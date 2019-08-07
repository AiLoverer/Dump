<?php

Yii::import('zii.widgets.CPortlet');
 
class AdminMenuPortlet extends CPortlet
{   
	public $activeItem;
	
	public function init()
    {	
        parent::init();		
    }
	
    protected function renderContent()
    {
        // Menu
					
		$this->widget('zii.widgets.CMenu', array(
			'activateItems'=>false,
			'items'=>array(				
				array('label'=>'General', 'url'=>array('site/admin'), 'active'=>$this->activeItem=='General'),
				array('label'=>'Users', 'url'=>array('user/index'), 'active'=>$this->activeItem=='Users'),
				array('label'=>'Groups', 'url'=>array('userGroup/index'), 'active'=>$this->activeItem=='Groups'),
				array('label'=>'Projects', 'url'=>array('project/index'), 'active'=>$this->activeItem=='Projects'),
				array('label'=>'Daemon', 'url'=>array('site/daemon'), 'active'=>$this->activeItem=='Daemon'),		
                array('label'=>'Mail', 'url'=>array('mail/index'), 'active'=>$this->activeItem=='Mail'),		
			),			
		));
    }
}


