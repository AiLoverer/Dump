//! \file ServerThread.h
//! \brief Implements a server thread.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "Thread.h"
#include "CritSec.h"

#ifdef _WIN32
typedef SOCKET SOCK;
#define CLOSESOCK(x) closesocket(x)
#define IOCTLSOCKET ioctlsocket
#else
typedef int SOCK;
#define CLOSESOCK(x) close(x)
#define INVALID_SOCKET -1
#define IOCTLSOCKET ioctl
#endif

#ifndef MIN
#define MIN(x,y) ( ((x)<(y))?(x):(y) )
#endif //MIN

// Forward declarations
class CSocketServer;
class CLog;

//! \class CServerThread
//! \brief A server thread - processes client requests.
class CServerThread : public CThread
{
public:

    //! Constructor.
    CServerThread(CSocketServer* pServer, int nThreadId);

    //! Returns the last time this thread waked up.
    time_t GetStartTime();

    //! Returns true if this thread is currently processing a request.
    bool IsBusy();

    //! Returns socket number this thread uses for reading/writing data.
	SOCK GetSocket();

	//! Sets socket for reading/writing data.
    void SetSocket(SOCK sock);

    //! Thread procedure.
    long ThreadProc(void* pParam);

private:

    //! Handles client request accepted on server socket. Reads the data
    //! from the socket, parses the command sequence and executes all commands in turn (for sync commands)
	//! or puts new requests into the queue (for assync commands).
    void HandleSocketRequest();

    //! This function parses incoming request string buffer. The buffer
    //! may contain a sequence of commands separated with '\n' characters.
    //! \return On success, returns 0; otherwise a non-zero value.
    //! @param[in] sInput Input buffer string.
    //! @param[out] asCmdSeq On output, this receives the list of commands extracted from buffer.
    int ParseInputBuffer(std::string& sInput, std::vector<std::string>& asCmdSeq);

    //! This function processes the sequence of commands
	//! \return Returns zero if all commands have executed successfully; otherwise the error status of the first failed command.
	//! @param[in] CmdSequence The list of commands to execute.
	//! @param[out] sErrMsg Cumulative error message.
    int ProcessCommandSequence(std::vector<std::string>& CmdSequence, std::string& sErrMsg);

    //! This function processes the command and writes the resulting error message to err_msg.
	//! \return Returns the status code of the command.
	//! @param[in] szCmdLine Command line.
	//! @param[out] sErrorMsg On output, receives the error message of the command.
    int ProcessCommand(const char* szCmdLine, std::string& sErrMsg);

    //! Processes a server command
    int ProcessServerCommand(int argc, char* argv[], std::string& sErrMsg);

    CSocketServer* m_pServer;  //!< Owner server.
    CCritSec m_CritSec;        //!< Critical section.
    int m_nThreadId;           //!< Thread number.
    CLog* m_pLog;              //!< Log.
    bool m_bBusy;              //!< Busy flag.
    time_t m_StartTime;        //!< Start time of the last request processing.
    SOCK m_Sock;               //!< Socket.
	std::string m_sCmdId;              //!< Assync command ID.
	std::string m_sCommand;    //!< Assync command.
};

//! \class CSitePollingThread
//! \brief Used to periodically poll the webapp.
class CSitePollingThread : public CThread
{
public:

    //! Constructor.
    CSitePollingThread(CSocketServer* pServer, std::string sPollCommand);

    //! Thread procedure.
    long ThreadProc(void* pParam);

private:

    CSocketServer* m_pServer;   //!< Owner server.
	std::string m_sPollCommand; //!< Path to PHP script to poll.
};


