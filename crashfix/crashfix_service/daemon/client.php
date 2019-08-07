<?php

function getmicrotime() 
{ 
    list($usec, $sec) = explode(" ", microtime()); 
    return ((float)$usec + (float)$sec); 
}

function call_morph($host, $service_port, $request, &$responce)
{
  $address = gethostbyname($host);

  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

  if ($socket === false) 
  {
    echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
  } 

  $result = socket_connect($socket, $address, $service_port);

  if ($result === false) 
  {
    echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
  }

  $responce = socket_read($socket, 128);

  if($responce === "100 Ready.".chr(10))
  {
    $responce = "";
    socket_write($socket, $request, strlen($request));

    $buf = "";

    while ($buf = socket_read($socket, 2048)) 
    {
      $responce = $responce.$buf;
    }
  }

  socket_close($socket);

  list($errcode, $errmsg) = explode(" ", $responce); 

  return $errcode; 
} 


function test_morphd($host, $service_port, $request)
{
  echo "Sending request to server...\n";

  $time_start = getmicrotime();
  

  $request = $request."END".chr(10);
  $responce = '';

  echo $request; 
  $errcode = call_morph($host, $service_port, $request, $responce);

  $time_end = getmicrotime();
  $time = $time_end - $time_start;

  echo "Error code = $errcode\n";
  echo "Server responce = $responce\n";
  echo "Request completed in $time sec.\n";

  return $errcode;
}


error_reporting(E_ALL);

$path = "<path_to_samples>";

$request_arr = array(
"warp"            => "MORPH -warp ".$path."electra.jpg ".$path."electra.jpg.xml ".$path."arnold.jpg.xml ".$path."tmp/result_warp.jpg".chr(10),
"warp_lw"         => "MORPH -warp -line_warp ".$path."electra.jpg ".$path."electra.jpg.xml ".$path."arnold.jpg.xml ".$path."tmp/result_warp_lw.jpg".chr(10),
"warp_resize"     => "MORPH -warp -resize ".$path."electra.jpg ".$path."electra.jpg.xml ".$path."arnold.jpg.xml ".$path."tmp/result_warp_resize.jpg".chr(10),
"warp_resize_lw"  => "MORPH -warp -line_warp -resize ".$path."electra.jpg ".$path."electra.jpg.xml ".$path."arnold.jpg.xml ".$path."tmp/result_warp_resize_lw.jpg".chr(10),
"aver4"           => "MORPH -aver4 ".$path."electra.jpg ".$path."electra.jpg.xml 0.5 0.5 ".$path."arnold.jpg ".$path."arnold.jpg.xml "." 0.5 0.5 ".$path."tmp/result_aver.jpg  ".$path."tmp/result_aver.jpg.xml".chr(10),
"aver4_lw"        => "MORPH -aver4 -line_warp ".$path."electra.jpg ".$path."electra.jpg.xml 0.5 0.5 ".$path."arnold.jpg ".$path."arnold.jpg.xml "." 0.5 0.5 ".$path."tmp/result_aver.jpg ".$path."tmp/result_aver.jpg.xml".chr(10),
"aver4_resize"    => "MORPH -aver4 -resize ".$path."electra.jpg ".$path."electra.jpg.xml 0.5 0.5 ".$path."arnold.jpg ".$path."arnold.jpg.xml "." 0.5 0.5 ".$path."tmp/result_aver_resize.jpg  ".$path."tmp/result_aver.jpg.xml".chr(10),
"cari"            => "MORPH -cari ".$path."electra.jpg ".$path."electra.jpg.xml "." 2 0.5 ".$path."tmp/result_cari.jpg".chr(10),
"cari_lw"         => "MORPH -cari -line_warp ".$path."electra.jpg ".$path."electra.jpg.xml "." 2 0.5 ".$path."tmp/result_cari.jpg".chr(10),
"init2"           => "MORPH -init2 ".$path."electra.jpg ".$path."haarcascade_frontalface_alt2.xml ".$path."default.xml ".$path."tmp/result_init2.jpg.xml\n"
);


foreach ($request_arr as $id => $request) 
{
  $err_code = test_morphd("localhost", "6790", $request);
  if($err_code!=0 && $err_code!=102) 
  {
    die("Unexpected return code. Code 0 or 102 expected.");
  }
}       

echo "\nTest completed successfuly.\n";

?>
