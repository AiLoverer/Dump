<?php

require_once('common.php');

// This is the configuration for yiic console application.
// Any writable CConsoleApplication properties can be configured here.
return array(
	'basePath'=>dirname(__FILE__).DIRECTORY_SEPARATOR.'..',
	'name'=>'CrashFix',
	
	// preloading 'log' component
	'preload'=>array('log'),
	
	// autoloading model and component classes
	'import'=>array(
		'application.models.*',
		'application.components.*',
	),
	
	// application components
	'components'=>array(
		
		'db'=>dbParams(),		
		'dbTest'=>dbParams('test_'),
				
		'daemon'=>array(
			'class'=>'Daemon',
			'host'=>'127.0.0.1',
			'servicePort'=>'50',
		),		
		
		'log'=>array(
			'class'=>'CLogRouter',
			'routes'=>array(
					array(
						'class'=>'CFileLogRoute',
						'levels'=>'error, warning, info',
						'logFile'=>'console.log',
						'maxFileSize'=>5*1024,
						'maxLogFiles'=>10,
					),
			),				
		),		
	),
    'params'=>getCommonParams(),
);