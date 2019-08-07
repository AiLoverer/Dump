//! \file ServerThread.cpp
//! \brief Implements a server thread.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "ServerThread.h"
#include "SocketServer.h"
#include "Log.h"
#include "Misc.h"
#include "Daemon.h"
#include "CommandProcessor.h"
#include "strconv.h"
#ifdef _WIN32
//#include "CrashRpt.h"
#endif

#define WR_MAX_ARG 1024   //!< Maximum size of arguments

CServerThread::CServerThread(CSocketServer* pServer, int nThreadId)
{
    m_pServer = pServer;
    m_nThreadId = nThreadId;
	m_pLog = pServer->m_pLog;
	m_bBusy = false;
	m_StartTime = 0;
	m_Sock = INVALID_SOCKET;
}

long CServerThread::ThreadProc(void* pParam)
{
    // Waiting for requests in infinite loop.
    while(1)
    {
        // Check if server waits for all threads termination.
        if(!m_pServer->IsRunning())
        {
            m_pLog->write(1, "Terminating thread %d\n", m_nThreadId);
        }

        // Wait for incoming request.
        m_pServer->m_Cond.Wait();

        m_pLog->write(2, "Thread %d: Waked up.\n", m_nThreadId);

        int nRequestsProcessed = 0;

        // In loop, get all pending requests from queue and process each one
        while(1)
        {
            // Check if there are pending requests.
            SOCK sock = 0;
            double IncomingTime = 0;
			std::string sCmdId;
			std::string sCommand;
            if(m_pServer->GetRequest(sock, IncomingTime, sCmdId, sCommand))
            {
                {
					CAutoLock lock(&m_CritSec);

                    // Set busy flag
                    m_bBusy = true;

                    // Set start time
                    time(&m_StartTime);

                    // Save socket
                    m_Sock = sock;

					// Save command.
					m_sCommand = sCommand;
					m_sCmdId = sCmdId;
                }

                m_pLog->write(2, "Thread %d: Processing request on sock = %d.\n", m_nThreadId, m_Sock);

                // Handle client request.
                HandleSocketRequest();

                // Increment count of processed requests.
                nRequestsProcessed++;
            }
            else
            {
                // There are no pending requests for us
                m_pLog->write(2, "Thread %d: There are no pending requests in the queue, go to sleep.\n", m_nThreadId);
                break;
            }
        }

        //
        if(nRequestsProcessed>0)
        {
            // Update our status.
            CAutoLock lock(&m_CritSec);
            m_bBusy = false;
            m_Sock = -1;
        }
    }

    return 0;
}

time_t CServerThread::GetStartTime()
{
    CAutoLock lock(&m_CritSec);
    return m_StartTime;
}

bool CServerThread::IsBusy()
{
    CAutoLock lock(&m_CritSec);
    return m_bBusy;
}

SOCK CServerThread::GetSocket()
{
    CAutoLock lock(&m_CritSec);
	return m_Sock;
}

void CServerThread::SetSocket(SOCK sock)
{
    CAutoLock lock(&m_CritSec);
    m_Sock = sock;
}


