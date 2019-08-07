#pragma once
#include "stdafx.h"
#include "Thread.h"
#include "CritSec.h"
#include "Log.h"

#ifdef _WIN32
typedef SOCKET SOCK;
#define CLOSESOCK(x) closesocket(x)
#else
typedef int SOCK;
#define CLOSESOCK(x) close(x)
#endif

class CMtClient;

//! Client thread class.
class CMtClientThread : public CThread
{
public:

    //! Constructor
    CMtClientThread(CMtClient* pClient, int nThreadId, CLog* pLog);

private:

    long ThreadProc(void* pParam);
        
	//! Executes a single request to server.
    int ExecuteTestRequest(bool bAssync, const char* szCmdLine, std::set<int> aExpectedRetCodes);

    int m_nThreadId;        //!< Thread Id
    CMtClient* m_pClient;   //!< Owner client
	CLog* m_pLog;           //!< Log
    int m_nSleepTime;       //!< Time to sleep
};

//! Multi-threaded client.
class CMtClient
{
    friend class CMtClientThread;
public:

    //! Constructor.
	//! @param[in] sHost Host address.
    //! @param[in] nPort Port address.    
    CMtClient(std::string sHost, int nPort);

    //! Destructor.
    ~CMtClient();

    //! Runs the multi-threaded client.
    //! @param[in] nThreadCount Count of threads.
    //! @param[in] nDuration Test duration (in msec).
    bool Run(int nThreadCount, int nDuration, std::string& sErrorMsg);

    //! Adds a request to the list of requests.
	//! \return Returns the zero-based request index in the list.
	//! @param[in] sRequest Request command line.
	//! @param[in] nExpectedRetCode Expected return code.
    int AddRequest(std::string sRequest, int nExpectedRetCode);

	//! Adds an assync request to the list of requests.
	int AddAssyncRequest(std::string sRequest, int nExpectedRetCode);

	//! Removes a request from the list by its zero-based index.
	//! @param[in] nIndex Zero-based request index.
	bool DelRequest(int nIndex);
	
	//! Executes a single request to server.
	bool ExecuteRequest(const char* szCmdLine, int& nServerRetCode, std::string& sServerResponse, std::string& sErrorMsg);

private:

    //! Initializes error log
    bool InitErrorLog(std::string& sErrorMsg);

	//! Sends a message with timeout.
	int SendMsgWithTimeout(SOCK sock, const char* msg, double timeout, std::string& sErrorMsg);

	//! Receives a message with timeout.
    int RecvMsgWithTimeout(SOCK sock, double timeout, unsigned uMaxSize, std::string& sMsg, std::string& sError);

    //! Request info
    struct RequestInfo
    {
		RequestInfo()
		{
			m_bAssync = false;
			m_nExpectedRetCode = 0;
		}

		bool m_bAssync;         //!< Async request or not
        std::string m_sCmdLine; //!< Command line
        int m_nExpectedRetCode; //!< Expected return code
    };

	// Configuration related
    std::string m_sHost;         //!< Host name
    int m_nPort;                 //!< Port number
    int m_nThreadCount;          //!< Count of worker threads
    int m_nDuration;             //!< Connection duration
    std::vector<CMtClientThread*> m_aThreads; //!< Worker threads
    CCritSec m_Lock;             //!< Sync object
    std::vector<RequestInfo> m_aRequests; //!< Array of requests
	CLog m_Log;                  //!< Log.
    std::string m_sErrorLogFile; //!< Error log path.
    int m_nLoggingLevel;         //!< Logging level.

	// Test results
    unsigned int m_uRequestsSucceeded;    //!< Total requests succeeded
    unsigned int m_uRequestsFailed;       //!< Total requests failed
    unsigned int m_uRequestsRejected;     //!< Total requests rejected
    unsigned int m_uSocketConnectionErrors; //!< Count of socket connection errors
    double m_dTotalTestTime;  //!< Test actual duration
    unsigned int m_uTotalRequestsProcessed; //!< Total requests processed

};
