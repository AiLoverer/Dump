// File: CritSec.h
// Description: Critical section wrapper classes. Code of CCritSec and CAutoLock classes
// is taken from DirectShow base classes and modified in some way.

#ifndef _CRITSEC_H
#define _CRITSEC_H

#ifndef _WIN32
#include <pthread.h>
#endif

// wrapper for whatever critical section we have
class CCritSec
{
    // make copy constructor and assignment operator inaccessible

    CCritSec(const CCritSec &refCritSec);
    CCritSec &operator=(const CCritSec &refCritSec);

#ifdef _WIN32
    CRITICAL_SECTION m_CritSec;
#else
    pthread_mutex_t m_cs_mutex;
#endif

public:

    CCritSec()
    {
#ifdef _WIN32
        InitializeCriticalSection(&m_CritSec);
#else
        pthread_mutexattr_t mAttr;
		pthread_mutexattr_init(&mAttr);
        pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);
        pthread_mutex_init(&m_cs_mutex, &mAttr);
        pthread_mutexattr_destroy(&mAttr);
#endif
    };

    ~CCritSec()
    {
#ifdef _WIN32
        DeleteCriticalSection(&m_CritSec);
#endif
    }

    void Lock()
    {
#ifdef _WIN32
        EnterCriticalSection(&m_CritSec);
#else
        pthread_mutex_lock(&m_cs_mutex);
#endif
    };

    void Unlock()
    {
#ifdef _WIN32
        LeaveCriticalSection(&m_CritSec);
#else
        pthread_mutex_unlock(&m_cs_mutex);
#endif
    };
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAutoLock
{
    // make copy constructor and assignment operator inaccessible

    CAutoLock(const CAutoLock &refAutoLock);
    CAutoLock &operator=(const CAutoLock &refAutoLock);

protected:
    CCritSec * m_pLock;

public:
    CAutoLock(CCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~CAutoLock()
    {
        m_pLock->Unlock();
    };
};

class CCondVar
{
public:

    CCondVar()
    {
#ifdef _WIN32
        m_hSemaphore = CreateSemaphore(NULL, 0, 100, NULL);
#else
        pthread_mutex_init(&m_Mutex, NULL);
        pthread_cond_init(&m_Cond, NULL);
#endif
    }

    ~CCondVar()
    {
#ifdef _WIN32
        CloseHandle(m_hSemaphore);
#else
        pthread_mutex_destroy(&m_Mutex);
        pthread_cond_destroy(&m_Cond);
#endif
    }

    void Signal()
    {
#ifdef _WIN32
        ReleaseSemaphore(m_hSemaphore, 1, NULL);
#else
        pthread_cond_signal(&m_Cond);
#endif
    }

    void Wait()
    {
#ifdef _WIN32
        WaitForSingleObject(m_hSemaphore, INFINITE);
#else
        pthread_cond_wait(&m_Cond, &m_Mutex);
#endif
    }

private:

#ifdef _WIN32
    HANDLE m_hSemaphore;
#else
    pthread_mutex_t m_Mutex;
    pthread_cond_t  m_Cond;
#endif

};

#endif  //_CRITSEC_H
