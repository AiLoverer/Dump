//! \file DaemonConsole.cpp
//! \brief Implements command line parsing and daemon console command execution.
//! \author Oleg Krivtsov
//! \date Feb 2012

#include "stdafx.h"
#include "DaemonConsole.h"
#include "Misc.h"
#include "LibDumperVersion.h"

#ifdef _WIN32
//#include "CrashRpt.h"
#endif

CDaemon* CDaemonConsole::m_pDaemon = NULL;

CDaemonConsole::CDaemonConsole()
{
}

CDaemonConsole::~CDaemonConsole()
{
}

void CDaemonConsole::PrintVersion()
{
    const char* szCompileMode = "(DEBUG)";
#ifdef NDEBUG
    szCompileMode = "";
#endif //NDEBUG
	printf("CrashFix Daemon %s %s [ %s %s ]\n",
		szCompileMode, LIBDUMPER_VER, __DATE__, __TIME__);
	printf("Copyright (c) Oleg Krivtsov 2012\n");
}

void CDaemonConsole::PrintUsage()
{
    printf("Usage: crashfixd <command> ...\n");
    printf("   where available commands are:\n");
    printf(" \n");
    printf("   --help                Prints usage help.\n");
    printf(" \n");
	printf("   -v                    Prints version and copyright information.\n");
	printf(" \n");
#ifdef _WIN32
    printf("   --install             Installs the service.\n");
    printf(" \n");
    printf("   --remove              Removes the service.\n");
    printf(" \n");
#endif
	printf("   --start               Starts the service.\n");
    printf(" \n");
#ifdef _WIN32
    printf("   --stop                Stops the service.\n");
    printf(" \n");
    printf("   --restart             Restarts the service.\n");
    printf(" \n");
#endif
    printf("   -c <config_file_name> \n");
    printf("   --config <config_file_name> Used with --start. Defines the name of \n");
    printf("                           configuration file.\n");
	printf(" \n");
}

int CDaemonConsole::Run(int argc, char* argv[])
{
    /* Read command line parameters */

    int cur_arg=1;
    std::string sAction;
    std::string sConfigFile;
    std::string sPrefix;
    eMonitorOption MonitorOption = MO_UNDEFINED;
	bool bRestartedOnCrash = false;
    int nPid = 0;
    CDaemon daemon;
	m_pDaemon = &daemon;
	
    //MessageBoxA(NULL, "", "", 0);

    while(args_left()>0)
    {
        if(cmp_arg("--help"))
        {
            if(args_left()!=1)
            {
                m_sErrorMsg = "Too many parameters for --help command.";
                goto exit;
            }

            sAction += "help";
            break;
        }
		else if(cmp_arg("--test-crash"))
		{
			int* p = NULL;
			*p = 13;
		}
		else if(cmp_arg("-v"))
        {
            if(args_left()!=1)
            {
                m_sErrorMsg = "Too many parameters for -v command.";
                goto exit;
            }

            sAction += "version";
            break;
        }
        else if(cmp_arg("--start"))
        {
            sAction += "start";
        }
        else if(cmp_arg("--stop"))
        {
            sAction += "stop";
        }
#ifdef _WIN32
		else if(cmp_arg("--install"))
        {
            sAction += "install";
        }
        else if(cmp_arg("--remove"))
        {
            sAction += "remove";
        }
        else if(cmp_arg("--restart"))
        {
            sAction += "restart";
        }
		else if(cmp_arg("--run"))
        {
            sAction += "run";
        }
#endif
        else if(cmp_arg("-c") || cmp_arg("--config"))
        {
            skip_arg();
            char* szParam = get_arg();
            if(szParam==NULL)
            {
                m_sErrorMsg = "Exptected configuration file name.";
                goto exit;
            }

            sConfigFile = szParam;
        }
        else if(cmp_arg("--run-as-monitor"))
        {
            skip_arg();
            char* szParam = get_arg();
            if(szParam==NULL)
            {
                m_sErrorMsg = "Exptected PID.";
                goto exit;
            }

            nPid = atoi(szParam);

            MonitorOption = MO_IS_MONITOR;
        }
		else if(cmp_arg("--restart-on-crash"))
        {
            MonitorOption = MO_NO_MONITOR;
			bRestartedOnCrash = TRUE;
        }
        else
        {
            char* szParam = get_arg();
            m_sErrorMsg += "Unexpected parameter: ";
            m_sErrorMsg += szParam;
            goto exit;
        }

        skip_arg();
    }

    if(sAction=="help")
    {
        PrintUsage();
        goto exit;
    }
	else if(sAction=="version")
    {
        PrintVersion();
        goto exit;
    }
#ifdef _WIN32
	else if(sAction=="install")
    {
        if(!sConfigFile.empty())
            daemon.SetConfigFile(sConfigFile);
		return daemon.Install();
    }
	else if(sAction=="remove")
    {
        if(!sConfigFile.empty())
            daemon.SetConfigFile(sConfigFile);
		return daemon.Remove();
    }
	else if(sAction=="run")
    {
        if(!sConfigFile.empty())
            daemon.SetConfigFile(sConfigFile);
        daemon.SetMonitorOption(MonitorOption, nPid);
		daemon.SetErrorRestartFlag(bRestartedOnCrash);

		m_pDaemon = &daemon;
        daemon.Run();
		return 0;
    }
#endif
    else if(sAction=="start")
    {
        if(!sConfigFile.empty())
            daemon.SetConfigFile(sConfigFile);
        daemon.SetMonitorOption(MonitorOption, nPid);
		daemon.SetErrorRestartFlag(bRestartedOnCrash);

		m_pDaemon = &daemon;
        return daemon.Start();
    }
	else if(sAction=="restart")
    {
        if(!sConfigFile.empty())
            daemon.SetConfigFile(sConfigFile);
        if(!sPrefix.empty())
            daemon.SetPrefix(sPrefix);

		m_pDaemon = &daemon;
        return daemon.Restart();
    }
	else if(sAction=="stop")
    {
        if(!sConfigFile.empty())
            daemon.SetConfigFile(sConfigFile);
        if(!sPrefix.empty())
            daemon.SetPrefix(sPrefix);

        return daemon.Stop();
    }
    else
    {
		m_sErrorMsg = "Invalid combination of commands.";

#ifdef _WIN32
		m_pDaemon = &daemon;
		daemon.EnterServiceMain();
#endif

        goto exit;
    }

exit:

	m_pDaemon = NULL;

    return m_sErrorMsg.empty()?0:1;

}

std::string CDaemonConsole::GetErrorMsg()
{
	return m_sErrorMsg;
}