// This function is called in context of thread_id thread.
// It reads client request data from sock socket and processes the request.
void CServerThread::HandleSocketRequest()
{
    // Set up the input buffer.
    std::string sInputBuffer;
    std::vector<std::string> asCommandSeq;
    int nParseResult = -1;
    int nProcessResult = -1;
    std::string sRecvError;
    std::string sResponse;
	const int TIMEOUT = 5;
    const int MAX_IN_BUFF_SIZE = 10240;
    std::string sErrorMsg;

	// Check if socket number is valid
	if(m_Sock!=0)
	{
		// If socket number is valid, we should read the data from socket,
		// parse them and execute commands

		// Send invitation message with 5-second timeout.
		m_pLog->write(2, "Thread %d: Sending invitation message.\n", m_nThreadId);
		if(m_pServer->SendMsgWithTimeout(m_Sock, "100 Accepting requests.\n", TIMEOUT, sErrorMsg)<0)
		{
			m_pLog->write(0, "Thread %d: Error sending invitation message.\n", m_nThreadId);
			goto exit;
		}

		// Read request data with 10-second timeout
		m_pLog->write(2, "Thread %d: Reading request data.\n", m_nThreadId);
		if(m_pServer->RecvMsgWithTimeout(m_Sock, TIMEOUT, MAX_IN_BUFF_SIZE, sInputBuffer, sRecvError)<0)
		{
			m_pLog->write(0, "Thread %d: Error reading request data: %s.\n", m_nThreadId, sRecvError.c_str());
			goto exit;
		}

		// Extract command sequence from the input buffer and produce an array of commands.
		nParseResult = ParseInputBuffer(sInputBuffer, asCommandSeq);
		if(nParseResult<0)
		{
			m_pLog->write(0, "Thread %d: Invalid input data.\n", m_nThreadId);
			goto exit;
		}

		// Process the sequence of commands.
		m_pLog->write(2, "Thread %d: Processing command sequence.\n", m_nThreadId);
		nProcessResult = ProcessCommandSequence(asCommandSeq, sResponse);

		// Send response to client.
		m_pLog->write(2, "Thread %d: Sending response %s.\n", m_nThreadId, sResponse.c_str());
		if(m_pServer->SendMsgWithTimeout(m_Sock, sResponse.c_str(), TIMEOUT, sErrorMsg)<0)
		{
			m_pLog->write(0, "Thread %d: Error sending response.\n", m_nThreadId);
		}
	}
	else
	{
		// If socket number is invalid, we should process the assynchronous command
		m_pLog->write(2, "Thread %d: Processing assynchronous command '%s'.\n", m_nThreadId, m_sCommand.c_str());
		nProcessResult = ProcessCommand(m_sCommand.c_str(), sResponse);
		m_pLog->write(2, "Thread %d: Assync command completed with code %d '%s'\n", m_nThreadId, nProcessResult, sResponse.c_str());
		// Update assync command status
		m_pServer->UpdateAssyncCommandInfoOnCompletion(m_sCmdId.c_str(), nProcessResult, sResponse);
	}

exit:

    // Clean up
	if(m_Sock)
	{
		// Close socket
		CLOSESOCK(m_Sock);
		m_Sock = 0;
	}

	// Clean command
	m_sCommand.clear();

    // End.
    return;
}

int CServerThread::ParseInputBuffer(std::string& sInput, std::vector<std::string>& asCmdSeq)
{
    m_pLog->write(2, "Thread %d: Parsing input buffer.\n", m_nThreadId);

    // Init output
    asCmdSeq.clear();

    char* pPtr = NULL;
    const char* szInput = sInput.c_str();
    size_t nBufSize = sInput.size();

    // Now split the command sequence using ';' character as a separator.

    // Get first command
    char* pCmd = strtok_r((char*)szInput, ";\n", &pPtr);
    while( pCmd != NULL )
    {
        if(strlen(pCmd)==0)
        {
            m_pLog->write(2, "Thread %d: encountered empty command.\n", m_nThreadId);
            break;
        }

        // Add command to the list
        asCmdSeq.push_back(pCmd);

        m_pLog->write(2, "Thread %d: encountered command: %s\n", m_nThreadId, pCmd);

        // Get the next command
        pCmd = strtok_r(NULL, ";\n", &pPtr);
    }

    // Done
    m_pLog->write(2, "Thread %d: command sequence parsed ok.\n", m_nThreadId);
    return 0;
}

int CServerThread::ProcessCommandSequence(std::vector<std::string>& cmd_seq, std::string& err_msg)
{
    char err_code[64] = "";
    char sz_cmd_num[10] = "";
    int result = -1;
    std::string cmd_err_msg;

    unsigned i;
    for(i=0; i<cmd_seq.size(); i++)
    {
        result = ProcessCommand(cmd_seq[i].c_str(), cmd_err_msg);
        if(result<0)
        {
            break;
        }
    }

    sprintf(err_code, "%d ", result);
    sprintf(sz_cmd_num, "%u ", i);

    err_msg = std::string(err_code);
    err_msg += cmd_err_msg;
    err_msg += "\n"; // end of message marker

    return result;
}

