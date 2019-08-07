//! \file main.cpp
//! \brief Entry point to daemon console application.
//! \author Oleg Krivtsov
//! \date Feb 2012

#include "stdafx.h"
#include "DaemonConsole.h"
#include "strconv.h"

/*
*  Main function.
*/

int main(int argc, char* argv[])
{
	CDaemonConsole DaemonConsole;
	
	int nResult = DaemonConsole.Run(argc, argv);

	if(!DaemonConsole.GetErrorMsg().empty())
    {
        printf("%s\n", DaemonConsole.GetErrorMsg().c_str());
        printf("\nType --help for available commands.\n\n");
        return 1;
    }

	return nResult;
}

