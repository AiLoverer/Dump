<?php

function getmicrotime() 
{ 
    list($usec, $sec) = explode(" ", microtime()); 
    return ((float)$usec + (float)$sec); 
}

function call_daemon($host, $service_port, $request, &$responce)
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


function test_daemon($host, $service_port, $request)
{
  echo "Sending request to server...\n";

  $time_start = getmicrotime();
  

  $request = $request."END".chr(10);
  $responce = '';

  echo $request; 
  $errcode = call_daemon($host, $service_port, $request, $responce);

  $time_end = getmicrotime();
  $time = $time_end - $time_start;

  echo "Error code = $errcode\n";
  echo "Server responce = $responce\n";
  echo "Request completed in $time sec.\n";

  return $errcode;
}


error_reporting(E_ALL);

$err_code = test_daemon("localhost", "50", "daemon status\n");
$err_code = test_daemon("localhost", "50", "dumper --dump-crash-report D:\\projects\\dumper\\dumper\\trunk\\test_data\\crashreport.zip D:\\projects\\dumper\\dumper\\trunk\\bin\\out2.txt D:\\projects\\dumper\\dumper\\trunk\\test_data\\\n");

?>
