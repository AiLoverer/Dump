//! \file SocketServer.cpp
//! \brief Socket server implementation.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "SocketServer.h"
#include "Log.h"
#include "Daemon.h"
#include "Misc.h"
#include "strconv.h"
#include "Outputter.h"
#include "LibDumperVersion.h"


CSocketServer::CSocketServer()
{
    m_pDaemon = NULL;
    m_ServerSock = 0;
    m_nServerPort = -1;
    m_nMaxRequestQueueSize = 5;
    m_nTotalThreads = 2;
    m_nBusyThreads = 0;
    m_bRunning = false;
    m_pLog = NULL;
    m_pPollingThread = NULL;
	// Init command ID seed number
	m_nAssyncCommandIdSeed = 0;

#ifdef _WIN32
    // In Windows, we have to initialize Winsock library.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    assert(iResult==0);
    iResult;
#endif
}

CSocketServer::~CSocketServer()
{
#ifdef _WIN32
    // In Windows, we have to clean up the winsock library
    int iResult = WSACleanup();
    iResult;
    assert(iResult==0);
#endif
}

// This function creates server socket
bool CSocketServer::Init(CDaemon* pDaemon, int nPort, int nMaxQueueSize, int nTotalThreads, int nMaxMemUsageMB, CLog* pLog)
{
    m_sErrorMsg = "Unspecified error.";
	m_pDaemon = pDaemon;
    m_nServerPort = nPort;
    m_nMaxRequestQueueSize = nMaxQueueSize;
	m_nTotalThreads = nTotalThreads;
	m_nMaxMemUsageMB = nMaxMemUsageMB;
	m_pLog = pLog;

    /* Create the TCP socket */
    if ((m_ServerSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        m_sErrorMsg = "Failed to create server socket.";
        return false;
    }

    /* Enable address reuse */
    int on = 1;
    if(-1==setsockopt(m_ServerSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on) ))
    {
        m_pLog->write(0, "Couldn't enable address reuse, continuing anyway.\n");
        m_pLog->log_last_error(0, "setsockopt function returned an error");
    }

    /* Construct the server sockaddr_in structure */
    memset(&m_Server, 0, sizeof(sockaddr_in));         /* Clear struct */
    m_Server.sin_family = AF_INET;                     /* Internet/IP */
    m_Server.sin_addr.s_addr = htonl(INADDR_ANY);      /* Incoming addr */
    m_Server.sin_port = htons(m_nServerPort);          /* server port */

    /* Bind the server socket */
    if (bind(m_ServerSock, (struct sockaddr *) &m_Server, sizeof(sockaddr_in)) < 0)
    {
        std::ostringstream err_msg;
        err_msg << "Failed to bind the server socket to port ";
        err_msg << m_nServerPort;
        m_sErrorMsg = ((char*)err_msg.str().c_str());
        CLOSESOCK(m_ServerSock);
        return false;
    }

    /* Listen on the server socket */
    if (listen(m_ServerSock, 5) < 0)
    {
        m_sErrorMsg = "Failed to listen on server socket.";
        CLOSESOCK(m_ServerSock);
        return false;
    }

    /* Create worker threads. */
	InitThreadPool();

	// Init PDB cache
	std::wstring sPdbSearchDir = strconv::utf82w(m_pDaemon->GetWebRootDir());
#ifdef _WIN32
	sPdbSearchDir += L"protected\\data\\debugInfo";
#else
	sPdbSearchDir += L"protected/data/debugInfo";
#endif
	bool bAdd = m_PdbCache.AddPdbSearchDir(sPdbSearchDir, PDB_USUAL_DIR, true);
	if(!bAdd)
	{
		m_sErrorMsg = "Failed to init debug info cache in directory ";
		m_sErrorMsg += strconv::w2utf8(sPdbSearchDir);
        CLOSESOCK(m_ServerSock);
        return false;
	}

	// Set max mem usage (convert to KB).
	m_PdbCache.SetMaxMemUsage(m_nMaxMemUsageMB*1024);

    // Done
    m_sErrorMsg = "Success.";
    return true;
}

