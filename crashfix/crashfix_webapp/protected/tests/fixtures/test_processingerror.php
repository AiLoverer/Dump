<?php

return array(
    'err1'=>array( 
		'type'=>ProcessingError::TYPE_CRASH_REPORT_ERROR,
		'srcid'=>1,
		'message'=>'Error saving crash report',		
    ),	
	
	'err2'=>array( 
		'type'=>ProcessingError::TYPE_DEBUG_INFO_ERROR,
		'srcid'=>1,
		'message'=>'Unsupported architecture',		
    ),
);