#include "stdafx.h"
#include "CommandProcessor.h"
#include "PdbCache.h"
#include "Misc.h"
#include "LibDumperVersion.h"

int main(int argc, char* argv[])
{
    //printf("dumper version " LIBDUMPER_VER " build date "__DATE__" "__TIME__"\n");

    CCommandProcessor cp;

    // Direct logging to terminal
    cp.InitLog(L"stdout", 0);

    CPdbCache PdbCache;    
    cp.SubstitutePdbCache(&PdbCache, false);
    
    // Process command
    int nRetCode = cp.Run(argc, argv);

    // Print error message
    printf("%s\n", cp.GetErrorMsg().c_str());
	
    // Print usage
    if(nRetCode<0)
        cp.PrintUsage();
    
    return nRetCode;
}



