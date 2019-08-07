//! \file Config.cpp
//! \brief Config file reading functions.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "Config.h"

#if (defined WIN32 || defined WIN64) && (_MSC_VER >= 1200)
// disable MSVC8 security warning
#pragma warning( disable: 4996 )
#endif

CConfig::CConfig()
{
}

CConfig::CConfig(const char* szFileName, bool* pbInit)
{
	if(pbInit!=NULL)
		*pbInit = false;

	bool bInit = Init(szFileName);

	if(pbInit)
		*pbInit = bInit;
}

CConfig::~CConfig()
{
}

bool CConfig::Init(const char* szFileName)
{
	m_sFileName.clear();

	if(szFileName==NULL)
		return false;

	// Open file
	FILE* f = fopen(szFileName, "rt");
	if(f==NULL)
		return false; // Error opening file

	// Close file
	fclose(f);

	// Save file name
	m_sFileName = szFileName;

	return true;
}

const char* CConfig::GetFileName()
{
	if(m_sFileName.empty())
		return NULL;

	return m_sFileName.c_str();
}


char* CConfig::getProfileString(const char* param_name, char* buf, int buf_len)
{
	char* szRet = NULL;
	char szLine[4096]="";
	char* s = NULL;
	char* c = NULL;
	char szName[4096]="";
	char szVal[4096]="";
	bool bFound = false;
	unsigned int i = 0;
	bool bComment = false;
	unsigned int len = 0;
	FILE* f = NULL;

	// Validate input parameters
	if(m_sFileName.empty() ||
		param_name==0 ||
		buf==NULL ||
		buf_len==0)
		goto cleanup; // Invalid arg

	// Open file
	f = fopen(m_sFileName.c_str(), "rt");
	if(f==NULL)
		goto cleanup; // Error opening file

	// Read file
	while(!feof(f))
	{
		// Read a line from file
		s = fgets(szLine, 4095, f);
		if(s==NULL)
			break; // nothing to read?

		// Check if it is comment (begins with '#' or ';' symbol)
		len = strlen(s);
		for(i=0;i<len;i++)
		{
			// Check if line starts with a comment character
			if(s[i]=='#' || s[i]==';')
			{
				// This line is a comment.
				bComment = true;
				break;
			}

			// Check for non-space characters
			if(s[i]!=' ' && s[i]!='\t')
				break; // Non-space char encountered
		}

		// If we encountered a comment line, skip it
		if(bComment)
		{
			// Reset comment flag
			bComment = false;

			continue; // Skip comment lines
		}

		// Look for '#' or ";" symbol inside the line
		// (inline comments)
		c = strchr(s, '#');
		if(c!=NULL)
		{
			// remove comment part
			*c=0;
		}

		c = strchr(s, ';');
		if(c!=NULL)
		{
			// remove comment part
			*c=0;
		}

		// Now we need to separate parameter name from its value.
		// Look for the first '=' symbol.
		c = strchr(s, '=');
		if(c==NULL)
			continue; // not valid line format?

		// get parameter name
		len = (int)(c-s);
		strncpy(szName, s, len);
		szName[len]=0;
		trim(szName);

		// Check if this parameter is what we need
		if(strcmp(szName, param_name)==0)
		{
			// get parameter value
			int val_len = (int)(strlen(s)-len-1);
			strncpy(szVal, c+1, val_len);
			szVal[val_len]=0;
			if(szVal[val_len-1]=='\n')
				szVal[val_len-1]=0;
			trim(szVal);

			// Copy value to output buffer
			strncpy(buf, szVal, buf_len);
			// Ensure the string is zero-terminated
			if(strlen(szVal)>=(unsigned)buf_len)
				buf[buf_len-1]=0;
			bFound = true;
			break;
		}
	}

	// If parameter found...
	if(bFound)
	{
		// Return pointer to buffer
		szRet = buf;
	}

cleanup:

	// Close file
	if(f)
		fclose(f);

	return szRet;
}