int CServerThread::ProcessCommand(const char* szCmdLine, std::string& sErrorMsg)
{
    char *pArg = NULL, *pPtr = NULL;
    char *argv[WR_MAX_ARG + 1] = {NULL};
    int	 argc = 0;
	std::string sCmdLine = szCmdLine;

    argc = 0;
    if( szCmdLine != NULL && *szCmdLine != '\0' )
    {
		size_t nLen = strlen(szCmdLine);
		pArg = pPtr = (char*)szCmdLine;
		bool bDoubleQuotes = false;
		size_t i;
		for(i=0; i<nLen; i++)
		{
			if(*pPtr=='\"')
			{
				if(!bDoubleQuotes)
				{
					// Begin of an argument enclosed into double-quotes
					pPtr++;
					pArg = pPtr; // save starting pointer
					bDoubleQuotes = true;
				}
				else
				{
					// End of an argument enclosed into double-quotes
					*pPtr = '\0'; // zero-terminate argument
					bDoubleQuotes = false;
					argv[argc] = pArg;
					argc++;
					pPtr++;
					pArg=pPtr;
				}
			}
			else if(*pPtr==' ' && !bDoubleQuotes)
			{
				*pPtr = '\0'; // zero-terminate argument

				if(strlen(pArg)!=0)
				{
					// Save argument
					argv[argc] = pArg;
					argc++;
				}

				pPtr++;
				pArg = pPtr; // Save starting pointer
			}
			else
			{
				// Inside of an argument body

				if(i==nLen-1 && strlen(pArg)!=0)
				{
					// Save argument
					argv[argc] = pArg;
					argc++;
				}

				pPtr++;
			}

            if( argc >= WR_MAX_ARG )
            {
                m_pLog->write(2, "process_command: Count of command arguments has exeeded the limit.\n");
                sErrorMsg = "Count of command arguments has exeeded the limit.";
                return -1;
            }
        }
    }

    argv[argc] = NULL;

    int i;
    for(i=0; i<argc; i++)
    {
        m_pLog->write(2, "%s\n", argv[i]);
    }

    int nErrorCode = 101;
    sErrorMsg = "Unknown command.\n";

	if(argc>0 && strcmp(argv[0], "assync")==0)
	{
		// Assynchronous command encountered

		if(sCmdLine.length()<=7)
		{
			sErrorMsg = "Command line is too short: ";
			sErrorMsg += szCmdLine;
		}
		else
		{
			// Remove "assync " prefix
			sCmdLine = sCmdLine.substr(7, std::string::npos);

			// Put request to the queue
			SOCK sock = 0;
			std::string sCmdId;
			bool bAddRequest = m_pServer->AddRequest(sock, sCmdLine.c_str(), &sCmdId);
			if(!bAddRequest)
			{
				sErrorMsg = "Couldn't put the assync request to the queue.";
			}
			else
			{
				nErrorCode = 0;
				std::ostringstream str;
				str << "Assync command {";
				str << sCmdId;
				str << "} has been added to the request queue.";
				sErrorMsg = str.str();
			}
		}
	}
	else if(argc>0 && strcmp(argv[0], "dumper")==0)
    {
		// A synchronous "dumper" command
        CCommandProcessor cp;
        cp.SubstituteLog(m_pLog, false);
		cp.SubstitutePdbCache(&m_pServer->m_PdbCache, false);
        nErrorCode = cp.Run(argc, argv);
        sErrorMsg = cp.GetErrorMsg();
    }
    else if(argc>0 && strcmp(argv[0], "daemon")==0)
    {
		// A synchronous "daemon" command
        nErrorCode = ProcessServerCommand(argc, argv, sErrorMsg);
    }
    else
    {
        nErrorCode = 101;

        if(argc==0)
            sErrorMsg = "No command specified.";
        else
            sErrorMsg = "Unexpected command encountered.";
    }

    return nErrorCode;
}