void CSocketServer::InitThreadPool()
{
    m_bRunning = true;
    m_nBusyThreads = 0;

    // Create threads for thread pool
    int i;
    for(i=0; i<m_nTotalThreads; i++)
    {
        CServerThread* pThread = new CServerThread(this, i);
        pThread->Run(NULL);
        m_aThreads.push_back(pThread);
    }

    // Create the polling thread - it will poll the webapp periodically.
	m_pPollingThread = new CSitePollingThread(this, m_pDaemon->GetSitePollCommand());
    m_pPollingThread->Run(NULL);
}

void CSocketServer::DestroyThreadPool()
{
    // This method destroys all threads in the thread pool
    CAutoLock lock(&m_Lock);

    int i;
    for(i=0; i<m_nTotalThreads; i++)
    {
        // Wait until thread exits.
        m_aThreads[i]->Wait();

        // Delete thread
        delete m_aThreads[i];
    }

    // Clear thread list
    m_aThreads.clear();
}

int CSocketServer::GetSleepingThreadCount()
{
    // Returns count of threads that are currently sleeping.
    CAutoLock lock(&m_Lock);
    assert(m_nTotalThreads>=m_nBusyThreads);
    return m_nTotalThreads - m_nBusyThreads;
}

void CSocketServer::Terminate()
{
    // This function is called by the daemon when SIGTERM signal is received.
    // Here we close all opened sockets and return.

    m_pLog->write(0, "Terminating server...\n");

    /* Closing server socket. */

    CLOSESOCK(m_ServerSock);

    /* Close all opened sockets in thread pool */

    m_pLog->write(0, "Freeing all server resources.\n");

	CAutoLock lock(&m_Lock);

    // Close sockets that are pending execution in thread pool.
    int i;
    for(i=0; i<m_nTotalThreads; i++)
    {
        CLOSESOCK(m_aThreads[i]->GetSocket());
    }

    /* Close all sockets awaiting in request queue. */

    while(m_RequestQueue.size()>0)
    {
        SOCK sock = m_RequestQueue.front().m_Sock;
        CLOSESOCK(sock);
        m_RequestQueue.pop();
    }
}

void CSocketServer::Run()
{
    // This method accepts incoming requests in loop and passes them to queue. Then it signals conditional variable
    // to wake up a worker thread for further request processing.

    while (1)
    {
        SOCK client_sock;
		sockaddr_in client;

#ifdef _WIN32
        int clientlen = sizeof(sockaddr_in);
#else
		socklen_t clientlen = sizeof(sockaddr_in);
#endif

        /* Wait for client connection */

        while ((client_sock = accept(m_ServerSock, (struct sockaddr *) &client, &clientlen)) < 0)
        {
            // Failed to accept client connection
            m_pLog->write(0, "Failed to accept client connection.\n");
            m_pLog->log_last_error(0, "accept failed");
            m_pDaemon->AddError(true, "A network error encountered (couldn't accept client connection).");
            Sleep(1000);
        }

        m_pLog->write(2, "Incoming client connection on socket %d...\n", client_sock);

        // Set socket to be non-blocking.
#ifdef _WIN32
        u_long on = 1;
#else
		int on = 1;
#endif
        if(IOCTLSOCKET(client_sock, FIONBIO, &on)<0)
        {
            m_pLog->write(0, "Couldn't enable non-blocking socket mode: ioctl() failed.\n");
            CLOSESOCK(client_sock);
            return;
        }

		// Add a new request to the queue
        AddRequest(client_sock, NULL, NULL);
    }
}

bool CSocketServer::IsRunning()
{
    // This method returns the value of running flag.

    CAutoLock lock(&m_Lock);
    return m_bRunning;
}

