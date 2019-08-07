<?php

return array(

	'showErrorSummary' => true,
	
	'attributes' => array(
        'enctype' => 'multipart/form-data',		
    ),
      
    'elements'=>array(
        'fileAttachment'=>array(
            'type'=>'file',            
        ),        
		'guid'=>array(
            'type'=>'hidden',
			'value'=>'tmp_'.MiscHelpers::GUID(),
        ),        
    ),
 
    'buttons'=>array(
        'upload'=>array(
            'type'=>'submit',
            'label'=>'Upload',
        ),
    ),
);