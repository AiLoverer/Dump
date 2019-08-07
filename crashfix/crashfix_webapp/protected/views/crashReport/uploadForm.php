<?php

return array(

	'showErrorSummary'=>true,
	
	'attributes' => array(
        'enctype' => 'multipart/form-data',
    ),
      
    'elements'=>array(
        'fileAttachment'=>array(
            'type'=>'file',            
        ),        		
    ),
 
    'buttons'=>array(
        'upload'=>array(
            'type'=>'submit',
            'label'=>'Upload',
        ),
    ),
);