int CSocketServer::SendMsgWithTimeout(SOCK sock, const char* msg, double timeout, std::string& sErrorMsg)
{
    // This method sends the message string over sock socket with timeout.
    // It blocks until either message is sent, or until timeout exceeds.

    sErrorMsg = "Unspecified error.";

    // Get current time
    double write_start_time = microtime();

    // Count of bytes written
    int bytes_written = 0;

    int flags = 0;

#ifdef _WIN32

#else
    flags = MSG_DONTWAIT;
#endif

    int nMsgLen = (int)strlen(msg);

    while(bytes_written<nMsgLen)
    {
	    fd_set wfds;
		struct timeval tv;

		// Add socket to fd_set.
		FD_ZERO(&wfds);
		FD_SET(sock, &wfds);

		// Set timeout.
		tv.tv_sec = 5;
		tv.tv_usec = 0;

        // Waiting for socket ready state.
        int nfds = 0;
#ifndef _WIN32
        nfds = sock+1;
#endif

        int retval = select(nfds, NULL, &wfds, NULL, &tv);
        if(retval < 1)
        {
            sErrorMsg = "Select error - no data available.";
            return -1;
        }

        // Now socket is ready, write data
        int written = send(sock, msg + bytes_written, (int)nMsgLen - bytes_written, flags);
        if(written<0
#ifndef _WIN32
			&& (errno!=EAGAIN || errno!=EWOULDBLOCK)
#endif
			)
        {
            sErrorMsg = "Send error - can't send data.";
            return -1;
        }

        // Get current time
        double cur_time = microtime();

        // Get duration in seconds
        double duration = (cur_time-write_start_time)/1000;
        if(duration>timeout)
        {
            sErrorMsg = "Timeout exceeded.";
            return -2;
        }

        // Update count of bytes written
        if(written>0)
            bytes_written += written;
    }

    sErrorMsg = "Success.";
    return 0;
}

int CSocketServer::RecvMsgWithTimeout(SOCK sock, double timeout, unsigned uMaxSize, std::string& sMsg, std::string& sError)
{
    // This method reads the data of maximum uMaxSize size from socket sock.
    // The end of data is determined by zero character '\0'.
    // The method blocks until either all available data read ('\0' encountered), or uMaxSize reached, or timeout exceeded.
    // On output, it produces a string sMsg containing the data being read.

    int nStatus = -1;
    int nBytesRead = 0;
    const int BUFF_SIZE = 1024;
    char buf[BUFF_SIZE];
    int nfds = 0; // One file descriptor
    int nRetVal = -1;
    int nReceived = 0;
    int i;
    bool bFoundTerminator = false;
    double read_start_time = 0;
    double cur_time = 0;

    sError = "Unspecified error";

    if(sock==0 || timeout<0 || uMaxSize==0)
    {
        sError = "Invalid input parameter";
        goto exit; // Invalid parameter
    }


    // Init output
    sMsg.clear();

    // Current time stamp.
    read_start_time = microtime();

    // Wait for data and read available data chunks
    while(1)
    {
        fd_set rfds;
        struct timeval tv;

        // Add socket to fd_set.
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        // Set timeout.
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int flags = 0;
#ifdef _WIN32
#else
        flags = MSG_DONTWAIT;
#endif
        // Waiting for incoming data.
#ifndef _WIN32
        nfds = sock+1;
#endif
        nRetVal = select(nfds, &rfds, NULL, NULL, &tv);
        if(nRetVal < 1)
        {
            // Select error - no data available
            sError = "Select error - no data to receive";
            goto exit;
        }

        // Now data available, read it.
        memset(buf, 0, BUFF_SIZE);
        nReceived = recv(sock, buf, BUFF_SIZE-1, 0);
        if(nReceived < 0)
        {
            // recv error - no data read
            sError = "Recv error - no data to read";
            goto exit;
        }

        // Get current time
		cur_time = microtime();

        // Get duration in seconds
        double duration = (cur_time-read_start_time)/1000;
        if(duration>5)
        {
            sError = "Timeout exceeded";
            goto exit;
        }

        // Update bytes received count
        nBytesRead += nReceived;

        if(nBytesRead>(int)uMaxSize)
        {
            // Max data size reached
            sError = "Maximum data size reached";
            goto exit;
        }

        // Append this data chunk to message
        sMsg += std::string(buf, nReceived);

        // Look for termination character.
        for(i=0; i<nReceived; i++)
        {
            if(buf[i]=='\n')
            {
                // Found terminator
                if(i!=nReceived-1)
                {
                    sError = "Encountered more data after terminator character";
                    goto exit;
                }

                bFoundTerminator = true;
                break;
            }
        }

        if(bFoundTerminator)
            break;
    }

    sError = "Success";
    nStatus = 0;

exit:

    if(nStatus!=0)
    {
        sMsg.clear();
    }

    return nStatus;
}

