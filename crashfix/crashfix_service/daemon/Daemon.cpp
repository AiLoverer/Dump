//! \file Daemon.cpp
//! \brief Daemon implementation.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "Daemon.h"
#include "Config.h"
#include "strconv.h"
#include "Misc.h"
#include "CSmtp.h"
#include <iostream>
#include "base64.h"

#ifdef _WIN32
//#include "CrashRpt.h"
#endif

// Static variables
#ifdef _WIN32
#define SERVICE_NAME             L"CrashFix"
#define SERVICE_DISPLAY_NAME     L"CrashFix Service"
#define SERVICE_DESC             L"Processes crash report files"
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\LocalService"
SERVICE_STATUS CDaemon::m_ServiceStatus;
SERVICE_STATUS_HANDLE CDaemon::m_ServiceStatusHandle;
#endif
CDaemon* CDaemon::m_pInstance = NULL;

CDaemon::CDaemon()
{
	// Init internal variables
	m_pInstance = this;
	m_nThreadCount = 0;
	m_nMaxQueueSize = 0;
	m_nServerPort = 0;
	m_nLoggingLevel = 0;
	m_nErrorLogMaxSizeKB = 0;
	m_nCacheMaxEntries = 0;
	m_nCacheMaxMemUsageMB = 0;
	m_MonitorOption = MO_UNDEFINED;
#ifdef _WIN32
	m_nPidToMonitor = _getpid();
#else
	m_nPidToMonitor = getpid();
#endif
	m_bRestartedAfterCrash = false;
	//m_bCriticalError = false;
#ifdef _WIN32
	m_hEventStop = CreateEvent(NULL, TRUE, FALSE, L"Local\\04B8BCA1-BDAF-4686-82CE-A7DF707C5287");
	m_bLogInitialized = false;
#endif
}

CDaemon::~CDaemon()
{
	// Clean up
	m_pInstance = NULL;
#ifdef _WIN32
	if(m_hEventStop)
		CloseHandle(m_hEventStop);
#endif
}

void CDaemon::SetConfigFile(std::string sConfigFile)
{
	m_sConfigFile = sConfigFile;
}

void CDaemon::SetPrefix(std::string sPrefix)
{
	m_sPrefix = sPrefix;
}

void CDaemon::SetMonitorOption(eMonitorOption opt, int nPid)
{
	m_MonitorOption = opt;
	m_nPidToMonitor = nPid;
}

void CDaemon::SetErrorRestartFlag(bool bRestarted)
{
	m_bRestartedAfterCrash = bRestarted;
}

std::string CDaemon::GetWebRootDir()
{
	return m_sWebRootDir;
}

std::string CDaemon::GetSitePollCommand()
{
	return m_sPollCommand;
}

void CDaemon::Run()
{
	// Read config file.
	ReadConfig();

    // Check if process is already running.
    if(0<CheckProcessExists())
        exit(EXIT_FAILURE);

	// In Linux, we need to spawn the child process
	// and continue further initialization in that child process.
	// The following line will do that.
	Daemonize(DT_SPAWN_DAEMON_PROCESS);

#ifndef _WIN32
start:
#endif

    // Reread config file.
	ReadConfig();
		
	// If we are not a monitor process...
	if(m_MonitorOption!=MO_IS_MONITOR)
	{
		// Save process ID to pidfile.
		SavePidToFile();
	}

	// Check if we should launch monitor process.
	if(m_MonitorOption!=MO_IS_MONITOR && m_bLaunchMonitorProcess)
	{
	    // This will fork monitor process. The monitor process will
	    // periodically check our state.
	    LaunchMonitorProcess();
	}

    if(m_MonitorOption!=MO_IS_MONITOR)
	{
		// Init socket server.
		InitSocketServer();

        // We have initialized ourselves successfully.
        // If we are not a monitor process, output the success message.
		//fprintf(stderr, "CrashFix daemon has started successfully.\n");
	}

    // Init logging.
    // In Linux, this will redirect the standard file descriptors to log file.
	// Since now, we will use the log file for output.
	InitErrorLog();

	/*CR_EXCEPTION_INFO ei;
    memset(&ei, 0, sizeof(CR_EXCEPTION_INFO));
    ei.cb = sizeof(CR_EXCEPTION_INFO);
    ei.exctype = CR_SEH_EXCEPTION;
    ei.code = 1234;
    ei.pexcptrs = NULL;
	crGenerateErrorReport(&ei);*/

	// Check if we are the monitor process ourselves
	if(m_MonitorOption==MO_IS_MONITOR)
	{
	    // We are the monitor process. Enter the loop and perform sanity checks
	    // of the parent daemon process.
	    RunAsMonitor();

#ifndef _WIN32
	    // If the function above has exited, this means the daemon has crashed,
	    // the new daemon process was forked, and we are now the new daemon.
	    goto start;
#endif
	}
    else
	{
	    // Run server. This function doesn't return until server is stopped.
        RunSocketServer();
	}

	// Exit.
	m_Log.write(0, "Daemon is exiting.\n");
	exit(EXIT_SUCCESS);
}

int CDaemon::ReadPidFile()
{
    // Open fipdfile
	FILE* f = fopen(m_sPIDFile.c_str(), "rt");
	if(f==NULL)
	{
	    std::stringstream str;
	    str<<"Could not read PID file ";
	    str<<m_sPIDFile;
		perror(str.str().c_str());
		return -1;
	}

    int nPid = -1;
	if(1!=fscanf(f, "%d", &nPid))
	{
		printf("Error reading PID from pidfile %s.\n", m_sPIDFile.c_str());
		return -1;
	}

	printf("Read pidfile (pid=%d).\n", nPid);

	fclose(f);
	f = NULL;

    return nPid;
}

int CDaemon::CheckProcessExists()
{
#ifdef _WIN32
	return -1; // Not implemented
#else
    printf("Checking if crashfixd process exists...\n");

    // Read PID-file
    int nPid = ReadPidFile();
    if(nPid==-1)
        return -1; // PID file does not exist.

    // Check if a process with such a PID exists
    if (kill(nPid, 0) == 0)
    {
        /* process is running or a zombie */
        printf("The crashfixd daemon process is already running.\n");
        return nPid;
    }
    else if (errno == ESRCH)
    {
        /* no such process with the given pid is running */
        printf("There is no running process with such a PID.\n");
        return -2;
    }
    else
    {
        /* some other error... use perror("...") or strerror(errno) to report */
        perror("Error checking crashfixd process existance");
        return -3;
    }
#endif


}

