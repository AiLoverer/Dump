<?php

return array(
    'root'=>array(
        'username'=>'root',
		'usergroup'=>1,
		'password'=>'7dcfe0dd0af7bf680e0ae5e410ac02ee',
		'salt'=>'e8wqopb0guk8ryhbargqzfahmqaz5td8',
		'flags'=>3,
		'status'=>User::STATUS_ACTIVE,
		'flags'=>1,
		'email'=>'test@localhost',
        'cur_project_id'=>1,
        'cur_appversion_id'=>1
    ),
	
	'devel'=>array(
        'username'=>'devel',
		'usergroup'=>2,
		'password'=>'7dcfe0dd0af7bf680e0ae5e410ac02ee',
		'salt'=>'e8wqopb0guk8ryhbargqzfahmqaz5td8',		
		'status'=>User::STATUS_ACTIVE,
		'flags'=>0,
		'email'=>'devel@localhost',
		'pwd_reset_token'=>'6c9016a509b565a0eb9a4d6c5fd7be0c',
    ),
    
	'guest'=>array(
        'username'=>'guest',
		'usergroup'=>4,
		'password'=>'7dcfe0dd0af7bf680e0ae5e410ac02ee',
		'salt'=>'e8wqopb0guk8ryhbargqzfahmqaz5td8',		
		'status'=>User::STATUS_ACTIVE,
		'flags'=>0,
		'email'=>'guest@localhost',
		'pwd_reset_token'=>'6c9016a509b565a0eb9a4d6c5fd7be0c',
    ),
	
	'guest2'=>array(
        'username'=>'guest2',
		'usergroup'=>4,
		'password'=>'7dcfe0dd0af7bf680e0ae5e410ac02ee',
		'salt'=>'e8wqopb0guk8ryhbargqzfahmqaz5td8',		
		'status'=>User::STATUS_ACTIVE,
		'flags'=>0,
		'email'=>'guest2@localhost',
		'pwd_reset_token'=>'6c9016a509b565a0eb9a4d6c5fd7be0c',
    ),
);