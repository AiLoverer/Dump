#include "stdafx.h"
#include "MtClient.h"
#include "Misc.h"
#include "strconv.h"

//---------------------------------------------------------------
// CMtClient impl
//---------------------------------------------------------------

CMtClient::CMtClient(std::string sHost, int nPort)
{
	m_sHost = sHost;
    m_nPort = nPort;
    //m_uRequestsInTotal = 0;
    m_uRequestsSucceeded = 0;
    m_uRequestsFailed = 0;
    m_uRequestsRejected = 0;
    m_uSocketConnectionErrors = 0;
    m_dTotalTestTime = 0;
    m_uTotalRequestsProcessed = 0;
    m_nLoggingLevel = 0;

#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    assert(iResult==0);
    iResult;
#endif

    // Init random numbers generator.
    time_t seconds;
    time(&seconds);
    srand( (UINT)seconds );
}

CMtClient::~CMtClient()
{
    size_t i;
    for(i=0; i<m_aThreads.size(); i++)
    {
        delete m_aThreads[i];
    }
    m_aThreads.clear();

#ifdef _WIN32
    int iResult = WSACleanup();
    iResult;
    assert(iResult==0);
#endif
}

bool CMtClient::InitErrorLog(std::string& sErrorMsg)
{
    if(m_sErrorLogFile.empty())
        m_sErrorLogFile ="./daemon_tests.log";

    BOOL bInitLog = m_Log.init(strconv::a2w(m_sErrorLogFile), false);
    if(!bInitLog)
    {
        sErrorMsg = "Error initializing error log file: ";
        sErrorMsg += m_sErrorLogFile;
        return false;
    }

    // Write general server info to error.log
	m_Log.write(0, "===========================\n");
	m_Log.write(0, "Daemon Test Log\n");
	m_Log.write(0, "===========================\n");
	m_Log.write(0, "Configuration summary:\n");
    m_Log.write(0, "Total thread count = %d\n", m_nThreadCount);
    m_Log.write(0, "Port number = %d\n", m_nPort);
    m_Log.write(0, "Log path = %s\n", m_sErrorLogFile.c_str());

    // Set max log size to 10 MB
    m_Log.set_max_size(10*1024);
    // Perform log rotation if log is too large.
    m_Log.check_error_log_size();

	return true;
}

int CMtClient::AddRequest(std::string sRequest, int nExpectedRetCode)
{
	CAutoLock lock(&m_Lock);

    RequestInfo ri;
    ri.m_sCmdLine = sRequest;
    ri.m_nExpectedRetCode = nExpectedRetCode;

    m_aRequests.push_back(ri);

	return (int)(m_aRequests.size()-1);
}

int CMtClient::AddAssyncRequest(std::string sRequest, int nExpectedRetCode)
{
	CAutoLock lock(&m_Lock);

    RequestInfo ri;
	ri.m_bAssync = true;
    ri.m_sCmdLine = sRequest;
    ri.m_nExpectedRetCode = nExpectedRetCode;

    m_aRequests.push_back(ri);

	return (int)(m_aRequests.size()-1);
}

bool CMtClient::DelRequest(int nIndex)
{
	if(nIndex<0 || nIndex>=(int)m_aRequests.size())
		return false; // Invalid index

	CAutoLock lock(&m_Lock);

	// Delete request from the list
	m_aRequests.erase(m_aRequests.begin()+nIndex);

	return true;
}

