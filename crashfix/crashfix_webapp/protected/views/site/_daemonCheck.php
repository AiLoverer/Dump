<div id="daemon-check" style="display:block">

<?php if($retCode==Daemon::DAEMON_CHECK_INACTIVE): ?>    
    
    <h2>CrashFix Service is Inactive</h2>

    <p>
        The CrashFix background service (daemon) seems to be inactive or not responding. 
        Some features will be disabled.			
    </p>

    <p>This may be by any of the following reasons:</p>
    <ul>			
        <li>The service may not have been started.</li>
        <li>The service may has stopped because of a critical error.</li>			
    </ul>
    
    <p>Please contact the administrator to fix this problem.</p>

<?php elseif($retCode==Daemon::DAEMON_CHECK_EXTENSION_MISSING): ?>    
    
    <h2>CrashFix Web Application Configuration Error</h2>

    <p>
        The CrashFix web application's configuration is incorrect. PHP extension 
        <i>sockets</i> is not installed. Some features will be disabled.
    </p>
    
    <p>
        Recommended solution:
    </p>
    
    <p>
        PHP extension <i>sockets</i> is required for communication between
        CrashFix service and web application, but it is not installed.
        Please modify your <i>php.ini</i> file and enable the <i>sockets</i> extension.
    </p>
    
    <p>Please contact the administrator to fix this problem.</p>
    
<?php elseif($retCode==Daemon::DAEMON_CHECK_CONFIG_ERROR || 
             $retCode==Daemon::DAEMON_CHECK_VER_MISMATCH): ?>    
    
<h2>CrashFix Service Configuration Error</h2>

    <p>
        CrashFix service's version and CrashFix web application version
        are not the same. Some features will be disabled.
    </p>

    <p>
        Recommended solution:
    </p>
    <p>
        CrashFix components should have the same version to be able to 
        communicate correctly.
        Please install the correct version of CrashFix service (daemon).        
    </p>
    
    <p>Please contact the administrator to fix this problem.</p>
    
<?php elseif($retCode==Daemon::DAEMON_CHECK_BAD_WEB_ROOT_DIR): ?>    
    
<h2>CrashFix Service Configuration Error</h2>

    <p>
        CrashFix service's configuration file is not correct. <i>WEB_ROOT_DIR</i>
        parameter does not point to this web application's installation directory.
        Some features will be disabled.
    </p>
    
    <p>
        Recommended solution:
    </p>

    <ul>
        <li>Ensure that the absolute path to <i>php.exe</i> is on your <i>PATH</i> 
            environment variable</i>
        <li>Edit <i>crashfixd.conf</i> file and specify the correct 
        value for <i>WEB_ROOT_DIR</i> parameter. 
    </ul>
    
<?php endif; ?>   
        
    <center><input type="button" id="btn_close" value="Close"></center>  
</div>

