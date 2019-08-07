<?php

// Format email headers
$emailFrom = "no-reply@127.0.0.1";
$headers="From: {$emailFrom}\r\nReply-To: {$emailFrom}";	

return array(
    'mail1'=>array(
		'create_time'=>time(),
		'status'=>MailQueue::STATUS_PENDING,
		'recipient'=>'test@localhost',
		'email_subject'=>'Test Email Subject',
		'email_headers'=>$headers,
		'email_body'=>'Email body',
    ),	
);