//! \file UploaderConsole.h
//! \brief Incapsulates command line parsing and command execution.
//! \author Oleg Krivtsov
//! \date Feb 2012

#pragma once
#include "stdafx.h"
#include "Uploader.h"
#include "Misc.h"

//! \class CUploaderConsole
//! \brief Incapsulates uploader command line parsing and command execution.
class CUploaderConsole
{
public:

	//! Constructor.
	CUploaderConsole();

	//! Prints version and copyright to terminal
    void PrintVersion();

	//! Prints usage to terminal
    void PrintUsage();

	//! Runs a console command.
    int Run(int argc, char* argv[]);

	//! Returns error message.
	std::string GetErrorMsg();

private:

	//! Callback function called by worker thread.
	static BOOL CALLBACK UploadCallback(CallbackInfo* pInfo, LPVOID lpParam);

	//! Callback function called by worker thread.
	void OnUploadCallback(CallbackInfo* pInfo);

	/* Variables used internally. */

	std::string m_sErrorMsg;      //!< Error message
	CUploader m_Uploader;         //!< Aggregated uploader object.
	int m_nLastStatusChangedFile; //!< The last processed file
	bool m_bVerbose;              //!< Verbose output flag.
};