void CDaemon::LaunchMonitorProcess()
{
	// Do not launch monitor when another monitor restarted us
	// after crash. The old monitor still continues to work.
	if(m_bRestartedAfterCrash)
		return;

	// Launch monitor process
#ifdef _WIN32
	std::wstring sExePath = GetModuleName(NULL);
	std::ostringstream sCmdLine;
	sCmdLine << strconv::w2a(sExePath);
	sCmdLine << " --start -c \"";
	sCmdLine << m_sConfigFile;
	sCmdLine << "\" --run-as-monitor ";
	sCmdLine << _getpid();
	std::string str = sCmdLine.str();
	int nResult = execute(str.c_str(), false);
	if(nResult!=0)
	{
		std::ostringstream str;
		str << "Error launching monitoring process (error code = ";
		str << nResult;
		str << ")";
		Die(str.str().c_str(), true);
	}


#else
    // Spawn the child monitor process
	Daemonize(DT_SPAWN_MONITOR_PROCESS);

#endif
}

void CDaemon::AddPrefix(std::string& sFileName)
{
	// This method prepends a file name with the prefix string.
	// This is used when the daemon is installed in some non-standard directory.
	sFileName = m_sPrefix + sFileName;
}

void CDaemon::ReadConfig()
{
	// This method reads the daemon's configuration file and
	// extacts our configuration parameters from it.

	if(m_sConfigFile.empty())
	{
		// Set config file name to default
#ifdef _WIN32
		m_sConfigFile = strconv::w2a(GetModulePath(NULL));
		m_sConfigFile += "\\..\\conf\\crashfixd.conf";
#else
		m_sConfigFile = "/etc/crashfix/crashfixd.conf";
#endif
	}

	// Check config file - does it exist?
	struct stat st;
	if( stat(m_sConfigFile.c_str(), &st)!=0 || // does such file exist?
		(st.st_mode&S_IFDIR) )  // is it regular file (not directory)?
	{
		std::string sErrMsg = "Can't read configuration file (the file doesn't exist) ";
		sErrMsg += m_sConfigFile;
		Die((char*)sErrMsg.c_str(), true);
	}

	// Check the file - can it be opened for reading?
	FILE* f = fopen(m_sConfigFile.c_str(), "rt");
	if(f==NULL)
	{
		std::string sErrMsg = "Can't read configuration file for reading (permission denied) ";
		sErrMsg += m_sConfigFile;
		Die((char*)sErrMsg.c_str(), true);
	}
	fclose(f);

	bool bInit = false;
	CConfig config(m_sConfigFile.c_str(), &bInit);

	// Reading general settings.
	const int BUFF_SIZE = 2048;
	char szBuff[BUFF_SIZE];

	//if(m_sPrefix.empty())
	//{
	//	// Override the prefix if it was not specified in the command line
	//	const char* szPrefix = config.getProfileString("PREFIX", szBuff, BUFF_SIZE);
	//	if(szPrefix && strlen(szPrefix)>0)
	//		SetPrefix(szPrefix);
	//}

	// Read license file name
	m_sLicenseFile = config.getProfileString("LICENSE_FILE", szBuff, 2048);
	AddPrefix(m_sLicenseFile); // This parameter is affected by prefix

	// Read max thread count
	m_nThreadCount = config.getProfileInt("MAX_THREAD_COUNT", 50);

	// Read max queue size
	m_nMaxQueueSize = config.getProfileInt("MAX_QUEUE_SIZE", 100);

	// Read server port number
	m_nServerPort = config.getProfileInt("SERVER_PORT", 50);

	// Read pid file name
	m_sPIDFile = config.getProfileString("PIDFILE", szBuff, 2048);
	AddPrefix(m_sPIDFile); // Affected by prefix

	// Read webroot dir (where the webapp files located)
	m_sWebRootDir = config.getProfileString("WEB_ROOT_DIR", szBuff, 2048);
	if(m_sWebRootDir.empty())
	{
#ifdef _WIN32

#else
		m_sWebRootDir = "/var/www";
#endif
	}
	AddPrefix(m_sWebRootDir); // Affected by prefix

	// Append slash to the web root dir path if it doesn't presents
	if(!m_sWebRootDir.empty() && m_sWebRootDir.at(m_sWebRootDir.length()-1)!='/')
		m_sWebRootDir += '/';

	// Format polling command
	m_sPollCommand = "php \"";
	m_sPollCommand += m_sWebRootDir;
	m_sPollCommand += "protected/yiic.php\" poll";

	// Reading log file name
	m_sErrorLogFile = config.getProfileString("ERROR_LOG_PATH", szBuff, 2048);
	AddPrefix(m_sErrorLogFile);

	if(m_sErrorLogFile.empty())
	{
		// Set config file name to default
#ifdef _WIN32
		m_sErrorLogFile = "error.log";
#else
		m_sErrorLogFile = "/var/log/crsd.conf";
#endif
	}

	// Read logging level
	m_nLoggingLevel = config.getProfileInt("LOGGING_LEVEL", 0);

	// Read max size of the error log
	m_nErrorLogMaxSizeKB = config.getProfileInt("ERROR_LOG_MAX_SIZE_KB", 10*1024);

	// Read cache related settings.

	m_nCacheMaxEntries = config.getProfileInt("MAX_DEBUG_INFO_CACHE_SIZE", 100);

	m_nCacheMaxMemUsageMB = config.getProfileInt("MAX_MEM_USAGE_MB", 100);

	/* Read monitoring process related settings */

	m_sMonitorLogFile = config.getProfileString("MONITOR_LOG_PATH", szBuff, 2048);
	AddPrefix(m_sMonitorLogFile);

	m_sWebmasterEmail = config.getProfileString("WEBMASTER_EMAIL", szBuff, 2048);

	m_bLaunchMonitorProcess = 1== config.getProfileInt("LAUNCH_MONITORING_PROCESS", 0);

	m_sSmtpServerHost = config.getProfileString("SMTP_SERVER", szBuff, 2048);

	m_nSmtpServerPort = 25;
	size_t nPos = m_sSmtpServerHost.rfind(':');
	if(nPos!=m_sSmtpServerHost.npos)
	{
		std::string sSmtpServerPort = m_sSmtpServerHost.substr(nPos+1);
		m_nSmtpServerPort = atoi(sSmtpServerPort.c_str());
		m_sSmtpServerHost = m_sSmtpServerHost.substr(0, nPos);
	}

	m_sSmtpLogin = config.getProfileString("SMTP_LOGIN", szBuff, 2048);

	m_sSmtpPassword = config.getProfileString("SMTP_PASSWORD", szBuff, 2048);

	m_bNotifyWebmasterOnErrors = 1== config.getProfileInt("NOTIFY_WEBMASTER_ON_ERRORS", 0);

	m_bRestartDaemonOnCrash = 1== config.getProfileInt("RESTART_DAEMON_ON_CRASH", 0);
}

