<?php

require_once('common.php');

return CMap::mergeArray(
	require(dirname(__FILE__).'/main.php'),	
	array(
	
		// preloading 'fixture' component
		// preloading 'log' component
		'preload'=>array('fixture', 'log'),
				
		'components'=>array(
		
			'fixture'=>array(
				'class'=>'system.test.CDbFixtureManager',			
				'connectionID'=>'db',				
			),
			
			'db'=>dbParams('test_'),
			
			'log'=>array(
                            'class'=>'CLogRouter',
                            'routes'=>array(
                                    array(
                                            'class'=>'CFileLogRoute',
                                            'levels'=>'error, warning, info',
                                    ),				
			),
		),
		),
	)
);