bool CMtClient::Run(int nThreadCount, int nDuration, std::string& sErrorMsg)
{
    // Save parameters
    m_nThreadCount = nThreadCount;
    m_nDuration = nDuration;

	m_uTotalRequestsProcessed = 0;
    m_uRequestsSucceeded = 0;
    m_uRequestsRejected = 0;
    m_uRequestsFailed = 0;
    m_uSocketConnectionErrors = 0;

    sErrorMsg = "Unspecified error.";

    // Init logging
    if(!InitErrorLog(sErrorMsg))
    {
		sErrorMsg = "Error initializing error log.";
        return false;
    }


    double dStartTime = microtime();

    m_Log.write(0, "Main Thread: Creating worker threads.\n");

    // Create working threads
    int i;
    for(i=0; i<m_nThreadCount; i++)
    {
        CMtClientThread* pThread = new CMtClientThread(this, i, &m_Log);
        if(pThread==NULL)
        {
            sErrorMsg = "Couldn't create threads - out of memory.";
            m_Log.write(0, "Main Thread: Error creating worker threads - out of memory.\n");
            return false;
        }
        m_aThreads.push_back(pThread);
        if(!pThread->Run(NULL))
        {
            sErrorMsg = "Couldn't create threads - max thread count exceeded.";
            m_Log.write(0, "Main Thread: Error creating worker threads - max thread count reached.\n");
            return false;
        }
    }

    m_Log.write(0, "Main Thread: waiting until all threads exit.\n");

    // Wait until all threads exit
    for(i=0; i<m_nThreadCount; i++)
    {
        if(m_aThreads[i]!=NULL)
        {
            m_aThreads[i]->Wait();
            m_Log.write(0, "Main Thread: thread %d has exited, waiting for others.\n", i);
            delete m_aThreads[i];
        }
    }

	m_aThreads.clear();

	m_Log.write(0, "Main Thread: all worker threads have exited.\n");

    double dFinishTime = microtime();
    m_dTotalTestTime = (dFinishTime-dStartTime)/1000;

    // Print summary
    m_Log.write(0, "=== Summary ===\n");
    m_Log.write(0, "Total requests processed: %u\n", m_uTotalRequestsProcessed);
    m_Log.write(0, "      Requests succeeded: %u\n", m_uRequestsSucceeded);
    m_Log.write(0, "         Requests failed: %u\n", m_uRequestsFailed);
    m_Log.write(0, " Requests rejected (DOS): %u\n", m_uRequestsRejected);
    m_Log.write(0, "           Socket errors: %u\n", m_uSocketConnectionErrors);
    m_Log.write(0, "        Total tests time: %0.2f sec.\n", m_dTotalTestTime);

	// Close log file
	m_Log.term();

	if(m_uTotalRequestsProcessed!=m_uRequestsSucceeded)
	{
        sErrorMsg = "Some requests have failed or were rejected or there were socket connection errors.";
	}

	return m_uTotalRequestsProcessed==m_uRequestsSucceeded?true:false;
}

// Sends a request to daemon and gets response
bool CMtClient::ExecuteRequest(const char* szCmdLine, int& nServerReturnCode, std::string& sServerResponse, std::string& sErrorMsg)
{
    bool bStatus = false;           // Status
    int n = -1;
    std::string sResponse;
    SOCK sock = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server = NULL;

	nServerReturnCode = -1;
	sServerResponse = "";
	sErrorMsg = "Unspecified error.";

    // Get host address
    server = gethostbyname(m_sHost.c_str());
    if(server == NULL)
    {
		sErrorMsg = "Error getting host name";
		goto exit;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(m_nPort);

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

    // Read greeting message from server.
	n = RecvMsgWithTimeout(sock, 10, 1024, sResponse, sErrorMsg);
    if (n != 0 || sResponse.compare("100 Accepting requests.\n")!=0)
    {
		sErrorMsg = "Unexpected greeting message " + sResponse;
        goto exit;
    }

    // Send request to server.
    n = SendMsgWithTimeout(sock, szCmdLine, 10, sErrorMsg);
    if (n != 0)
    {
		sErrorMsg = "Error writing to socket";
        goto exit;
    }

    /* Read server response. */
    n = RecvMsgWithTimeout(sock, 10, 1024, sResponse, sErrorMsg);
    if (n != 0)
    {
		sErrorMsg = "Error reading server response";
        goto exit;
    }

    nServerReturnCode = atoi(sResponse.c_str());
	sServerResponse = sResponse;

	sErrorMsg = "Success.";
	bStatus = true;

exit:

	// Close socket
	if(sock!=0)
		CLOSESOCK(sock);

	// Return status
    return bStatus;
}

int CMtClient::SendMsgWithTimeout(SOCK sock, const char* msg, double timeout, std::string& sErrorMsg)
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

    // Message sent ok
    sErrorMsg = "Success.";
    return 0;
}

int CMtClient::RecvMsgWithTimeout(SOCK sock, double timeout, unsigned uMaxSize, std::string& sMsg, std::string& sError)
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


//---------------------------------------------------------------
// CClientThread impl
//---------------------------------------------------------------

CMtClientThread::CMtClientThread(CMtClient* pClient, int nThreadId, CLog* pLog)
{
    m_nThreadId = nThreadId;
    m_pClient = pClient;
    m_nSleepTime = 500;
	m_pLog = pLog;
}

long CMtClientThread::ThreadProc(void* pParam)
{
    double dStartTime = microtime();

    while(1)
    {
        // Sleep for a while (choose interval randomly)
		DWORD dwSleepTime = rand()%m_pClient->m_nDuration;
        Sleep(dwSleepTime);

        m_pLog->write(0, "Thread %d: Waked up.\n", m_nThreadId);

        // Get current time in msec
        double dCurTime = microtime();

        // Check if time has exceeded and we should exit
        if(dCurTime-dStartTime>=m_pClient->m_nDuration)
        {
            m_pLog->write(0, "Thread %d: test time exceeded, breaking.\n", m_nThreadId);
            break;
        }

        // Select a request from the list and execute it
        int nIndex = (int)(rand()%m_pClient->m_aRequests.size());

        std::string sCmdLine = m_pClient->m_aRequests[nIndex].m_sCmdLine;
        int nExpectedRetCode =  m_pClient->m_aRequests[nIndex].m_nExpectedRetCode;
		bool bAssync =  m_pClient->m_aRequests[nIndex].m_bAssync;

        std::set<int> aExpectedRetCodes;
        aExpectedRetCodes.insert(nExpectedRetCode);

        int nResult = ExecuteTestRequest(bAssync, sCmdLine.c_str(), aExpectedRetCodes);
    }

    return 0;
}