void CDaemon::InitErrorLog()
{
	// This method opens error log file or monitor log file (if in monitor mode)
	// and dumps the header lines to the file. The header lines contain general
	// daemon information.

#ifdef _WIN32
	if(m_bLogInitialized)
		return; // Log is already initialized
	m_bLogInitialized = true;
#endif

	BOOL bInitLog = FALSE;

	if(m_MonitorOption==MO_IS_MONITOR)
	{
#ifndef _WIN32
        // In Linux, we have to first close the handle to error.log file,
        // which is already opened.
        m_Log.term();
#endif
        bInitLog = m_Log.init(strconv::a2w(m_sMonitorLogFile), true);
	}
	else
		bInitLog = m_Log.init(strconv::a2w(m_sErrorLogFile), true);
	if(!bInitLog)
	{
		std::string sErrorMsg = "Couldn't open log file ";
		sErrorMsg += m_sErrorLogFile;
		Die(sErrorMsg.c_str(), true);
	}

    // Set logging level (take the level from config).
    m_Log.set_level(m_nLoggingLevel);
    m_Log.set_max_size(m_nErrorLogMaxSizeKB);

	// Write general server info to error.log
	m_Log.write(0, "==================================\n");
	if(m_MonitorOption==MO_IS_MONITOR)
		m_Log.write(0, "CrashFix Daemon Monitor Log\n");
	else
		m_Log.write(0, "CrashFix Daemon Log\n");
    m_Log.write(0, "==================================\n");
	m_Log.write(0, "Configuration summary:\n");
	m_Log.write(0, "Path to configuration file is '%s'\n", m_sConfigFile.c_str());
	m_Log.write(0, "Logging level is %d\n", m_nLoggingLevel);
	m_Log.write(0, "Log maximum file size is %d KB.\n", m_nErrorLogMaxSizeKB);
	if(m_MonitorOption==MO_IS_MONITOR)
		m_Log.write(0, "Path to log file is '%s'\n", m_sMonitorLogFile.c_str());
	else
	{
		m_Log.write(0, "Path to log file is '%s'\n", m_sErrorLogFile.c_str());
		m_Log.write(0, "Path to pidfile is '%s'\n", m_sPIDFile.c_str());
		m_Log.write(0, "Count of worker threads is %d\n", m_nThreadCount);
		//m_Log.write(0, "Server port number is %d\n", m_nServerPort);
		//m_Log.write(0, "Maximum request queue size is %d\n", m_nMaxQueueSize);
		m_Log.write(0, "Debug info cache maximum size is %d\n", m_nCacheMaxEntries);
		m_Log.write(0, "Memory consumption maximum limit is %d MB\n", m_nCacheMaxMemUsageMB);
	}
}

void CDaemon::InitSocketServer()
{
	// Init socket server - try to bind the server socket.
	bool bInit = m_SocketServer.Init(this, m_nServerPort, m_nMaxQueueSize, m_nThreadCount, m_nCacheMaxMemUsageMB, &m_Log);
	if(!bInit)
	{
		// Coldn't init the socket server by some reason.
		std::string sErrorMsg = "Couldn't init socket server: ";
		sErrorMsg += m_SocketServer.GetErrorMsg();
		Die(sErrorMsg.c_str(), true);
	}
}

void CDaemon::RunSocketServer()
{
#ifdef _WIN32
	// Create stop waiting thread
	CreateThread(NULL, 0, StopWaitingThread, this, 0, NULL);
#endif

	// This method enters the infinite loop of listening the server socket.
	// It exists only when the socket server stops.
	m_SocketServer.Run();
}

bool CDaemon::Daemonize(eDaemonizeType Flag)
{
	// This method is used mainly in Linux to fork off the parent process,
	// change working dir and set umask.

#ifdef _WIN32
	return false;
#else
	/* Our process ID and Session ID */
	pid_t pid = -1;
	pid_t sid = -1;
	std::string sErrorMsg;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
	{
	    sErrorMsg = "Couldn't fork off the parent process.";
	    goto cleanup;
	}

	if(pid>0) // If pid>0, we are in parent process
	{
	    if(Flag==DT_SPAWN_DAEMON_PROCESS)
	    {
            // We (root-launched process) have spawned the child (daemon) process
            printf("Forked the CrashFix daemon process: pid = %d.\n", pid);
            exit(EXIT_SUCCESS);
	    }
	    else if(Flag==DT_SPAWN_DAEMON_ON_CRASH)
	    {
            // We (monitor) have spawned the child (daemon) process
            m_Log.write(0, "Spawned the daemon process: pid = %d.\n", pid);
            m_nPidToMonitor = pid;
	    }
	    else if(Flag==DT_SPAWN_MONITOR_PROCESS)
	    {
	        // We have spawned the monitor process
	        m_Log.write(0, "Spawned the monitoring process: pid = %d\n", pid);
	        m_nPidToMonitor = pid;
	    }
	}
	else // pid==0
	{
		// We are now the child process

		/* Change the file mode mask, this is required to access files. */
		umask(0);

		/* Create a new SID for the child process, this will detach us from terminal. */
		sid = setsid();
		if (sid < 0)
		{
			sErrorMsg = "Error creating a new SID.";
			goto cleanup;
		}

		/* Change the current working directory to root '/' */
		if ((chdir("/")) < 0)
		{
			sErrorMsg = "Error changing current working directory.";
			goto cleanup;
		}

        // Get parent pid
        int nParentPid = getppid();

        // If bFlag is false, this means the parent process wants us to be the monitor.
		if(Flag==DT_SPAWN_MONITOR_PROCESS)
		{
	        // We are now the monitor process.
            m_MonitorOption = MO_IS_MONITOR;
            m_nPidToMonitor = nParentPid; // Monitor the parent daemon process.

            // In Linux, we need to handle SIGCHLD signal to avoid
            // zombie process records in system table (if our child process terminates unexpectedly)
            RegisterCHLDHandler(true);
	    }
	    else if(Flag==DT_SPAWN_DAEMON_ON_CRASH)
	    {
            // The daemon was restarted by the monitoring process, possibly after segmentation fault.
            m_bRestartedAfterCrash = true;
            m_MonitorOption = MO_NO_MONITOR; // Do not launch the monitor another time, there is already one.

            // Ignore SIGCHLD signal (default behavior)
            RegisterCHLDHandler(false);
	    }

	    // In Linux, we register the SIGTERM signal handler.
	    // The sigterm signal is sent by the system when it wants us to shut down.
	    RegisterTermHandler();
	}

cleanup:

    if(!sErrorMsg.empty())
    {
        if(Flag==DT_SPAWN_DAEMON_ON_CRASH)
        {
            m_Log.write(0, sErrorMsg.c_str());
            std::string sErr = "Error spawning the child process while restarting the daemon: ";
            sErr += sErrorMsg;
            AddError(true, sErr.c_str());
        }
        else
            Die(sErrorMsg.c_str(), true);
    }

    // Return true to indicate we are in child process now.
    // Return false to indicate we are still in the parent process.
    return (pid==0)?true:false;
#endif

	// Return false to indicate we are still in the parent process.
	return false;
}

