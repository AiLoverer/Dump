//! \file Config.h
//! \brief Config file reading/writing functions.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"

//! \class CConfig
//! \brief Config file reading and writing functionality.
class CConfig
{
public:

	//! Default constructor.
	CConfig();

	//! Constructor.
	//! @param[in] szFileName Config file name.
	//! @param[out] pbInit Receives the result of initialization.
	CConfig(const char* szFileName, bool* pbInit);

	//! Destructor.
	virtual ~CConfig();

	//! Opens config file.
	//! @param[in] szFileName Config file name.
	//! @param[out] pbInit Receives the result of initialization.
	bool Init(const char* szFileName);

	//! Returns file name; or NULL if no file open.
	const char* GetFileName();

	//! Reads a string from config file
	//! @param[in] param_name Name of the string
	//! @param[out] buf Buffer that receives string's value
	//! @param[in] buf_len Length of the buffer
	char* getProfileString(const char* param_name, char* buf, int buf_len);

	//! Reads an integer value from config file
	//! @param[in] param_name Name of the parameter
	//! @param[in] default_val Default value (will be assigned if parameter is missing from config)
	int getProfileInt(const char* param_name, int default_val);

	//! Writes a string to config file
	//! @param[in] param_name Name of the string
	//! @param[in] value String's value
	int writeProfileString(const char* param_name, const char* value);

private:

	//! Removes trailing spaces from string.
	void trim(char* s);

	std::string m_sFileName; //!< Config file name
};