int CSocketServer::GetServerStatus(std::string& err_msg)
{
    // This method checks the server state and returns the status code
    // followed by the status message.

    int nStatus = R_UNEXPECTED_ERROR; // Status code.
    err_msg = "Unspecified error.";   // Status message.

    int nProblemCount = 0;            // Count of problems detected.
    std::ostringstream error_list;    // The list of problems.

    // Acquire synchronisation lock.
    CAutoLock lock(&m_Lock);

    // Check request queue size. If the queue has exceeded its maximum size,
    // issue error message.
    if((int)m_RequestQueue.size()==m_nMaxRequestQueueSize)
    {
        nProblemCount++;
        error_list << "Pending requests queue has exceeded its maximum limit of " << m_nMaxRequestQueueSize << ".";
        error_list << ";";
    }

    // Check daemons' error list (polling thread may write errors there).
    int nErrorCount = m_pDaemon->GetErrorCount();
    int i;
    for(i=0; i<nErrorCount; i++)
    {
        std::string sError;
        if(!m_pDaemon->GetError(i, sError))
            continue;

        nProblemCount++;
        error_list << sError;
        error_list << ";";
    }

    // Clear daemon's error list
    m_pDaemon->ClearErrors();

    // Get current time.
    time_t cur_time;
    time(&cur_time);

    // Maximum request execution time
    const int MAX_REQUEST_TIME = 60*5;

    // Walk through threads in pool and check each thdead's wake up time.
    // If a thread has exceeded maximum wait time, issue an error.
    for(i=0; i< m_nTotalThreads; i++)
    {
        if(!m_aThreads[i]->IsBusy())
            continue; // Skip sleeping threads.

        // Get the last wake up time of the thread.
        time_t StartTime = m_aThreads[i]->GetStartTime();

        // Format the time as string
        char szDateTime[64] = "";
        struct tm * timeinfo = NULL;
        timeinfo = gmtime( &StartTime );
		if(timeinfo)
			strftime(szDateTime, 64,  "%Y-%m-%dT%H:%M:%SZ ", timeinfo);

        // If thread executes request too long, issue an error.
        if(difftime(cur_time, StartTime)>=MAX_REQUEST_TIME)
        {
            nProblemCount ++;
            error_list << "Thread " << i << " doesn't respond since " << szDateTime << ").";
            error_list << ";";
        }
    }

    // If there were any problems...
    if(nProblemCount>0)
    {
        // Format message
        std::string err_str = error_list.str();
        err_str = err_str.substr(0, err_str.length()-2); // remove last "; "

        // Set error status
        nStatus = R_UNEXPECTED_ERROR;
        err_msg = err_str.c_str();
        goto exit;
    }

    err_msg = "The daemon is functioning correctly, no problems detected.";
    nStatus = R_SUCCESS;

exit:

    return nStatus;
}

int CSocketServer::GetServerLicenseInfo(LPCWSTR szOutFile, std::string& sErrMsg)
{
	// This method gets license info and returns it as XML.

    int nStatus = R_UNEXPECTED_ERROR; // Status code.
    sErrMsg = "Unspecified error.";   // Status message.
	FILE* f = NULL;
	COutputter doc;
	char szBuffer[1024];
	time_t CreationDate = 0;
	std::string sDateCreated;
	int nDaysExpiresFromNow = -1;
	std::string sFeatureType;

    if(szOutFile==NULL)
	{
		sErrMsg = R_INVALID_PARAM;
		goto exit;
	}


#ifdef _WIN32
	fopen_s(&f, strconv::w2a(szOutFile).c_str(), "wt");
#else
	f = fopen(strconv::w2utf8(szOutFile).c_str(), "wt");
#endif

	if(f==NULL)
	{
		sErrMsg = "Error opening output file for writing";
		goto exit;
	}

	doc.Init(f, OUTPUT_XML);

	doc.BeginDocument("LicenseInfo");

	doc.BeginSection("General");

	Time2String(CreationDate, sDateCreated);
	doc.PutRecord("DateCreated", sDateCreated.c_str());

	sprintf(szBuffer, "%d", nDaysExpiresFromNow);
	doc.PutRecord("DaysExpiresFromNow", szBuffer);

	sprintf(szBuffer, "%d", 0);
	doc.PutRecord("Evaluation", szBuffer);

	doc.EndSection();

	doc.BeginSection("ProductInfo");

	doc.PutRecord("ProductName", "CrashFix");

	sprintf(szBuffer, "%d.%d", 1, 0);
	doc.PutRecord("ProductVersion", szBuffer);

	sFeatureType = "Professional";
	
	doc.PutRecord("Edition", sFeatureType.c_str());

	doc.EndSection();

	doc.BeginSection("CustomerInfo");

	doc.PutRecord("Name", "");
	doc.PutRecord("Surname", "");
	doc.PutRecord("CompanyName", "");
	doc.PutRecord("Country", "");
	doc.PutRecord("State", "");
	doc.PutRecord("City", "");
	doc.PutRecord("PostalCode", "");
	doc.PutRecord("Address", "");

	doc.EndSection();

	doc.EndDocument();

	sErrMsg = "Success";
	nStatus = R_SUCCESS;

exit:

	if(f!=NULL)
		fclose(f);

	return nStatus;
}


