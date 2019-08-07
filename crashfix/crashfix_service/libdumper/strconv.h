//! \file strconv.h
//! \brief // String conversion functions
//! \author Oleg Krivtsov
//! \date 2011

#ifndef _STRCONV_H
#define _STRCONV_H

#include "stdafx.h"
#include <vector>

namespace strconv
{
    //! Multi-byte to wide char conversion
    std::wstring a2w(std::string str);  

    //! Wide char to multibyte conversion
    std::string w2a(std::wstring str);  

	//! Wide char to UTF-8 conversion
    std::string w2utf8(std::wstring str);  

	//! UTF-8 to wide-char conversion
	std::wstring utf82w(std::string str);

	//! Multi-bye to UTF-8 conversion
    std::string a2utf8(std::string str);  

	//! UTF-8 to multi-byte conversion
	std::string utf82a(std::string str);

	//! Converts a number to string.
	std::string n2a(const char* szFormat, ...);

	//! Converts a number to string
	std::wstring n2w(const wchar_t* szFormat, ...);

	//! Formats a string
	std::string format(int nMaxChar, const char* szFormat, ...);

	//! Formats a string
	std::wstring formatW(int nMaxChar, const wchar_t* szFormat, ...);
};

#endif  //_STRCONV_H


