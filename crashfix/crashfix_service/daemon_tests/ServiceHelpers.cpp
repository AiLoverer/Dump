//! \file ServiceHelpers.h
//! \brief Contains daemon starting\stopping\restarting helper functions.
//! \author Oleg Krivtsov
//! \date Mar 2012

#include "stdafx.h"
#include "ServiceHelpers.h"

std::string SystemCmd(const char* szCmd)
{
#ifndef _WIN32
    // Execute command
    FILE * fp = NULL;
    char res[1024] = "";
    fp = popen(szCmd,"r");
    if(fp==NULL)
        return std::string("Error"); // Error executing command

    // Read response
    int nBytes = fread(res, 1, 1023, fp);
    pclose(fp);
    if(nBytes==0)
        return std::string();

    return std::string(res);
#endif

	return std::string();
}

bool StartDaemon(std::string& sErrorMsg)
{
    sErrorMsg = "Unspecified error.";

#ifdef _WIN32

	return true;

#else

    // First check that the daemon process in not running
    std::string sResponse = SystemCmd("pgrep crashfixd");
    if(!sResponse.empty())
    {
        sErrorMsg = "The daemon process is already running!";
        return false;
    }

    // Start daemon using service shell command
    sResponse = SystemCmd("service crashfixd start 2>&1");
    if(sResponse.find("Forked the CrashFix daemon process")==std::string::npos)
    {
        // Invalid responce
        sErrorMsg = "Error starting the daemon; invalid responce from service command.";
        return false;
    }

    // Check that process ID exists
    sResponse = SystemCmd("pgrep crashfixd");
    if(sResponse.empty())
    {
        sErrorMsg = "Error starting daemon; after the service command, the daemon process doesn't exist.";
        return false; // The daemon process doesn't exist
    }

    // Success
    sErrorMsg = "Success.";
	return true;

#endif

}

bool StopDaemon(std::string& sErrorMsg)
{
    sErrorMsg = "Unspecified error.";

#ifdef _WIN32

    return true;

#else

    // First check that the daemon process in running
    std::string sResponse = SystemCmd("pgrep crashfixd");
    if(sResponse.empty())
    {
        sErrorMsg = "Error stopping the daemon: the daemon process is not running!";
        return false; // The daemon process is not running!
    }

    // Stop daemon using service shell command
    sResponse = SystemCmd("service crashfixd stop 2>&1");
    if(sResponse.find("CrashFix service has been stopped OK")==std::string::npos)
    {
        sErrorMsg = "Error stopping the daemon; invalid responce from service command.";
        return false; // Invalid responce
    }

    // Check that process ID doesn't exist
    sResponse = SystemCmd("pgrep crashfixd");
    if(!sResponse.empty())
    {
        sErrorMsg = "Error stopping the daemon; the daemon process still exists.";
        return false; // The daemon process still exists
    }

    // Success
    sErrorMsg = "Success";
	return true;

#endif
}

bool RestartDaemon(std::string& sErrorMsg)
{
#ifdef _WIN32

    return true;

#else

    // First check that the daemon process in running
    std::string sResponse = SystemCmd("pgrep crashfixd");
    if(sResponse.empty())
        return false; // The daemon process is not running!

    // Restart daemon using service shell command
    sResponse = SystemCmd("service crashfixd restart 2>&1");
    if(sResponse.find("CrashFix service has been stopped OK")==std::string::npos ||
       sResponse.find("Forked the CrashFix daemon process")==std::string::npos)
        return false; // Invalid responce

    // Check that process ID exists
    sResponse = SystemCmd("pgrep crashfixd");
    if(sResponse.empty())
        return false; // The daemon process doesn't exist

    // Success
	return true;

#endif

	return true;
}

bool KillDaemon(std::string& sErrorMsg)
{
    sErrorMsg = "Unspecified error.";
    char szBuf[256]="";

#ifdef _WIN32

    return true;

#else

    // First check that the daemon process in running
    std::string sResponse = SystemCmd("pgrep crashfixd");
    if(sResponse.empty())
    {
        sErrorMsg = "Error killing the daemon: the daemon process is not running!";
        return false; // The daemon process is not running!
    }

    // Read daemon's pidfile
    int nPid = -1;
    FILE* fp = fopen("/var/run/crashfixd.pid", "rt");
    if(fp==NULL)
    {
        sErrorMsg = "Error reading daemon's pidfile /var/run/crashfixd.pid.";
        return false; // Invalid responce
    }

    if(1!=fscanf(fp, "%d", &nPid) || nPid<=0)
    {
        fclose(fp);
        sErrorMsg = "Error reading daemon's process ID from file /var/run/crashfixd.pid.";
        return false; // Invalid responce
    }
    fclose(fp);
    sprintf(szBuf, "kill %d 2>&1", nPid);

    // Kill daemon with SIGTERM signal using service shell command
    sResponse = SystemCmd(szBuf);
    if(!sResponse.empty())
    {
        sErrorMsg = "Error killing the daemon; invalid responce from kill command: '";
        sErrorMsg += sResponse;
        sErrorMsg += "'";
        return false; // Invalid response
    }

    // Wait some time until daemon stops itself
    Sleep(5000);

    // Check that process ID doesn't exist
    sResponse = SystemCmd("pgrep crashfixd");
    if(!sResponse.empty())
    {
        sErrorMsg = "Error killing the daemon; the daemon process still exists.";
        return false; // The daemon process still exists
    }

    // Success
    sErrorMsg = "Success";
	return true;

#endif
}