int CSocketServer::GetServerConfigInfo(LPCWSTR szOutFile, std::string& sErrMsg)
{
	// This method gets config info and returns it as XML.

    int nStatus = R_UNEXPECTED_ERROR; // Status code.
    sErrMsg = "Unspecified error.";   // Status message.
	FILE* f = NULL;
	COutputter doc;
	char szBuffer[1024];

    if(szOutFile==NULL)
	{
		sErrMsg = R_INVALID_PARAM;
		goto exit;
	}

#ifdef _WIN32
	fopen_s(&f, strconv::w2a(szOutFile).c_str(), "wt");
#else
	f = fopen(strconv::w2utf8(szOutFile).c_str(), "wt");
#endif

	if(f==NULL)
	{
		sErrMsg = "Error opening output file for writing";
		goto exit;
	}

	doc.Init(f, OUTPUT_XML);

	doc.BeginDocument("ConfigInfo");

	doc.BeginSection("Daemon");

	sprintf(szBuffer, "%d.%d.%d", LIBDUMPER_VER_MAJOR, LIBDUMPER_VER_MINOR, LIBDUMPER_VER_BUILD);
	doc.PutRecord("DaemonVer", strconv::a2utf8(szBuffer).c_str());

	doc.PutRecord("WebRootDir", strconv::a2utf8(m_pDaemon->GetWebRootDir()).c_str());

	doc.EndSection();

	doc.EndDocument();

	sErrMsg = "Success";
	nStatus = R_SUCCESS;

exit:

	if(f!=NULL)
		fclose(f);

	return nStatus;
}

std::string CSocketServer::GetErrorMsg()
{
    // Return the last error
    return m_sErrorMsg;
}

bool CSocketServer::AddRequest(SOCK& Sock, const char* szCommand, std::string* psCmdId)
{
	// Validate input
	if(Sock==0 && szCommand==NULL)
		return false;

	if(psCmdId!=NULL)
		*psCmdId = "";

	// Lock sync object
    m_Lock.Lock();

    // Check queue size - is it full?
    if((int)m_RequestQueue.size()>=m_nMaxRequestQueueSize)
    {
        m_Lock.Unlock();

        m_pLog->write(0, "The request queue size has exceeded its maximum (too many requests).\n");
        m_pDaemon->AddError(false, "The request queue size has exceeded its maximum (too many requests).");
        std::string sErrorMsg;
        SendMsgWithTimeout(Sock, "102 Too many requests.\n", 5, sErrorMsg);
		if(Sock!=0)
			CLOSESOCK(Sock); // denial of service
    }
    else
    {
        // Add this request to pending requests queue.

        m_pLog->write(2, "Adding request into message queue...\n");

        // Prepare request info
        RequestInfo request;
		if(Sock!=0)
			request.m_Sock = Sock;
		else
		{
#ifdef _WIN32
			int nPid = _getpid();
#else
			int nPid = getpid();
#endif
			char szCmdId[32];
			sprintf(szCmdId, "%d.%d", nPid, ++m_nAssyncCommandIdSeed);
			request.m_sCommandId = szCmdId;
			request.m_sCommand = szCommand;

			// Add assync command info record
			AddAssyncCommandInfo(request.m_sCommandId.c_str(), szCommand);
			if(psCmdId!=NULL)
				*psCmdId = request.m_sCommandId;
		}

        request.m_dIncomingTime = microtime();

        // Add request to the queue
        m_RequestQueue.push(request);

        int nReadyThreads = m_nTotalThreads-m_nBusyThreads;
        int nSignalCount = MIN(nReadyThreads, (int)m_RequestQueue.size());

        m_Lock.Unlock();

        // Wake up as many threads as needed to process as many requests as possible.
        int i;
        for(i=0; i<nSignalCount; i++)
        {
            // Signal conditional variable
            m_pLog->write(2, "Signaling conditional variable.\n");

            // This will wake up a thread
            m_Cond.Signal();
        }
    }

	// Done.
	return true;
}

