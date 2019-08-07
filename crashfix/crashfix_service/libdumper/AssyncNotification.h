//! \file AssyncNotification.h
//! \brief Provides a way to communicate between sender thread and the main thread.
//! \author zexspectrum
//! \date 2009

#pragma once
#include "stdafx.h"

//! Assynchronous notification
struct AssyncNotification
{
    //! Constructor 
    AssyncNotification();  

    //! Resets the event
    void Reset();

    //! Sets the progress message and percent completed
    void SetProgress(CString sStatusMsg, int percentCompleted, bool bRelative=true);

    //! Sets the percent completed
    void SetProgress(int percentCompleted, bool bRelative=true);

    //! Returns the current assynchronious operation progress
    void GetProgress(int& nProgressPct, std::vector<CString>& msg_log);

    //! Notifies about assynchronious operation completion
    void SetCompleted(int nCompletionStatus);

    //! Blocks until assynchronious operation is completed
    int WaitForCompletion();

    //! Cancels the assynchronious operation
    void Cancel();

    //! Determines if the assynchronous operation was cancelled.
    bool IsCancelled();

	//! Determines if the assynchronous operation has completed.
	bool IsCompleted();

    //! Waits until the feedback is received
    void WaitForFeedback(int &code);

    //! Notifies about feedback is ready to be received
    void FeedbackReady(int code);

private:

    CComAutoCriticalSection m_cs; //!< Protects internal state
    int m_nCompletionStatus;      //!< Completion status of the assync operation
    HANDLE m_hCompletionEvent;    //!< Completion event
    HANDLE m_hCancelEvent;        //!< Cancel event
    HANDLE m_hFeedbackEvent;      //!< Feddback event
    int m_nPercentCompleted;      //!< Percent completed
    std::vector<CString> m_statusLog; //!< Status log
};
