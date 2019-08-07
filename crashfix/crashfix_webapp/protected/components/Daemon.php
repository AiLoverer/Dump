<?php

/**
 * The Daemon class is used for interaction with crashfix daemon.
 */
class Daemon extends CApplicationComponent
{
	public $host;        // Service host.
	public $servicePort; // Service port.
	public $errorMsg;    // Error message.
	
    const DAEMON_CHECK_OK                = 0; // Daemon check OK.
    const DAEMON_CHECK_INACTIVE          = 1; // Daemon seems to be inactive or not responding.
    const DAEMON_CHECK_EXTENSION_MISSING = 2; // Some PHP extension is missing.
    const DAEMON_CHECK_CONFIG_ERROR      = 3; // Error getting configuration info.    
    const DAEMON_CHECK_VER_MISMATCH      = 4; // Version mismatch.    
    const DAEMON_CHECK_BAD_WEB_ROOT_DIR  = 5; // WEB_ROOT_DIR parameter is incorrect.    
    const DAEMON_CHECK_STATUS_ERROR      = 6; // Daemon reports some problems.    
    
    
	public function init()
	{
		return parent::init();
	}

    /**
	 * Checks if daemon works and responds on requests. It actually
     * does a real check once in a few minutes; more frequent calls
     * just return saved previous check status.
	 * @param boolean $realCheck On output, receives true if a real check was made, otherwise false.
     * @param string $errorMsg On output, returns error message.
	 * @return integer Daemon check status.
	 */
	public function checkDaemon(&$realCheck, &$errorMsg)
	{        
		// Check session data
		$sessionDataExist = isset(Yii::app()->session['lastDaemonCheckTime']) &&
		   isset(Yii::app()->session['lastDaemonCheckStatus']);
		if($sessionDataExist)
		{
			$lastCheckTime = Yii::app()->session['lastDaemonCheckTime'];
			$lastCheckStatus = Yii::app()->session['lastDaemonCheckStatus'];
			
			// Compare current time with the time we've last checked the daemon
			$curTime=time();
			if($curTime-$lastCheckTime<=5*60) // 5 min
			{
				// Daemon has been already checked recently.					
				$realCheck=false;
				return $lastCheckStatus; 
			}
		}	
		
		// The daemon should be checked now		
		$realCheck=true;
        $retCode = self::DAEMON_CHECK_OK;
        $errorMsg = 'Unspecified error';
		
		// Save current time in session data
		Yii::app()->session['lastDaemonCheckTime'] = time();
		
        // Check if php_sockets extension presents
        if(!extension_loaded('sockets'))
        {
            $retCode = self::DAEMON_CHECK_EXTENSION_MISSING;
            $errorMsg = 'PHP sockets extension is disabled';
        }
        else
        {
            // Check daemon's status
            $responce = "";
            $result = Yii::app()->daemon->getDaemonStatus($responce);        
            if($result<0)
            {
                $retCode = self::DAEMON_CHECK_INACTIVE;
                $errorMsg = 'Daemon seems to be inactive or not';
            }
            else
            {
                // Check daemon's configuration            
                $configInfo = Yii::app()->daemon->getConfigInfo();        
                if(!is_array($configInfo))
                {
                    $retCode = self::DAEMON_CHECK_CONFIG_ERROR;
                    $errorMsg = 'Error getting daemon configuration info';                
                }
                else
                {
                    if($configInfo['DaemonVer']!=Yii::app()->params['version'])
                    {
                        $retCode = self::DAEMON_CHECK_VER_MISMATCH;
                        $errorMsg = 'Daemon version and web application version are not the same'; 
                    }
                    else
                    {
                        $webRootDir = $configInfo['WebRootDir'].'protected';
                        $webRootDir = str_replace('\\', '/', $webRootDir);

                        $curDir = Yii::app()->basePath;
                        $curDir = str_replace('\\', '/', $curDir);
                        if($curDir!=$webRootDir)
                        {
                            $retCode = self::DAEMON_CHECK_BAD_WEB_ROOT_DIR;
                            $errorMsg = 'Daemon configuration is incorrect (WEB_ROOT_DIR parameter does not point to this web application\'s installation directory)';                
                        }                              
                    }               
                }
            }                       
        }
        
        // Save ret code to session data
        Yii::app()->session['lastDaemonCheckStatus'] = $retCode;		
        return $retCode;		        
	}
	
    
	/**
	 * Gets daemon status message(s).
	 * @param string $status On return, receives daemon status strings separated with ';'
	 * @return integer Error code. 
	 */
	public function getDaemonStatus(&$status)
	{
		return $this->execCommand('daemon status', $status);
	}
	