void CDaemon::SavePidToFile()
{
	/* Write pid to pidfile. */

	FILE* f = fopen(m_sPIDFile.c_str(), "wt");
	if(f==NULL)
	{
		std::string sErrMsg = "Error opening pidfile ";
		sErrMsg += m_sPIDFile;
		sErrMsg += " for writing process ID.";
		Die((char*)sErrMsg.c_str(), true);
	}

#ifdef _WIN32
	fprintf(f, "%u\n", GetCurrentProcessId());
#else
	fprintf(f, "%d\n", getpid());
#endif

	fclose(f);
}

#ifndef _WIN32
void CDaemon::TerminateHandler(int)
{
	// Linux only.
	// This code is executed when a SIGTERM signal is caught by the child process.
	// This signal is typically received on system shut down and means the process
	// should free used resources and exit as soon as possible.

	// Log error message
	CLog* pLog = CDaemon::GetInstance()->GetLog();
	pLog->write(0, "SIGTERM signal caught (the system wants us to shut down).\n");

    // Soft shutdown
    if(CDaemon::GetInstance()->GetMonitorOption()!=MO_IS_MONITOR)
    {
        // In Linux, we need to send SIGTERM signal to our monitoring process.
        // Otherwise the monitoring process will relaunch the daemon.
        int nPid = CDaemon::GetInstance()->GetPidToMonitor();
        if(nPid!=0)
        {
            pLog->write(0, "Sending SIGTERM signal to monitoring process (pid = %d) to avoid relaunching the daemon.\n", nPid);
            kill(nPid, SIGTERM);
        }

        // Now we shut down our socket server.
        // We need to close all socket connections, especially server socket
        // to minimize possible risk of socket binding errors and system resources exhaustion.
        CDaemon::GetInstance()->GetSocketServer()->Terminate();
    }

    pLog->write(0, "Terminated. Bue!\n");

	// Terminate the process.
	exit(0);
}

void CDaemon::RegisterTermHandler()
{
	// Linux only.
	// This method registers a SIGTERM handler. The SIGTERM signal is
	// received on system shutdown.

	struct sigaction sigterm_handler;
	sigterm_handler.sa_handler = TerminateHandler;
	sigaction(SIGTERM, &sigterm_handler, NULL);
}

/* SIGCHLD handler. */
void CDaemon::SIGCHLDHandler(int)
{
    // Log error message
	CDaemon::GetInstance()->GetLog()->write(0, "SIGCHLD signal caught (the child process has terminated).\n");
    CDaemon::GetInstance()->GetLog()->write(0, "Waiting for all child zombie processes...\n");

	/* Wait for all dead processes.
	 * We use a non-blocking call to be sure this signal handler will not
	 * block if a child was cleaned up in another part of the program. */
	while (waitpid(-1, NULL, WNOHANG) > 0)
	{
	}

    CDaemon::GetInstance()->GetLog()->write(0, "Cleaned up all child zombie processes.\n");
}

void CDaemon::RegisterCHLDHandler(bool bRegister)
{
	// Linux only.
	// This method registers a SIGCHLD handler. The SIGCHLD signal is
	// received by the parent process when its child has terminated.
	// The parent process should read the child's exit code to avoid zombie records
	// in system process table. A zombie process or defunct process is a process that
    // has completed execution but still has an entry in the process table. This entry
    // is still needed to allow the parent process to read its child's exit status.

    if(bRegister)
    {
        struct sigaction sigterm_handler;
        sigterm_handler.sa_handler = SIGCHLDHandler;
        sigaction(SIGCHLD, &sigterm_handler, &m_OldSIGCHLDAction);
    }
    else
    {
        sigaction(SIGCHLD, &m_OldSIGCHLDAction, NULL);
    }
}
#endif


void CDaemon::Die(const char* szMessage, bool bUsePStr)
{
	// This method is called on a unfovaurable outcome.
	// In this method, we output an error message to screen and to log file and
	// terminate the process with error code.

	// Log error message
	m_Log.write(0, "Dying: %s\n", szMessage);

#ifdef _WIN32
	printf(szMessage);
	printf("\n");
#else
	// Print error message to screen
	if(bUsePStr)
		perror(szMessage);
	else
		printf("%s\n", szMessage);
#endif

	// Exit with error
	exit(EXIT_FAILURE);
}

int CDaemon::Start()
{
#ifdef _WIN32
	return StartNTService();
#else
	Run();
	return 0;
#endif
}

int CDaemon::Restart()
{
	// Stop the service
	int nResult = Stop();
	if(nResult==0)
	{
		// Start again
		nResult = Start();
	}

	return nResult;
}

int CDaemon::Stop()
{
    int nStatus = 1;
	int nPid = -1;
	int i = 0;
	bool bExists = false;
	bool bTerminated = false;

#ifdef _WIN32

	// Delete pidfile
	DeleteFile(strconv::a2w(m_sPIDFile).c_str());

	return StartNTService(FALSE);

#else
    // Read config
    ReadConfig();

    // Check if daemon process exists
    nPid = CheckProcessExists();
    if(nPid>0)
    {
        // Send SIGTERM signal
        printf("Sending TERM signal.");
        kill(nPid, SIGTERM);
        for(i=0; i<10; i++)
        {
            // Wait for a while
            Sleep(1000);
            printf(".");
            fflush(stdout);

            // Check if process still exists
            if(kill(nPid, 0)!=0 && errno==ESRCH)
            {
                // Process does not exist
                bTerminated = true;
                break;
            }
        }

        printf("\n");

        if(bTerminated)
            printf("CrashFix daemon has been stopped by TERM signal.\n");
    }


    // Deleting pidfile
    printf("Deleting pidfile.\n");
    if(0!=unlink(m_sPIDFile.c_str()))
    {
        perror("Could not delete pidfile");
    }

    if(!bTerminated)
    {
        printf("Trying killall command.\n");
        if(0!=system("killall -KILL crashfixd"))
        {
            perror("Error killing crashfixd daemon process");
        }
    }

    printf("CrashFix service has been stopped OK.\n");
#endif

    nStatus = 0;
	return nStatus;
}

