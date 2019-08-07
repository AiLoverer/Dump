<?php

return array(
    'bug1'=>array(
        'date_created'=>123456789,
		'date_last_modified'=>123456789,
		'project_id'=>1,
		'appversion_id'=>1,
		'status'=>Bug::STATUS_ACCEPTED,
		'summary'=>'Bug on crash report #1',
		'description'=>'Some description',
		'reported_by'=>1,
		'assigned_to'=>1,
		'priority'=>Bug::PRIORITY_CRITICAL,
		'reproducability'=>Bug::REPRO_ALWAYS,		
    ),
	
	'bug2'=>array(
        'date_created'=>12456789,
		'date_last_modified'=>12456789,
		'project_id'=>2,
		'appversion_id'=>4,
		'status'=>Bug::STATUS_FIXED,
		'summary'=>'Bug on crash report #1',
		'description'=>'Some description',
		'reported_by'=>1,		
		'priority'=>Bug::PRIORITY_MEDIUM,
		'reproducability'=>Bug::REPRO_NOT_TRIED,		
    ),
	
);