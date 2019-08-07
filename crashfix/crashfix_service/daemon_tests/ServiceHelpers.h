//! \file ServiceHelpers.h
//! \brief Contains daemon starting\stopping\restarting helper functions.
//! \author Oleg Krivtsov
//! \date Mar 2012

#pragma once
#include "ServiceHelpers.h"
#include "Misc.h"

//! Runs a system command and returns command output as a string.
//! @param[in] szCmd Command line.
std::string SystemCmd(const char* szCmd);

//! Starts the daemon.
//! @param[out] On output, receives an error message.
bool StartDaemon(std::string& sErrorMsg);

//! Stops the daemon.
//! @param[out] On output, receives an error message.
bool StopDaemon(std::string& sErrorMsg);

//! Sends the daemon signal SIGTERM (this should perform soft daemon shutdown).
//! @param[out] On output, receives an error message.
bool KillDaemon(std::string& sErrorMsg);

//! Restarts the daemon.
//! @param[out] On output, receives an error message.
bool RestartDaemon(std::string& sErrorMsg);