CDaemon* CDaemon::GetInstance()
{
	// Returns pointer to this.
	return m_pInstance;
}

CLog* CDaemon::GetLog()
{
	// Returns owned log object.
	return &m_Log;
}

void CDaemon::RunAsMonitor()
{
	// This method is executed when we are running as a monitor process.
	// Here we enter the infinite loop of checking the daemon process status
	// periodically and notify webmaster on unexpected events through Email.

    int nStatus = -1;
    int nExecute = -1;
    int nServerRetCode = -1;
    std::vector<std::string> asServerErrors;

	m_Log.write(0, "Running as monitoring process (pid to monitor = %d)\n", m_nPidToMonitor);

	for(;;)
	{
		// Wake up regularily
#ifdef _WIN32
		if(WAIT_OBJECT_0==WaitForSingleObject(m_hEventStop, 60*1000))
		{
			m_Log.write(0, "Stop event.\n");
			return;
		}
#else
		Sleep(60*1000); // Wair for a munute
#endif

		m_Log.write(1, "Waking up and checking daemon process.\n");

		BOOL bError = FALSE;
		BOOL bRestart = FALSE;
		std::string sErrorMsg;

#ifdef _WIN32

		// Open process handle.
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_nPidToMonitor);
		if(hProcess==NULL)
		{
			// It seems that the process we are watching at has terminated (probably due to crash).
			// We need to restart the process.

			m_Log.write(0, "Unexpected event: Error opening process to monitor (PID = %d)\n", m_nPidToMonitor);
			m_Log.log_last_error();
			bError = TRUE;   // There is an error event
			bRestart = TRUE; // We have to restart the process
			AddError(true, "The monitoring process couldn't open the daemon process handle (the daemon process seems to be not launched or dead).");
			goto skip;
		}

		// We have opened the process handle successfully, but we want to perform an
		// additional check of process exit code.
		DWORD dwExitCode = 1;
		BOOL bGet = GetExitCodeProcess(hProcess, &dwExitCode);
		CloseHandle(hProcess);
		if(!bGet)
		{
			// We couldn't get the exit code of the process.
			// We have to notify the webmaster and restart the process.

			m_Log.write(0, "Unexpected event: the function GetExitCodeProcess has failed with error %u\n", dwExitCode);
			AddError(true, "The monitoring process couldn't get exit code of the daemon process (the daemon process seems to be not launched or dead).");
			m_Log.log_last_error(0);
			bError = TRUE;
			bRestart = TRUE;
			goto skip;
		}
		else if(dwExitCode!=STILL_ACTIVE)
		{
			// It seems that the process has exited.
			// We have to notify the webmaster and restart the process.

			m_Log.write(0, "Unexpected event: daemon process has exited with code = %u\n", dwExitCode);
			AddError(true, "The monitoring process detected that the daemon process has terminated (probably because of a critical error). The daemon will be automatically restarted.");
			bError = TRUE;
			bRestart = TRUE;
			goto skip;
		}
#else

        if(kill(m_nPidToMonitor, 0)<0)
        {
            // The process seems to be terminated.
            m_Log.write(0, "Unexpected event: error opening process to monitor (PID = %d)\n", m_nPidToMonitor);
            m_Log.log_last_error(0, "kill function returned error code");
            AddError(true, "The monitoring process has detected that the daemon process is inactive (not launched at all or has terminated by some reason). The daemon process will be relaunched.");
            bError = TRUE;   // There is an error event
            bRestart = TRUE; // We have to restart the process
            goto skip;
        }

#endif

        // If we came here, the process we are watching at seems to be running.
        // But we want to additionally perform the check of server's status by issuing the 'daemon status' command.
        asServerErrors.clear();
        nExecute = ExecuteClientRequest("daemon status\n", sErrorMsg, nServerRetCode, asServerErrors);
        if(0!=nExecute)
        {
            // Critical error - couldn't execute request to daemon.
            // We need to notify webmaster, but do not restart the process.
            m_Log.write(0, "Unexpected event: couldn't execute a request to daemon: %s.\n", sErrorMsg.c_str());
            std::string sErr = "The monitoring process couldn't connect to daemon: " + sErrorMsg;
            AddError(true, sErr.c_str());

            bError = TRUE;
            bRestart = FALSE;
            goto skip;
        }
        else
        {
            // Request executed ok, now read what daemon returned
            if(nServerRetCode!=0)
            {
                // It seems that the daemon reports an error status.
                // We need to notify webmaster, but do not restart the process.
                m_Log.write(0, "Unexpected event: the daemon reports some errors.\n");
                size_t i;
                for(i=0; i<asServerErrors.size(); i++)
                {
                    AddError(false, asServerErrors[i].c_str());
                    m_Log.write(0, "Error %d. %s\n", i+1, asServerErrors[i].c_str());
                }

                bError = TRUE;
                bRestart = FALSE;
                goto skip;
            }
        }

skip:
		// If there were errors
		if(bError)
		{
			// Restart daemon
			if(bRestart)
			{
                if(RestartDaemonOnCrash())
                {
                    // If the function returned true, we are in child process.
                    // We need to exit the loop to let the new daemon run.
                    return;
                }
                else
                {
                    // The function returned false, we are in parent (monitor) process.
                }
			}

			// Notify webmaster about errors
            NotifyWebmasterOnError();

            // Then continue monitoring in loop.
		}
	}
}

#ifdef _WIN32
int CDaemon::LaunchDaemonProcess(int &nPid)
{
	std::wstring sExePath = GetModuleName(NULL);
	std::ostringstream sCmdLine;
	sCmdLine << strconv::w2a(sExePath);
	sCmdLine << " --run";
	std::string str = sCmdLine.str();
	int nResult = execute(str.c_str(), false, &nPid);
	return nResult;
}
#endif

