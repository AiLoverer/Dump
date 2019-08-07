//! \file DaemonConsole.h
//! \brief Implements command line parsing and daemon console command execution.
//! \author Oleg Krivtsov
//! \date Feb 2012

#include "stdafx.h"
#include "Daemon.h"
#include "Misc.h"
#ifdef _WIN32
//#include "CrashRpt.h"
#endif

//! \class CDaemonConsole
//! \brief Manages daemon commands.
class CDaemonConsole
{
public:

	//! Constructor.
	CDaemonConsole();

	//! Destructor.
	virtual ~CDaemonConsole();

	//! Prints version to terminal.
	void PrintVersion();

	//! Prints usage to terminal
    void PrintUsage();

	//! Runs a command
    int Run(int argc, char* argv[]);

	//! Returns error message.
	std::string GetErrorMsg();

private:

	/* Privately used variables. */

	std::string m_sErrorMsg; //!< Last error message.
	static CDaemon* m_pDaemon; //!< Pointer to daemon singleton.
};

