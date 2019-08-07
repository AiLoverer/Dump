#include "stdafx.h"
#include "Thread.h"

CThread::CThread()
{
    m_bValid = false;
    m_pParam = NULL;
    m_lRetCode = 0;
#ifdef _WIN32
    m_hThread = NULL;
#endif
}

CThread::~CThread()
{
#ifdef _WIN32
    // Check thread is stopped
    assert(m_hThread==NULL);
#endif
}

bool CThread::Run(void* pParam)
{
    // Save parameter
    m_pParam = pParam;

#ifdef _WIN32
    m_hThread = CreateThread(NULL, 0, Win32ThreadProc, this, 0, NULL);
    if(m_hThread==NULL)
        return false;
#else
    int nResult = pthread_create(&m_hThread, NULL, LinuxThreadFunc, this);
    if(nResult!=0)
        return false;
#endif

    m_bValid = true;
    return true;
}

void CThread::Wait()
{
    if(!m_bValid)
        return;

#ifdef _WIN32
    WaitForSingleObject(m_hThread, INFINITE);
    m_hThread=NULL;
#else
    void* ret_val = NULL;
    pthread_join(m_hThread, &ret_val);
#endif
}

#ifdef _WIN32
DWORD CThread::Win32ThreadProc(LPVOID lpParam)
{
    CThread* pThread = (CThread*)lpParam;
    pThread->m_lRetCode = pThread->ThreadProc(pThread->m_pParam);
    return 0;
}
#else
void* CThread::LinuxThreadFunc(void* lpParam)
{
    CThread* pThread = (CThread*)lpParam;
    pThread->m_lRetCode = (long)pThread->ThreadProc(pThread->m_pParam);
    return NULL;
}
#endif

void CThread::Exit(int nCode)
{
#ifdef _WIN32
    TerminateThread(m_hThread, nCode);
#else
    pthread_exit(NULL); // Terminate this thread.
#endif
}


