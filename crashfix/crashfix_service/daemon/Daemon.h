//! \file Daemon.h
//! \brief Daemon implementation.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "Log.h"
#include "SocketServer.h"
#include "strconv.h"
#include "base64.h"

#define PRODUCT_NAME_BASE64 "Y3Jhc2hmaXg" // base-64 encoded "crashfix"

// Error codes
#define R_SUCCESS 0
#define R_UNEXPECTED_ERROR 1
#define R_INVALID_PARAM 2

//! Monitor launching option
enum eMonitorOption
{
	MO_UNDEFINED,       //!< Undefined
    MO_NO_MONITOR,      //!< Do not launch monitor process
    MO_LAUNCH_MONITOR,  //!< Need to launch another monitor process
    MO_IS_MONITOR,      //!< This process is monitor
};

enum eDaemonizeType
{
   DT_SPAWN_DAEMON_PROCESS,  //!< Create new daemon process, then exit.
   DT_SPAWN_DAEMON_ON_CRASH, //!< Create new daemon process, then continue.
   DT_SPAWN_MONITOR_PROCESS, //!< Create new monitor process, then continue.
};

struct ErrorInfo
{
	ErrorInfo()
	{
		m_bCriticalError = FALSE;
		tmCreateTime = 0;
		tmUpdateTime = 0;
		tmSentTime = 0;
	}

	BOOL m_bCriticalError;
	time_t tmCreateTime;
	time_t tmUpdateTime;
	time_t tmSentTime;
};

// Forward declarations.
struct ProductInfo;
struct CustomerInfo;

//! \class CDaemon
//! \brief Implements daemon.
class CDaemon
{
public:

    //! Constructor
    CDaemon();

    //! Destructor
    ~CDaemon();

    //! Sets config file name.
	//! @param[in] sConfigFile File name of the configuration.
    void SetConfigFile(std::string sConfigFile);

    //! Sets prefix string. The prefix is prepended to all path parameters
	//! in the configuration.
	//! @param[in] sPrefix Prefix string.
    void SetPrefix(std::string sPrefix);

	//! Sets/resets restarted-after-crash flag.
	//! @param[in] bRestarted Value TRUE means the daemon was restarted by the monitor process after crash.
	void SetErrorRestartFlag(bool bRestarted);

    //! Sets monitor option (this info defines if we should launch monitor or we should be monitor ourselves).
	//! @param[in] opt This defines if we should launch monitor or we should be monitor ourselves
	//! @param[in] nPid Process ID to watch.
    void SetMonitorOption(eMonitorOption opt, int nPid);

	//! Returns path to web root directory. The web root directory is where
	//! the web app is located.
    std::string GetWebRootDir();

	//! Returns site polling command. The command is executed in loop by the daemon's polling thread.
	//! The command runs a PHP script which in turn executes requests to us.
	std::string GetSitePollCommand();

    //! Returns pointer to owned socket server object.
    CSocketServer* GetSocketServer();

    //! Returns monitor option.
    eMonitorOption GetMonitorOption();

    //! Returns process ID to monitor.
    int GetPidToMonitor();

	//! Installs the service (Windows).
	int Install();

	//! Removes the service (Windows).
	int Remove();

    //! Runs the daemon.
    void Run();

	//! Starts the daemon.
	int Start();

	//! Restarts the daemon
	int Restart();

	//! Stops the daemon
	int Stop();

	//! Returns a pointer to the daemon instance.
	static CDaemon* GetInstance();

	//! Returns a pointer to owned log object.
	CLog* GetLog();

	//! Adds a critical error or warning to the list of errors.
    //! @param[in] bCritical If set, the critical error flag is raised.
    //! @param[in] szErrorMsg Error message.
    void AddError(bool bCritical, const char* szErrorMsg);

    //! Clears the list of errors (if bOnlyRetired set, removes only retired errors; otherwise clears all list).
    void ClearErrors(bool bOnlyRetired=false);

    //! Gets count of errors
    int GetErrorCount();

    //! Returns i-th error
    bool GetError(int nIndex, std::string& sError);

#ifdef _WIN32
	//! Registers service main procedure and runs it
	void EnterServiceMain();
#endif

protected:

	//! Launches monitoring process. The monitoring process checks
	//! daemon's status in loop and notifies the webmaster on errors.
	//! It can also restart the daemon.
    void LaunchMonitorProcess();

	//! Runs monitoring loop. This function returns only when the daemon is restarted.
    void RunAsMonitor();

	//! This method prints the error message and terminates the process with error code.
	//! @param[in] szMessage Error message to print.
	//! @param[in] bUsePStr Also print last system error.
    void Die(const char* szMessage, bool bUsePStr);

    //! Adds a prefix to the file name.
    //! @param[in] sFileName File name to add the prefix to.
    void AddPrefix(std::string& sFileName);

    //! Reads parameters from config file. This function may terminate the process when can't read config file.
    void ReadConfig();

    //! Initializes error log file or monitor log file.
    void InitErrorLog();

    //! Initializes socket server (binds to server socket).
    void InitSocketServer();

    //! Runs socket server (enters the listening loop). This function returns when the server stops.
    void RunSocketServer();

    //! Checks if daemon already running.
    //! @return PID on success (if process is running); otherwise a negative number.
    int CheckProcessExists();