int CDaemon::ExecuteClientRequest(const char* szCmdLine, std::string& sErrorMsg,
                                  int& nServerRetCode, std::vector<std::string>& asServerErrors)
{
	// This method sends a request to daemon through socket connection
	// and gets a response code and a list of error messages.

	int nStatus = -1; // Status
	SOCK sock = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server = NULL;
	const int a = 10;
	std::string sResponse;
	char* pPtr = NULL;
    const char* szInput = NULL;
    size_t nBufSize = 0;
    char* pToken = NULL;
	const int MAX_IN_BUFF_SIZE = 10240;
	const int TIMEOUT = 5;

    sErrorMsg = "Unspecified error.";

    // Init output
    asServerErrors.clear();

	// Get host address
	server = gethostbyname("localhost");
	if(server == NULL)
	{
		sErrorMsg = ("Error getting host name.");
		goto exit;
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serv_addr.sin_port = htons(m_nServerPort);

	// Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		sErrorMsg = "Error opening socket.";
		goto exit;
	}

	// Connect socket
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		sErrorMsg = "Error connecting socket";
		goto exit;
	}

	// Read server greeting message.
    if(0!=m_SocketServer.RecvMsgWithTimeout(sock, TIMEOUT, MAX_IN_BUFF_SIZE, sResponse, sErrorMsg))
    {
        // Error reading server greeting message
        sErrorMsg = "Error reading greeting message: " + sErrorMsg;
        goto exit;
    }

    // Get numeric status code
	if(atoi(sResponse.c_str())!=100)
	{
	    sErrorMsg = "Invalid server response (expected greeting message).";
	    goto exit;
	}

	m_Log.write(2, "Server invitation: %s\n", sResponse.c_str());

	// Send request to server.

    m_Log.write(2, "Sending request to server: %s\n", szCmdLine);
    if(0!=m_SocketServer.SendMsgWithTimeout(sock, szCmdLine, TIMEOUT, sErrorMsg))
    {
        // Error reading server greeting
        sErrorMsg = "Error sending request to server: " + sErrorMsg;
        goto exit;
    }

    // Read response from server.
    m_Log.write(2, "Reading response from server\n");
    if(0!=m_SocketServer.RecvMsgWithTimeout(sock, TIMEOUT, MAX_IN_BUFF_SIZE, sResponse, sErrorMsg))
    {
        // Error reading server greeting
        sErrorMsg = "Error reading server response: " + sErrorMsg;
        goto exit;
    }

	m_Log.write(2, "Server response: %s.\n", sResponse.c_str());

    szInput = sResponse.c_str();
    nBufSize = sResponse.size();

    // Get numeric status code
    pToken = strtok_r((char*)szInput, " ", &pPtr);
    if(pToken==NULL)
    {
        sErrorMsg = "Invalid server response code.";
        goto exit;
    }

	nServerRetCode = atoi(pToken);
	m_Log.write(2, "Server ret code: %d.\n", nServerRetCode);

    // Now split the error message list using ';' character as a separator.

    // Get first error message
    pToken = strtok_r(pPtr, ";\n", &pPtr);
    while( pToken != NULL )
    {
        if(strlen(pToken)!=0)
        {
            // Add command to the list
            asServerErrors.push_back(pToken);
        }

        // Get the next token
        pToken = strtok_r(NULL, ";\n", &pPtr);
    }

    // Request executed successfully
    sErrorMsg = "Success.";
    nStatus = 0;

exit:

    if(sock!=0)
        CLOSESOCK(sock);

	return nStatus;
}

void CDaemon::NotifyWebmasterOnError()
{
	// This method is called by the monitor to notify the webmaster
	// about an error through email.

	// Check our config - should we notify the webmaster on error or not?
	if(m_bNotifyWebmasterOnErrors==false)
		return;

	bool bCriticalError = false;
	std::vector<std::string> asErrorsToSend;
	{
		CAutoLock lock(&m_csLock);
		std::map<std::string, ErrorInfo>::iterator it;
		for(it=m_asErrors.begin(); it!=m_asErrors.end(); it++)
		{
			ErrorInfo& ei = it->second;
			time_t tmCurTime;
			time(&tmCurTime);
			// Check if 30 min elapsed since the last message was sent
			if(tmCurTime-ei.tmSentTime>60*30)
			{
				// Send it again
				ei.tmSentTime = tmCurTime;
				asErrorsToSend.push_back(it->first);
				if(ei.m_bCriticalError)
					bCriticalError = true;
			}
		}
	}

	if(asErrorsToSend.size()==0)
		return; // Nothing to send

	try
	{
		// Compose mail
		CSmtp mail;

		mail.SetSMTPServer(m_sSmtpServerHost.c_str(), m_nSmtpServerPort);
		mail.SetSenderName("CrashFix Daemon Monitor");
		mail.SetLogin(m_sSmtpLogin.c_str());
		mail.SetPassword(m_sSmtpPassword.c_str());
		mail.SetSenderMail("noreply@localhost");
		if(bCriticalError)
            mail.SetSubject("[CRITICAL] CrashFix Daemon Message");
        else
            mail.SetSubject("[WARNING] CrashFix Daemon Message");
		mail.AddRecipient(m_sWebmasterEmail.c_str());
		mail.SetXPriority(XPRIORITY_HIGH);
		mail.SetXMailer("CrashFix Daemon Monitor");
		mail.AddMsgLine("Hello,");
		mail.AddMsgLine("");
        mail.AddMsgLine("I have to inform you that the CrashFix Daemon has encountered a problem:");
		size_t i;
		for(i=0; i<asErrorsToSend.size(); i++)
		{
			std::string sErrorMsg = asErrorsToSend[i];

		    std::string sMsg =" - ";
			sMsg += sErrorMsg;
		    mail.AddMsgLine(sMsg.c_str());
		}

		// Clear old error messages
		ClearErrors(true);

		mail.AddMsgLine("");
		mail.AddMsgLine("--");
		mail.AddMsgLine("Regards,");
		mail.AddMsgLine("CrashFix Daemon Monitor");

		// Send mail
		m_Log.write(0, "Sending an E-mail to webmaster %s\n", m_sWebmasterEmail.c_str());

		mail.Send();
	}
	catch(ECSmtp e)
	{
		// Caught an SMTP error
		m_Log.write(0, "Email sending error (SMTP client error %d): %s.\n",
			e.GetErrorNum(), e.GetErrorText().c_str());
		std::string sServerResponse = e.GetServerResponce();
		if(!sServerResponse.empty())
		{
            m_Log.write(0, "SMTP server response: %s.\n", sServerResponse.c_str());
		}
	}
}

bool CDaemon::RestartDaemonOnCrash()
{
	// This method is called by monitor to restart the process we are watching at
	// on unexpected critical error (crash).

	// Check our config - should we restart the daemon on crash or not?
	if(m_bRestartDaemonOnCrash==false)
	{
	    m_Log.write(0, "Daemon restart disabled by config settings.\n");
	    // Return false to indicate we are still in parent process.
	    return false;
	}

	m_Log.write(0, "Restarting the daemon.\n");

	// Start the daemon process again
#ifdef _WIN32

	std::wstring sExePath = GetModuleName(NULL);
	std::ostringstream sCmdLine;
	sCmdLine << strconv::w2a(sExePath);
	sCmdLine << " --run -c \"";
	sCmdLine << m_sConfigFile;
	sCmdLine << "\" --restart-on-crash";
	std::string str = sCmdLine.str();
	int nPid = 0;
	int nResult = execute(str.c_str(), false, &nPid);

	if(nResult!=0)
	{
		m_Log.write(0, "Error restarting daemon process (error code = %d)\n", nResult);
	}
	else
	{
		// Update PID we are watching
		m_nPidToMonitor = nPid;
		m_Log.write(0, "The daemon was restarted (PID = %d)\n", nPid);
	}

	return false;

#else
    // Spawn the child daemon process.
    // DT_SPAWN_DAEMON_ON_CRASH parameter tells not to die on process creation errors,
    // because we need to continue monitoring.
    // Daemonize() will return false for the parent process, and true for the child process.
	return Daemonize(DT_SPAWN_DAEMON_ON_CRASH);

#endif
}

