//! \file SocketServer.h
//! \brief Socket server implementation.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "CritSec.h"
#include "ServerThread.h"
#include "CommandProcessor.h"
#include "PdbCache.h"

// Forward declarations
class CLog;
class CDaemon;

//! This structure describes client request.
//! Requests may be of two types - incoming data parsing requests and command execution requests.
//! If socket number is non-zero, the working thread should read the data from the socket, parse them and
//!  1. execute the sequence of commands (for synchronous command) or
//!  2. append new command execution request(s) to the queue (for assynchronous command).
//! If socket number is zero, the worker thread should execute the assynchronous command.
struct RequestInfo
{
	//! Constructor.
	RequestInfo()
	{
		m_Sock = 0;
		m_dIncomingTime = 0;		
	}

	SOCK m_Sock;              //!< Socket number.
	double m_dIncomingTime;   //!< Incoming time stamp.
	std::string m_sCommandId; //!< Unique ID of the assync command.
	std::string m_sCommand;   //!< Command line.
};

//! This structure describes an assynchronous command execution status.
struct AssyncCommandInfo
{
	//! Constructor.
	AssyncCommandInfo()
	{		
		m_bCompleted = false;
		m_nStatus = -1;
		m_sErrorMsg = "Unspecified error.";
	}

	std::string m_sCmdId;     //!< Command ID.
	bool m_bCompleted;        //!< Whether the command completed or still executing?
	int m_nStatus;            //!< Command completion status.
	std::string m_sCommand;   //!< Command line.
	std::string m_sErrorMsg;  //!< Error message returned by the command.
};

//! \class CSocketServer
//! \brief A socket server that listens for client requests and exequtes them on different threads.
class CSocketServer
{
    friend class CServerThread;
    friend class CSitePollingThread;
public:

    //! Constructor
    CSocketServer();

    //! Destructor
    virtual ~CSocketServer();

    //! Initializes the socket server.
    //! @param[in] pDaemon Pointer to owner daemon.
    //! @param[in] nPort   Port number.
    //! @param[in] nMaxQueueSize Maximum size of request queue.
    //! @param[in] nTotalThreads Total count of concurrent threads.
    //! @param[in] pLog Pointer to log object.
    bool Init(CDaemon* pDaemon, int nPort, int nMaxQueueSize, int nTotalThreads, int nMaxMemUsageMB, CLog* pLog);

    //! Terminates the socket server (closes all socket connections).
    void Terminate();

    //! Runs the socket server (enters the connection listening loop).
    void Run();

    //! Returns true if running.
    bool IsRunning();

    //! Returns count of threads that are ready to process client's requests.
    int GetSleepingThreadCount();

    //! Returns the last error msg.
    std::string GetErrorMsg();

    //! Sends a zero-terminated message to socket and waits for response for a given timeout.
    //! \return Zero on success.
    //! @param[in] sock Socket.
    //! @param[in] msg  Message text.
    //! @param[in] timeout Timeout interval (in sec).
    //! @param[out] sErrorMsg Status message.
    int SendMsgWithTimeout(SOCK sock, const char* msg, double timeout, std::string& sErrorMsg);

    //! This method reads the data of maximum uMaxSize size from socket sock.
    //! The end of data is determined by zero character '\0'.
    //! The method blocks until either all available data read ('\0' encountered), or uMaxSize reached, or timeout exceeded.
    //! On output, it produces a string sMsg containing the data being read.
    //! \return Zero on success.
    //! @param[in] sock Socket.
    //! @param[in] timeout Timeout interval (in sec).
    //! @param[in] uMaxSize Maximum allowed response size (in bytes).
    //! @param[out] sMsg  Message text.
    //! @param[out] sError Status message.
    int RecvMsgWithTimeout(SOCK sock, double timeout, unsigned uMaxSize, std::string& sMsg, std::string& sError);

private:

    //! Creates the threads in thread pool.
    void InitThreadPool();

    //! Destroys all threads in the pool.
    void DestroyThreadPool();

    //! Returns server status code and error message.
    //! @param[out] err_msg Status code followed by the status message.
    int GetServerStatus(std::string& sErrMsg);

	//! Returns server license information.
    //! @param[out] err_msg Status code followed by the status message.
    int GetServerLicenseInfo(LPCWSTR szOutFile, std::string& sErrMsg);

	//! Returns server config information.
    //! @param[out] err_msg Status code followed by the status message.
    int GetServerConfigInfo(LPCWSTR szOutFile, std::string& sErrMsg);

	//! Adds a new request to the request queue.
	//! @param[in] Sock Socket number (if non-zero, command line is ignored).
	//! @param[in] szCommand Command line (used if socket number is zero).
	//! @param[out] psCommandId Optional. If set, receives the assync command ID (used if socket number is zero).
	bool AddRequest(SOCK& Sock, const char* szCommand, std::string* psCommandId);

    //! This method gets the top request from the queue and returns the socket number, command and timestamp of the request.
    //! @param[out] Sock Socket number.
    //! @param[out] IncomingTime Request timestamp.
	//! @param[out] nCmdId Command ID.
	//! @param[out] sCommand Command line.	
	bool GetRequest(SOCK& Sock, double& dIncomingTime, std::string& sCmdId, std::string& sCommand);
       
	//! Adds an assync command info to internal list
	//! @param[in] sCommand Command line.
	bool AddAssyncCommandInfo(const char* szCmdId, const char* szCommand);

	//! Updates assync command information on command completion.
	//! @param[in] nCmdId Command id.	
	//! @param[in] nCompletionStatus Completion status.
	//! @param[in] sErrorMsg Error message.
	bool UpdateAssyncCommandInfoOnCompletion(const char* nCmdId, int nCompletionStatus, std::string& sErrorMsg);

	//! Returns the information on a single command or all assynchronous commands being processed.
	int GetAssyncCommandInfo(std::string& sErrorMsg, const char* szCommandId=NULL, bool bEraseCompleted=true);	

    /* Variables used internally */

	CDaemon* m_pDaemon;                   //!< Owner daemon.
    SOCK m_ServerSock;                    //!< Server socket.
    struct sockaddr_in m_Server;          //!< Sockaddr structure.
    int m_nServerPort;                    //!< Port number we listen on.
    int m_nMaxRequestQueueSize;           //!< Request queue size limit.
    int m_nTotalThreads;                  //!< Size of thread pool.
	int m_nMaxMemUsageMB;                 //!< Max mem usage (MB).
    std::string m_sErrorMsg;              //!< Last error message.
    std::vector<CServerThread*> m_aThreads; //!< Thread pool.
	CSitePollingThread* m_pPollingThread;   //!< Site polling thread.
    std::queue<RequestInfo> m_RequestQueue; //!< Request queue.
	std::map<std::string, AssyncCommandInfo> m_aAssyncCommandInfo;	//!< List of assync commands.	
	int m_nAssyncCommandIdSeed;           //!< Used to generate unique IDs for assync commands.
    int m_nBusyThreads;                   //!< Count of busy threads.
    bool m_bRunning;                      //!< Running flag.
    CLog* m_pLog;                         //!< Error log.
    CCritSec m_Lock;                      //!< Synchronisation object.
    CCondVar m_Cond;                      //!< Conditional variable (used to wake up a thread from pool).
	CPdbCache m_PdbCache;                 //!< PDB cache.	
};