bool CSocketServer::GetRequest(SOCK& Sock, double& IncomingTime, std::string& sCmdId, std::string& sCommand)
{
    // This method gets the top request from the queue and returns the socket number and timestamp of the request.

    CAutoLock lock(&m_Lock);

    // Check queue size
    if(m_RequestQueue.size()==0)
        return false; // Queue is empty

    // Get the socket from the front of the queue
    Sock = m_RequestQueue.front().m_Sock;
    // Get the timestamp of incoming request
    IncomingTime = m_RequestQueue.front().m_dIncomingTime;
	// Get command line
	sCommand = m_RequestQueue.front().m_sCommand;
	sCmdId = m_RequestQueue.front().m_sCommandId;

    // Remove request from the queue
    m_RequestQueue.pop();

    return true;
}

bool CSocketServer::AddAssyncCommandInfo(const char* szCmdId, const char* szCommand)
{
	// Validate input
	if(szCmdId==NULL || szCommand==NULL)
		return false;

	CAutoLock lock(&m_Lock);

	// Fill structure fields
	AssyncCommandInfo CmdInfo;
	CmdInfo.m_sCmdId = szCmdId;
	CmdInfo.m_sCommand = std::string(szCommand);
	CmdInfo.m_bCompleted = false;
	CmdInfo.m_nStatus = -1;
	CmdInfo.m_sErrorMsg = "Unspecified error.";

	// Add to the list
	m_aAssyncCommandInfo[szCmdId] = CmdInfo;

	// OK.
	return true;
}

bool CSocketServer::UpdateAssyncCommandInfoOnCompletion(const char* szCmdId, int nCompletionStatus, std::string& sErrorMsg)
{
	// Acquire synchronisation lock.
    CAutoLock lock(&m_Lock);

	// Look for such a command ID
	std::map<std::string, AssyncCommandInfo>::iterator it = m_aAssyncCommandInfo.find(szCmdId);
	if(it==m_aAssyncCommandInfo.end())
		return false; // Command not found

	AssyncCommandInfo& CmdInfo = it->second;

	// Update fields
	CmdInfo.m_bCompleted = true;
	CmdInfo.m_nStatus = nCompletionStatus;
	CmdInfo.m_sErrorMsg = sErrorMsg;

	// Done.
	return true;
}

int CSocketServer::GetAssyncCommandInfo(std::string& sErrorMsg, const char* szCommandId, bool bEraseCompleted)
{
	// This method return the status of assynchronous commands.

    int nStatus = R_UNEXPECTED_ERROR; // Status code.
    sErrorMsg = "Unspecified error.";   // Status message.
	std::ostringstream result;

    // Acquire synchronisation lock.
    CAutoLock lock(&m_Lock);

	bool bFound = false;
	std::set<std::string> aItemsToErase;
	std::map<std::string, AssyncCommandInfo>::iterator it;
	for(it=m_aAssyncCommandInfo.begin(); it!=m_aAssyncCommandInfo.end(); it++)
	{
		if(szCommandId!=NULL && szCommandId!=it->first)
			continue;

		bFound = true;

		result << "Command {" << it->second.m_sCmdId;
		result << "} ";
		if(!it->second.m_bCompleted)
		{
			result << "is still executing.";
		}
		else
		{
			result << "returned {" << it->second.m_nStatus << " " << it->second.m_sErrorMsg;
		}

		result <<"};";

		if(bEraseCompleted && it->second.m_bCompleted)
			aItemsToErase.insert(it->first);
	}

	std::set<std::string>::iterator it2;
	for(it2=aItemsToErase.begin(); it2!=aItemsToErase.end(); it2++)
	{
		std::string sCmdId = *it2;
		it = m_aAssyncCommandInfo.find(sCmdId);
		if(it!=m_aAssyncCommandInfo.end())
			m_aAssyncCommandInfo.erase(it);
	}

	if(!bFound)
	{
		nStatus = 1;
		sErrorMsg = "Such a command ID not found.";
		goto exit;
	}

	nStatus = 0;
	sErrorMsg = result.str();

exit:

	return nStatus;
}
