#pragma once
#include "stdafx.h"

// The following macros are used for parsing the command line
#define args_left() (argc-cur_arg)
#define arg_exists() (cur_arg<argc && argv[cur_arg]!=NULL)
#define get_arg() ( arg_exists() ? argv[cur_arg]:NULL )
#define skip_arg() cur_arg++
#define cmp_arg(val) (arg_exists() && (0==strcmp(argv[cur_arg], val)))

class CLog;
class CPdbCache;

//! \class CCommandProcessor
//! \brief Processes console commands.
class CCommandProcessor
{
public:

	//! Constructor.
    CCommandProcessor();

	//! Destructor.
    virtual ~CCommandProcessor();
    
    //! Prints usage to log
    void PrintUsage();

    //! Runs a command
    int Run(int argc, char* argv[]);

    //! Reads a minidump file and writes output to file
    int ReadDump(LPCSTR szFileName, LPCSTR szOutFile);

    //! Reads a PDB file and writes results to file
    int ReadPdb(LPCSTR szFileName, LPCSTR szOutFile);

    //! Extracts a stream from PDB file
    int ExtractPdbStream(LPCSTR szPdbFileName, int nStream, LPCSTR szOutFile);

    //! Extracts all PDB streams
    int ExtractPdbStreams(LPCSTR szPdbFileName, LPCSTR szOutDir);

    //! Dumps PDB file content
    int Dia2Dump(LPCSTR szPdbFileName, LPCSTR szOutFile);

    //! Dumps crash report contents.
	//! @param[in] szCrashRptFileName Crash report ZIP archive.
	//! @param[in] szOutFile Output XML file that will receive the resulting information.
	//! @param[in] szSymbolSearchDir Directory name where to search symbols. Optional.
	//! @param[in] bExactMatchBuildAge Wether to require exact match of PDB build age or not require.
    int DumpCrashReport(LPCWSTR szCrashRptFileName, LPCWSTR szOutFile, LPCWSTR szSymbolSearchDir, bool bExactMatchBuildAge);

	//! Extracts a file from crash report ZIP archive.
	int ExtractFile(LPCWSTR szCrashRptFileName, LPCWSTR szFileItemName, LPCWSTR szOutFile);

    //! Imports PDB file into symbol store.
    //! @param[in] PDB file name to import.
    //! @param[in] Name of the symbol store directory
    int ImportPdb(LPCWSTR szPdbFileName, LPCWSTR szSymDir, LPCWSTR szOutFile);

	int DeleteDebugInfo(LPCWSTR szPdbFileName);

#ifdef _WIN32

    //! Creates a minidump file
    int WriteDump(LPCSTR szOutFile);

    //! Creates a minidump file with dbghelp
    int WriteDumpDbgHelp(LPCSTR szOutFile);

#endif

    //! Opens log file
    bool InitLog(std::wstring sFileName, int nLoggingLevel);
    
    //! Replaces our log with another one
    CLog* SubstituteLog(CLog* pLog, bool bOwn = true);
    
    //! Returns last error message
    std::string GetErrorMsg();

    //! Replaces
    CPdbCache* SubstitutePdbCache(CPdbCache* pPdbCache, bool bOwn = true);

private:

    std::string m_sErrorMsg; //!< Last error message
    CLog* m_pLog;            //!< Pointer to log 
    bool m_bLogIsOwned;      //!< Log is owned and should be deleted when we exit
    CPdbCache* m_pPdbCache;  //!< PDB cache.
    bool m_bPdbCacheIsOwned; //!< PDB cache is owned and should be deleted when we exit
};