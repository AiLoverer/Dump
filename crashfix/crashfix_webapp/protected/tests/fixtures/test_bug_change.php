<?php

return array(
    'change1'=>array(
        'bug_id'=>1,
		'timestamp'=>time()-1,
		'user_id'=>1,
		'flags'=>BugChange::FLAG_INITIAL_CHANGE,
		'status_change_id'=>1,
		'comment_id'=>1,
    ),	
	
	'change2'=>array(
        'bug_id'=>1,
		'timestamp'=>time(),
		'user_id'=>1,
		'flags'=>0,		
    ),	
);