int CServerThread::ProcessServerCommand(int argc, char* argv[], std::string& sErrMsg)
{
	int cur_arg = 0;
    int nStatus = R_INVALID_PARAM;
    sErrMsg = ("Unspecified error.");

    if(!cmp_arg("daemon"))
    {
        sErrMsg = ("Unknown server command.");
        goto exit;
    }

	skip_arg();

    if(cmp_arg("status"))
    {
		skip_arg();

        if(args_left()!=0)
        {
            sErrMsg = "Invalid parameter count.";
            goto exit;
        }

        nStatus = m_pServer->GetServerStatus(sErrMsg);
        goto exit;
    }
	else if(cmp_arg("get-license-info"))
	{
		skip_arg();

		const char* szOutFile = get_arg();
		if(szOutFile==NULL)
		{
			sErrMsg = "Invalid argument.";
            goto exit;
		}

		std::wstring sOutFile = strconv::a2w(szOutFile);

		nStatus = m_pServer->GetServerLicenseInfo(sOutFile.c_str(), sErrMsg);
		goto exit;
	}
	else if(cmp_arg("get-config-info"))
	{
		skip_arg();

		const char* szOutFile = get_arg();
		if(szOutFile==NULL)
		{
			sErrMsg = "Invalid argument.";
            goto exit;
		}

		std::wstring sOutFile = strconv::a2w(szOutFile);

		nStatus = m_pServer->GetServerConfigInfo(sOutFile.c_str(), sErrMsg);
		goto exit;
	}
	else if(cmp_arg("get-assync-info"))
    {
		bool bEraseCompleted = false;
		std::string sCommandId;

		skip_arg();

		while(args_left())
		{
			if(cmp_arg("-erase-completed"))
			{
				bEraseCompleted = true;
				skip_arg();
			}
			else if(sCommandId.empty())
			{
				const char* szCmdId = get_arg();
				sCommandId = szCmdId;
				skip_arg();
			}
			else
			{
				sErrMsg = "Invalid parameter: ";
				sErrMsg += get_arg();
				goto exit;
			}
		}

		nStatus = m_pServer->GetAssyncCommandInfo(sErrMsg, sCommandId.c_str(), bEraseCompleted);
        goto exit;
    }
    else
    {
        nStatus = R_INVALID_PARAM;
        sErrMsg = "Invalid command name.";
        goto exit;
    }

exit:
    return nStatus;
}

//--------------------------------------------------
// CSitePollingThread impl
//--------------------------------------------------

CSitePollingThread::CSitePollingThread(CSocketServer* pServer, std::string sPollCommand)
{
    m_pServer = pServer;
	m_sPollCommand = sPollCommand;
}

long CSitePollingThread::ThreadProc(void* pParam)
{
	// In loop, wake up regularily and poll our PHP script.
    // The script will issue client requests back to us and collect results.
    for(;;)
    {
        // Sleep for some time
        Sleep(30*1000);

		// Record starting time
        double start_time = microtime();

        int nRet = 1; // ret code

#ifdef _WIN32
        // Execute PHP script
        nRet = execute(m_sPollCommand.c_str());
#else
        nRet = system(m_sPollCommand.c_str());
        if(nRet!=-1)
        {
            nRet = WEXITSTATUS(nRet);
        }
        else
        {
            m_pServer->m_pLog->log_last_error(0, "Function system failed");
        }

#endif

		// Record finish time
        double finish_time = microtime();
		// Calculate duration
        double duration = (finish_time-start_time)/1000;

		// Check script's ret val
        if(nRet!=0)
		{
			m_pServer->m_pLog->write(0,
                "Polling thread has encountered an error when executing PHP script '%s'; execution time = %0.2f sec; error code = %d.\n",
                m_sPollCommand.c_str(), duration, nRet);

			if(nRet==255)
			{
				m_pServer->m_pDaemon->AddError(true,
					"CrashFix web application is installed, but not configured correctly. "
					"PHP CLI has exited with return code 255 indicating that the PHP CLI has crashed while executing the script. Please check your php.ini "
				    "config file and ensure that all required PHP extensions are enabled.");
			}
			else
			{
				m_pServer->m_pDaemon->AddError(true,
					"CrashFix web application is not installed or not configured correctly. "
					"Please check %CRASHRPT_DIR%/protected/runtime/console.log file for diagnostics information."
					);
			}
		}
        else
        {
            m_pServer->m_pLog->write(1,
                "Polling thread has executed PHP script successfully (execution time = %0.2f sec).\n",
                duration);
        }

        // Additionally perform log rotation for daemon
        m_pServer->m_pLog->check_error_log_size();
    }
}
