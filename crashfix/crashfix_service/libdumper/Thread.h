#pragma once
#include "stdafx.h"

//! \class CThread
//! \brief A cross platform thread.
class CThread
{
public:

    //! Constructor.
    CThread();

    //! Destructor.
    ~CThread();

    //! Creates the thread and executes the thread procedure.
    //! \return true on success.
    //! @param[in] pParam Parameter to pass to thread procedure.
    bool Run(void* pParam);

    //! Blocks until thread exits its procedure.
    void Wait();
		
protected:

    //! Terminates the thread.
    //! @param[in] nCode Exit code.
    void Exit(int nCode);

#ifdef _WIN32
    //! Win32-specific thread procedure
    static DWORD WINAPI Win32ThreadProc(LPVOID lpParam);
#else
    //! Linux-specific thread func
    static void* LinuxThreadFunc(void* pParam);
#endif

    //! Platform-independent thread proc
    virtual long ThreadProc(void* pParam) = 0;

#ifdef _WIN32
    HANDLE m_hThread;    //!< Handle to thread
#else
    pthread_t m_hThread; //!< Handle to thread
#endif

    void* m_pParam;   //!< Parameter passed to thread
    long m_lRetCode;  //!< Thread's exit code
    bool m_bValid;    //!< Is this thread is a valid thread?
};