int CConfig::getProfileInt(const char* param_name, int default_val)
{
	const int BUF_SIZE = 32;
	char buf[BUF_SIZE];

	//const char* profile_name = m_sFileName.c_str();

	char* s = getProfileString(param_name, buf, BUF_SIZE);
	if(s==NULL) return default_val;

	return atoi(s);
}

int CConfig::writeProfileString(const char* param_name, const char* value)
{
	int nResult = -1;
	FILE* f = NULL;
	char* szTmpName = NULL;
	FILE* tmpf = NULL;
	char str[1024] = "";
	char name[512] = "";
	char val[512] = "";
	bool found = false;

	const char* profile_name = m_sFileName.c_str();

	// Validate input parameters
	if(profile_name==0 || param_name==0 || val==NULL)
		goto cleanup;

	// Open source file
	f = fopen(profile_name, "rt");
	if(f==NULL)
		goto cleanup;

	// Open temp file
	szTmpName = tmpnam(NULL);
	tmpf = fopen(szTmpName, "wt");
	if(tmpf==NULL)
		goto cleanup;

	while(!feof(f))
	{
		// Read a line from file
		char* s = fgets(str, 1024, f);
		if(s==NULL)
			break; // nothing to read?

		// check if it is a comment (begins with '#' or ';' symbol)
		unsigned i, comment=0;
		for(i=0;i<strlen(s);i++)
		{
			if(s[i]=='#' || s[i]==';')
			{
				comment = 1;
				break;
			}

			if(s[i]!=' ' && s[i]!='\t')
				break;
		}
		if(comment)
		{
			fprintf(tmpf, "%s", str); // Put line to temp file without changes
			continue;
		}

		// look for the first '=' symbol
		char* c = strchr(s, '=');
		if(c==NULL)
		{
			// not valid line format?
			fprintf(tmpf, "%s", str); // Put line to temp file without changes
			continue;
		}

		// get parameter name
		int name_len = (int)(c-s);
		strncpy(name, s, name_len);
		name[name_len]=0;
		trim(name);

		if(strcmp(name,param_name)!=0)
		{
			fprintf(tmpf, "%s", str); // Put line to temp file without changes
			continue;
		}

		found = true;
		// Put param name = val
		fprintf(tmpf, "%s = %s\n", param_name, value);
	}

	if(!found) // Append to the end of file
	{
		// Put param name = val
		fprintf(tmpf, "%s = %s\n", param_name, value);
	}

	fclose(f);
	f = NULL;
	fclose(tmpf);
	tmpf = NULL;

	// Copy temp file content to original file
	f = fopen(profile_name, "wt");
	if(f==NULL)
		goto cleanup;

	tmpf = fopen(szTmpName, "rt");
	if(tmpf==NULL)
		goto cleanup;

	while(!feof(tmpf))
	{
		// Read a line from file
		char* s = fgets(str, 1024, tmpf);
		if(s!=NULL)
			fputs(str, f);
	}

	// Success
	nResult = 0;

cleanup:

	// Close files
	if(f)
		fclose(f);

	if(tmpf)
		fclose(tmpf);

	// Remove temp file
	if(szTmpName)
		remove(szTmpName);

	return nResult;

}

void CConfig::trim(char* s)
{
	// Validate arg
	if(s==NULL)
		return;

	// Trim left spaces
	unsigned i = 0;
	unsigned len = strlen(s);
	for(i=0; i<len; i++)
	{
		if(s[i]!='\t'&&
		   s[i]!=' ' &&
		   s[i]!='\r' &&
		   s[i]!='\n')
			break;
	}

	if(i!=0)
	{
		// Copy a portion of line
		unsigned j;
		for(j=0; j<len-i; j++)
			s[j] = s[j+i];
		s[len-i]=0; // zero-terminate
	}

	// trim right spaces

	len = strlen(s);
	if(len!=0)
	{
        for(i=(int)len-1; i>=0; i--)
        {
            if(s[i]!='\t'&&
               s[i]!=' ' &&
               s[i]!='\r' &&
               s[i]!='\n')
               break;
        }

        if(i!=len-1)
            s[i+1] = 0;
	}
}