CSocketServer* CDaemon::GetSocketServer()
{
    return &m_SocketServer;
}

eMonitorOption CDaemon::GetMonitorOption()
{
    return m_MonitorOption;
}

int CDaemon::GetPidToMonitor()
{
    return m_nPidToMonitor;
}

void CDaemon::AddError(bool bCritical, const char* szErrorMsg)
{
	CAutoLock lock(&m_csLock);

    // Ensure there are not too many errors in the list
    if(m_asErrors.size()<50)
    {
        // Search for duplicate errors
        std::map<std::string, ErrorInfo>::iterator it = m_asErrors.find(szErrorMsg);
        if(it==m_asErrors.end())
        {
            // There are no such a message, add it
			ErrorInfo ei;
			time(&ei.tmCreateTime);
			time(&ei.tmUpdateTime);
			ei.m_bCriticalError = bCritical;
            m_asErrors[szErrorMsg] = ei;
        }
		else
		{
			// There is such a message, update its timestamp
			ErrorInfo& ei = it->second;
			time(&ei.tmUpdateTime);
		}
    }
    else
    {
        // There are too many errors
        m_Log.write(0, "AddError: Could not add error message '%s' to error list, because error message list is too long.", szErrorMsg);
    }
}

void CDaemon::ClearErrors(bool bOnlyRetired)
{
	CAutoLock lock(&m_csLock);

	if(!bOnlyRetired)
	{
		// Clear all errors
		m_asErrors.clear();
	}
	else
	{
		// Clear only errors that has expired
		bool bError = true;
		while(bError)
		{
			std::map<std::string, ErrorInfo>::iterator it;
			for(it=m_asErrors.begin(); it!=m_asErrors.end(); it++)
			{
				ErrorInfo& ei = it->second;
				time_t tmCurTime;
				time(&tmCurTime);
				// Check if error has not been updated for 5 min
				if(tmCurTime-ei.tmUpdateTime>60*5)
				{
					// Remove this error (it has expired).
					m_asErrors.erase(it);
					break;
				}
			}

			bError = false;
		}
	}
}

int CDaemon::GetErrorCount()
{
	CAutoLock lock(&m_csLock);
    return (int)m_asErrors.size();
}

bool CDaemon::GetError(int nIndex, std::string& sError)
{
	CAutoLock lock(&m_csLock);
    if(nIndex<0 || nIndex>=(int)m_asErrors.size())
        return false;

	std::map<std::string, ErrorInfo>::iterator it = m_asErrors.begin();
	int i;
	for(i=0; i<nIndex; i++)
		it++;

	sError = it->first;
    return true;
}

#ifdef _WIN32
int CDaemon::Install()
{
#ifdef _WIN32
	return InstallNTService();
#else
	return 1; // Not implemented
#endif
}

int CDaemon::Remove()
{
#ifdef _WIN32
	return RemoveNTService();
#else
	return 1; // Not implemented
#endif
}

int CDaemon::InstallNTService()
{
	// Open Service Control Manager
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL,
		SC_MANAGER_CONNECT|SC_MANAGER_CREATE_SERVICE);
	if(!hSCManager)
	{
		wprintf(L"Error opening Service Control Manager: %s\n", GetErrorMsg().c_str());
		return 1;
	}

	// Determine path to executable file
	WCHAR szModulePath[_MAX_PATH]=L"";
	GetModuleFileName(NULL, szModulePath, _MAX_PATH);

	std::wstring sCmdLine = L"\"";
	sCmdLine += szModulePath;
	sCmdLine += L"\"";
	sCmdLine += L" --run-as-service";

	// Install the service into SCM by calling CreateService
	SC_HANDLE hService = CreateService(
        hSCManager,                     // SCManager database
        SERVICE_NAME,                   // Name of service
        SERVICE_DISPLAY_NAME,           // Name to display
        SERVICE_QUERY_STATUS,           // Desired access
        SERVICE_WIN32_OWN_PROCESS,      // Service type
        SERVICE_AUTO_START,             // Service start type
        SERVICE_ERROR_NORMAL,           // Error control type
		szModulePath,                   // Service's binary
        NULL,                           // No load ordering group
        NULL,                           // No tag identifier
        NULL,                           // Dependencies
        NULL,                           // Service running account
        NULL                            // Password of the account
        );

	if(!hService)
	{
		wprintf(L"Error creating service: %s\n", GetErrorMsg().c_str());
		CloseServiceHandle(hSCManager);
		return -1;
	}

	CloseServiceHandle(hService);

	// Set service description
	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_CHANGE_CONFIG);
	if(hService)
	{
		SERVICE_DESCRIPTION sd;
		sd.lpDescription = SERVICE_DESC;
		if(!ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd))
		{
			wprintf(L"Error changing service config: %s\n", GetErrorMsg().c_str());
		}

		// Clean up
		CloseServiceHandle(hService);
	}
	else
	{
		wprintf(L"Error opening service for config change: %s\n", GetErrorMsg().c_str());
	}

	// Clean up
	CloseServiceHandle(hSCManager);

	printf("CrashFix service installed.\n");
	return 0;
}

int CDaemon::RemoveNTService()
{
	SERVICE_STATUS ssSvcStatus = {};

	// Open SCM
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if(!hSCManager)
	{
		// Print error message
		wprintf(L"Error opening Service Control Manager: %s.\n", GetErrorMsg().c_str());

		// Exit
		return 1;
	}

	// Open existing service
	SC_HANDLE hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_STOP |
        SERVICE_QUERY_STATUS | DELETE);
	if(!hService)
	{
		wprintf(L"Error opening service: %s\n", GetErrorMsg().c_str());
		CloseServiceHandle(hSCManager);
		return -1;
	}

	// Try to stop the service
    if(ControlService(hService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        wprintf(L"Stopping %s.", SERVICE_NAME);
        Sleep(1000);

        while (QueryServiceStatus(hService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                wprintf(L".");
                Sleep(1000);
            }
            else break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            wprintf(L"\n%s is stopped.\n", SERVICE_NAME);
        }
        else
        {
            wprintf(L"\n%s failed to stop.\n", SERVICE_NAME);
        }
    }

	// Remove service
	if(!DeleteService(hService))
	{
		DWORD dwError = GetLastError();
		wprintf(L"Error deleting service: %s\n", GetErrorMsg().c_str());
	}

	// Clean up
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	wprintf(L"CrashFix service removed.\n");
	return 0;
}