	/**
	 * Retrieves license information from daemon. 
	 * @return array License information or False on error.
	 */
	public function getLicenseInfo()
	{
		$licenseInfo = array();
		
		// Create temp file to write license info to
		$outFile = tempnam(Yii::app()->getRuntimePath(), "li");
		
		// Format command line for daemon request
		$cmdLine = 'daemon get-license-info "'.$outFile.'"';
		
		// Execute request
		$daemonResponce = "";
		$daemonRetCode = Yii::app()->daemon->execCommand($cmdLine, $daemonResponce);
		
		try
		{		
			// Check return code
			if($daemonRetCode!=0)
				throw new Exception('Daemon request has failed'); 
		
			// Load XML from temp file
			$doc = simplexml_load_file($outFile);
			if($doc==Null)
				throw new Exception('Invalid XML document '.$outFile);
			
			// Extract license info from XML file
			$elemGeneral = $doc->General;
			if($elemGeneral==Null)
				throw new Exception('Not found General element in XML document '.$outFile);		
						
			$licenseInfo['DateCreated'] = date('d M Y', strtotime($elemGeneral->DateCreated));
			$licenseInfo['DaysExpiresFromNow'] = $elemGeneral->DaysExpiresFromNow;
			$licenseInfo['Evaluation'] = $elemGeneral->Evaluation;
			
			$elemProductInfo = $doc->ProductInfo;
			if($elemProductInfo==Null)
				throw new Exception('Not found ProductInfo element in XML document '.$outFile);					
				
			$licenseInfo['ProductName'] = $elemProductInfo->ProductName;
			$licenseInfo['ProductVersion']= $elemProductInfo->ProductVersion;
			$licenseInfo['Edition'] = $elemProductInfo->Edition;
								
			$elemCustomerInfo = $doc->CustomerInfo;
			if($elemCustomerInfo==Null)
				throw new Exception('Not found CustomerInfo element in XML document '.$outFile);					
			
			$licenseInfo['Name'] = $elemCustomerInfo->Name;
			$licenseInfo['Surname'] = $elemCustomerInfo->Surname;
			$licenseInfo['CompanyName'] = $elemCustomerInfo->CompanyName;			
			$licenseInfo['Country'] = $elemCustomerInfo->Country;			
			$licenseInfo['State'] = $elemCustomerInfo->State;			
			$licenseInfo['City'] = $elemCustomerInfo->City;			
			$licenseInfo['PostalCode'] = $elemCustomerInfo->PostalCode;			
			$licenseInfo['Address'] = $elemCustomerInfo->Address;			
		}
		catch(Exception $e)
		{
			Yii::log($e->getMessage(), 'error');
			$licenseInfo=False;
		}
		
		// Delete temp file		
		@unlink($outFile);
		
		return $licenseInfo;
	}
    
    /**
	 * Retrieves configuration information from daemon. 
	 * @return array Config information or False on error.
	 */
	public function getConfigInfo()
	{
		$configInfo = array();
		
		// Create temp file to write to
		$outFile = tempnam(Yii::app()->getRuntimePath(), "ci");
		
		// Format command line for daemon request
		$cmdLine = 'daemon get-config-info "'.$outFile.'"';
		
		// Execute request
		$daemonResponce = "";
		$daemonRetCode = Yii::app()->daemon->execCommand($cmdLine, $daemonResponce);
		
		try
		{		
			// Check return code
			if($daemonRetCode!=0)
				throw new Exception('Daemon request has failed'); 
		
			// Load XML from temp file
			$doc = simplexml_load_file($outFile);
			if($doc==Null)
				throw new Exception('Invalid XML document '.$outFile);
			
			// Extract info from XML file
			$elemGeneral = $doc->Daemon;
			if($elemGeneral==Null)
				throw new Exception('Not found General element in XML document '.$outFile);		
			
            $configInfo['DaemonVer'] = (string)$elemGeneral->DaemonVer;			
			$configInfo['WebRootDir'] = (string)$elemGeneral->WebRootDir;		
		}
		catch(Exception $e)
		{
			Yii::log($e->getMessage(), 'error');
			$configInfo=False;
		}
		
		// Delete temp file		
		@unlink($outFile);
		
		return $configInfo;
	}
	
	/**
	 * Executes a command to daemon and reads responce
	 * @param string $command
	 * @param string $responce
	 * @return integer Error code.  
	 */
	public function execCommand($command, &$responce)
	{		
		return $this->execCommands(array($command), $responce);
	}
	
	/**
	 * Executes a sequence of commands.
	 * @param array $commands The list of commands to execute.
	 * @param string $responce Server responce string.
	 * @return integer Error code. 
	 * @throws type 
	 */
	public function execCommands($commands, &$responce)
	{
		// Validate input
		if(!is_array($commands))
			throw Exception('Invalid parameter (array expected).');
		
		// Format a request string
		$request = "";
		
		foreach($commands as $command)
		{
			// Separate commands with ';'
			$request = $request.$command.';';			
		}
		
		// Append '\n' terminator character
		$request = $request.chr(10);		
			
		// Send request
		return $this->sendRequest($request, $responce);
	}
	
	/**
	 * Executes sequence of commands to daemon and reads responce
	 * @param string $request  Request string.
	 * @param string $responce Responce string.
	 * @return integer Return code.
	 */
	private function sendRequest($request, &$responce)
	{
		$responce = "";
		$errorCode = -1;		
				
		// Create socket
		$sock = @socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($sock === FALSE) 
		{
			$responce = "Error creating connection to daemon.";
			return -1;
		} 
		
		// Connect socket	
		$result = @socket_connect($sock, '127.0.0.1', '50');
		if( FALSE === $result ) 
		{
			$responce = "Error connecting to daemon.";
			return -1;
		}		
		
		// Read greeting message from daemon
		$responce = socket_read($sock, 128);
		if($responce === "100 Accepting requests.".chr(10))
		{
			$responce = "";
			// Send request data
			socket_write($sock, $request, strlen($request));

			$buf = "";

			// Read responce
			while ($buf = socket_read($sock, 2048)) 
			{
				$responce = $responce.$buf;
			}			
		}
	
		// Close socket
		socket_close($sock);

		// Get error code and error message
		$errorCode = trim(strstr($responce, " ", true)); 	
		$responce = trim(strstr($responce, " "));
		if(strlen($errorCode)<=0)
		{
			$errorCode = -1;			
		}
				
		return $errorCode; 
	} 	
};