// Sends a request to daemon and gets response
int CMtClientThread::ExecuteTestRequest(bool bAssync, const char* szCmdLine, std::set<int> aExpectedRetCodes)
{
    int nStatus = -1;           // Status
    int nServerReturnCode = -1; // What server returned
    int n = -1;
    std::string sResponse;
    std::string sErrorMsg;
    std::ostringstream str;
	std::string sCmdLine;
	bool bExecute = false;

	// Modify command line if required (replace placeholders)
	str << m_nThreadId;
	sCmdLine = szCmdLine;
	replace(sCmdLine, std::string("{thread_number}"), str.str());

	m_pLog->write(0, "Thread %d: Executing request: %s.\n", m_nThreadId, sCmdLine.c_str());

    // Send request to server.
	bExecute = m_pClient->ExecuteRequest(sCmdLine.c_str(), nServerReturnCode, sResponse, sErrorMsg);
    if (bExecute != true)
    {
		m_pLog->write(0, "Thread %d: Error executing request '%s': server returned.\n", m_nThreadId, sCmdLine.c_str(), sResponse.c_str());

        m_pClient->m_Lock.Lock();
        m_pClient->m_uRequestsRejected++;
        m_pClient->m_Lock.Unlock();

        goto exit;
    }

	if(bAssync)
	{
		// Get command id
		int pos1 = sResponse.find('{');
		int pos2 = sResponse.find('}', pos1+1);
		std::string sCmdId = sResponse.substr(pos1+1, pos2-pos1-1);

		// Wait until command is finished
		for(;;)
		{
			std::string sRetMsg;

			std::ostringstream stCommand;
			stCommand << "daemon get-assync-info -erase-completed  " << sCmdId << "\n";
			std::string sCommand = stCommand.str();

			bool bExec = m_pClient->ExecuteRequest(sCommand.c_str(), nServerReturnCode, sResponse, sErrorMsg);
			if(!bExec)
			{
				std::string err_msg;
				err_msg += "Error executing request ";
				err_msg += sCmdLine;
				err_msg += " server responded ";
				err_msg += sResponse;

				m_pClient->m_Lock.Lock();
				m_pClient->m_uRequestsFailed++;
				m_pClient->m_Lock.Unlock();

				goto exit;
			}

			if(sResponse.find("still executing")!=sResponse.npos)
			{
				Sleep(1000);
				continue;
			}

			// Get command id and return message
			pos1 = sResponse.find('{');
			pos2 = sResponse.find('}', pos1+1);
			sCmdId = sResponse.substr(pos1+1, pos2-pos1-1);

			int pos3 = sResponse.find('{', pos2+1);
			int pos4 = sResponse.find('}', pos3+1);
			sRetMsg = sResponse.substr(pos3+1, pos4-pos3-1);
			nServerReturnCode = atoi(sRetMsg.c_str());

			break;
		}
	}

    if(aExpectedRetCodes.find(nServerReturnCode)== aExpectedRetCodes.end())
    {
        std::ostringstream stream;
        stream << "Unexpected server return code ";
        stream << nServerReturnCode;
        stream << " while expected ";
        std::set<int>::iterator it;
        for(it= aExpectedRetCodes.begin(); it!=aExpectedRetCodes.end(); it++)
            stream << *it << " ";

        m_pClient->m_Log.write(0, (char*)stream.str().c_str());

        stream << "\n";
        std::string err_msg = stream.str().c_str();
        err_msg += " on request ";
        err_msg += szCmdLine;
        err_msg += " server responded ";
        err_msg += sResponse;

        m_pClient->m_Lock.Lock();
        m_pClient->m_uRequestsFailed++;
        m_pClient->m_Lock.Unlock();

        nStatus = 1;
    }
    else
    {
        m_pClient->m_Lock.Lock();
        m_pClient->m_uRequestsSucceeded++;
        m_pClient->m_Lock.Unlock();
    }

    m_pLog->write(0, "Thread %d: Request completed ok.\n", m_nThreadId);
    nStatus = 0;

exit:

	// Update statistics
    m_pClient->m_Lock.Lock();
    m_pClient->m_uTotalRequestsProcessed ++;
    m_pClient->m_Lock.Unlock();

	// Return status
    return nStatus;
}