int CDaemon::StartNTService(BOOL bStart)
{
	// Open SCM
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(!hSCManager)
	{
		// Print error message
		wprintf(L"Error opening Service Control Manager: %s.\n", GetErrorMsg().c_str());

		// Exit
		return 1;
	}

	// Open existing service
	SC_HANDLE hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_START|SERVICE_STOP|SERVICE_INTERROGATE);
	if(!hService)
	{
		// Print error message
		wprintf(L"Error opening service: %s.\n", GetErrorMsg().c_str());

		// Exit
		return 1;
	}

	if(bStart)
	{
		// Start the service
		LPCWSTR aszArgs[1] = {L"--run-as-service"};
		if(!::StartService(hService, 0, NULL))
		{
			CloseServiceHandle(hSCManager);

			// Print error message
			wprintf(L"Error starting service: %s.\n", GetErrorMsg().c_str());

			return -1;
		}

		wprintf(L"Starting CrashFix service");

		// Check if service is running
		const int ATTEMPT_COUNT = 10;
		int i;
		for(i=0; i<ATTEMPT_COUNT; i++)
		{
			Sleep(1000);
			printf(".");

			SERVICE_STATUS ServiceStatus;
			if(!::ControlService(hService, SERVICE_CONTROL_INTERROGATE, &ServiceStatus))
			{
				CloseServiceHandle(hSCManager);
				// Print error message
				wprintf(L"\nError checking service status: %s.\n", GetErrorMsg().c_str());
				wprintf(L"\nError starting CrashFix service. Please refer to log file for more information.\n");
				return -1;
			}
			else
			{
				if(i==ATTEMPT_COUNT-1 || (
					ServiceStatus.dwCurrentState!=SERVICE_START_PENDING &&
					ServiceStatus.dwCurrentState!=SERVICE_RUNNING))
				{
					std::wstring err = GetErrorMsg();
					wprintf(L"\nError starting CrashFix service. Please refer to log file for more information.\n");
					return -1;
				}
				else break;
			}
		}


		wprintf(L"\nCrashFix service started.\n");
	}
	else
	{
		// Stop the service
		SERVICE_STATUS ServiceStatus;
		if(!::ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus))
		{
			CloseServiceHandle(hSCManager);
			// Print error message
			wprintf(L"Error stopping service: %s.\n", GetErrorMsg().c_str());
			return -1;
		}
		else
		{
			wprintf(L"CrashFix service stopped.\n");
		}
	}

	// Clean up
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return 0;
}

void CDaemon::EnterServiceMain()
{
	SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { SERVICE_NAME, ServiceMain },
        { NULL, NULL }
    };

    // Connects the main thread of a service process to the service control
    // manager, which causes the thread to be the service control dispatcher
    // thread for the calling process. This call returns when the service has
    // stopped. The process should simply terminate when the call returns.
    StartServiceCtrlDispatcher(serviceTable);
}

void WINAPI CDaemon::ServiceMain(DWORD dwArgc, PWSTR *pszArgv)
{
	int error = -1;
	int i = 0;

	m_ServiceStatus.dwServiceType    = SERVICE_WIN32_OWN_PROCESS;
	m_ServiceStatus.dwCurrentState    = SERVICE_START_PENDING;
	m_ServiceStatus.dwControlsAccepted  = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	m_ServiceStatus.dwWin32ExitCode   = 0;
	m_ServiceStatus.dwServiceSpecificExitCode = 0;
	m_ServiceStatus.dwCheckPoint     = 0;
	m_ServiceStatus.dwWaitHint      = 0;

	m_ServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME,
		(LPHANDLER_FUNCTION)ServiceControlHandler);
	if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		g_log.write(0, "Error registering service control handler (0x%X).\n", GetLastError());
		return;
	}

	// Set current folder
	std::wstring sDirName = GetModulePath(NULL);
	sDirName = GetParentDir(sDirName);
	SetCurrentDirectory(sDirName.c_str());

	CDaemon* pDaemon = CDaemon::GetInstance();

	// Report initial status to the SCM
    pDaemon->SetServiceStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

	// Init service
	pDaemon->SetMonitorOption(MO_IS_MONITOR, -1);
	pDaemon->ReadConfig();
	pDaemon->InitErrorLog();

	// Set status to Running
	pDaemon->SetServiceStatus(SERVICE_RUNNING);

	// Launch another process
	int nPid = 0;
	if(0==pDaemon->LaunchDaemonProcess(nPid))
	{
		// Enter the monitoring loop
		pDaemon->SetMonitorOption(MO_IS_MONITOR, nPid);
		pDaemon->Run();
	}

	// Set status to stopped
	CDaemon::GetInstance()->SetServiceStatus(SERVICE_STOPPED);
	return;
}

void CDaemon::ServiceControlHandler(DWORD request)
{
	switch(request)
	{
	case SERVICE_CONTROL_STOP:
		g_log.write(0, "Stopping the service.\n");

		// Tell SCM that the service is stopping.
        CDaemon::GetInstance()->SetServiceStatus(SERVICE_STOP_PENDING);

		SetEvent(CDaemon::GetInstance()->m_hEventStop);

		return;

	case SERVICE_CONTROL_SHUTDOWN:
		g_log.write(0, "Shutdown.\n");

		// Stop server
		SetEvent(CDaemon::GetInstance()->m_hEventStop);

		return;

	default:
		break;
	}

	return;
}

void CDaemon::SetServiceStatus(DWORD dwCurrentState,
                                    DWORD dwWin32ExitCode,
                                    DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure of the service.

    m_ServiceStatus.dwCurrentState = dwCurrentState;
    m_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    m_ServiceStatus.dwWaitHint = dwWaitHint;

    m_ServiceStatus.dwCheckPoint =
        ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED)) ?
        0 : dwCheckPoint++;

    // Report the status of the service to the SCM.
    ::SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus);
}

DWORD WINAPI CDaemon::StopWaitingThread(LPVOID lpParam)
{
	CDaemon* pDaemon = (CDaemon*)lpParam;

	// Wait for stop event
	WaitForSingleObject(pDaemon->m_hEventStop, INFINITE);

	pDaemon->GetLog()->write(0, "Stop event.\n");

	// Stop server
	CDaemon::GetInstance()->GetSocketServer()->Terminate();

	// and exit the thread procedure
	return 0;
}

#endif //_WIN32