    //! Spawns new process in another user session (Linux).
    //! @param[in] Flag The flag determines how to create the new process.
    bool Daemonize(eDaemonizeType Flag);

    //! Writes the current process ID to pidfile.
    //! In Linux this is /var/run/crashfixd.pid.
    void SavePidToFile();

    //! Reads PID from pidfile.
    //! @return int process ID on success; otherwise -1.
    int ReadPidFile();

    //! Registers termination handler for the daemon process (Linux).
    void RegisterTermHandler();

    //! Shuts the process down on SIGTERM signal (Linux).
    static void TerminateHandler(int);

    //! Registers child termination handler (Linux).
    void RegisterCHLDHandler(bool bRegister);

    //! This handler is called when a child process terminates (Linux).
    static void SIGCHLDHandler(int);

    //! Executes a request to the daemon. Used by the polling thread to check daemon's status.
    //! \return Returns zero if request is sent successfully.
    //! @param[in] szCmdLine Command name and parameters.
    //! @param[out] sErrorMsg On error, this receives the error description message.
    //! @param[out] nServerRetCode Server return code.
    //! @param[out] asServerErrors The list of error messages returned by the server.
	int ExecuteClientRequest(const char* szCmdLine, std::string& sErrorMsg,
                          int& nServerRetCode, std::vector<std::string>& asServerErrors);

    //! Notifies the webmaster on a undesired event.
	void NotifyWebmasterOnError();

    //! Restarts the daemon on critical error.
	bool RestartDaemonOnCrash();

#ifdef _WIN32

	//! Starts another daemon process (Windows only).
	int LaunchDaemonProcess(int &nPid);

	//! Thread function that waits for stop event and terminates the server.
	static DWORD WINAPI StopWaitingThread(LPVOID lpParam);

	//! Service's main procedure
	static void WINAPI ServiceMain(DWORD dwArgc, PWSTR *pszArgv);

	//! Service's control handler function
	static void ServiceControlHandler(DWORD request);

	//! Installs CrashFix as a Windows service
	int InstallNTService();

	//! Removes CrashFix service.
	int RemoveNTService();

	//! Starts CrashFix service.
	int StartNTService(BOOL bStart=TRUE);

	// Set the service status and report the status to the SCM.
    void SetServiceStatus(DWORD dwCurrentState,
        DWORD dwWin32ExitCode = NO_ERROR,
        DWORD dwWaitHint = 0);

#endif

    // General config settings
    std::string m_sPrefix;             //!< Prefix (base directory path).
	std::string m_sLicenseFile;        //!< License file.
    std::string m_sPIDFile;            //!< PID file path.
    std::string m_sConfigFile;         //!< Config file path.
    int m_nThreadCount;                //!< Count of threads.
    int m_nMaxQueueSize;               //!< Maximum queue size.
    int m_nServerPort;                 //!< Socket server port.
	std::string m_sWebRootDir;         //!< The directory where index.php is located.
	std::string m_sPollCommand;        //!< Polling command
	bool m_bRestartedAfterCrash;       //!< Monitor has restarted us after our crash.

    // Logging related settings
    std::string m_sErrorLogFile;       //!< Error log path.
    int m_nLoggingLevel;               //!< Logging level (0-minimum, 2-maximum).
    int m_nErrorLogMaxSizeKB;          //!< Maximum size of error log file

	// PDB cache related settings
    int m_nCacheMaxEntries;            //!< Maximum entries in cache.
	int m_nCacheMaxMemUsageMB;         //!< Maximum memory consumption in MB.

    // Monitoring process related settings
    std::string m_sMonitorLogFile;     //!< Path to monitor.log.
    std::string m_sWebmasterEmail;     //!< Webmaster's email address.
    bool m_bLaunchMonitorProcess;      //!< Should we launch monitoring process when starting the daemon?
    std::string m_sSmtpServerHost;     //!< SMTP server host.
    int m_nSmtpServerPort;             //!< SMTP server port.
	std::string m_sSmtpLogin;          //!< SMTP login (optional).
	std::string m_sSmtpPassword;       //!< SMTP password (optional).
    bool m_bNotifyWebmasterOnErrors;   //!< Should we notify webmaster on errors?
    bool m_bRestartDaemonOnCrash;      //!< Should we restart daemon on errors?
    int m_nPidToMonitor;               //!< ID of the process to monitor.
    //bool m_bCriticalError;             //!< Were there any critical errors?
	CCritSec m_csLock;
    std::map<std::string, ErrorInfo> m_asErrors; //!< The list of detected errors (will be sent to webmaster as email).

    // Owned objects
    CLog m_Log;                        //!< Log.
    CSocketServer m_SocketServer;      //!< Socket server.
    eMonitorOption m_MonitorOption;    //!< Monitor option.
    static CDaemon* m_pInstance;       //!< Pointer to this daemon instance.
#ifdef _WIN32
	static SERVICE_STATUS m_ServiceStatus; //!< Status of NT service
	static SERVICE_STATUS_HANDLE m_ServiceStatusHandle; //!< Handle to NT service.
	HANDLE m_hEventStop;               //!< Event used to force daemon process to exit.
	bool m_bLogInitialized;            //!< Log init flag.
#else
	struct sigaction m_OldSIGCHLDAction; //!< Old SIGCHLD signal action
#endif
};

