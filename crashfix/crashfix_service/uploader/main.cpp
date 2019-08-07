//! \file main.cpp
//! \brief Defines the entry point for uploader console application.
//! \author Oleg Krivtsov
//! \date Feb 2012

#include "stdafx.h"
#include "UploaderConsole.h"

int main(int argc, char* argv[])
{
	CUploaderConsole UploaderConsole;
	int nResult = UploaderConsole.Run(argc, argv);	
	// If there was an error, print error message	
	if(nResult!=0)
		printf(UploaderConsole.GetErrorMsg().c_str());

	return nResult;
}

