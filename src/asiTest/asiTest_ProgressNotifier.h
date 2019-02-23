//-----------------------------------------------------------------------------
// Created on: 22 June 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiTest_ProgressNotifier_h
#define asiTest_ProgressNotifier_h

// asiTest includes
#include <asiTest.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

#ifdef USE_TBB
  // TBB includes
  #include <atomic.h>
#endif

//! Notification tool to take care of algorithmic messages.
class asiTest_ProgressNotifier : public ActAPI_IProgressNotifier
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiTest_ProgressNotifier, ActAPI_IProgressNotifier)

public:

  //! Constructor.
  //! \param[in,out] os output stream.
  asiTest_ProgressNotifier(Standard_OStream& os);

// Thread-unsafe methods:
public:

  //! Cleans up the internal state of the Progress Notifier, so that it
  //! becomes ready to track another job.
  virtual void
    Reset();

  //! Initializes the Progress Notifier with the deterministic capacity
  //! value. Capacity represents the unitless overall progress value which
  //! can be ever collected by all running tasks.
  //!
  //! Please note, that by default the progress scale is declared with
  //! infinite capacity. Practically, it means that algorithm is not able
  //! to foresee the number of steps it will need to complete. Be sure that
  //! in such a case your interface reacts adequately (e.g. no percentage is
  //! shown to the user).
  //!
  //! \param[in] capacity capacity to set (infinite by default: INT_MAX).
  virtual void
    Init(const int capacity = INT_MAX);

  //! Returns the capacity value.
  //! \return requested capacity value.
  virtual int
    Capacity() const;

  //! Returns true if the capacity value is infinite.
  //! \return true/false.
  virtual bool
    IsInfinite() const;

  //! Sets message localization key.
  //! \param[in] msgKey localization key to set.
  virtual void
    SetMessageKey(const TCollection_AsciiString& msgKey);

  //! Returns message localization key.
  //! \return localization key.
  virtual TCollection_AsciiString
    MessageKey() const;

  //! Sets the ultimate progress status for the job.
  //! \param[in] status progress status to set.
  virtual void
    SetProgressStatus(const ActAPI_ProgressStatus status);

  //! Returns current progress status.
  //! \return ultimate progress status.
  virtual ActAPI_ProgressStatus
    ProgressStatus() const;

  //! Requests job cancellation.
  virtual void
    Cancel();

  //! Checks whether the job is being cancelled.
  //! \return true/false.
  virtual bool
    IsCancelling();

  //! Checks whether the job is in running state.
  //! \return true/false.
  virtual bool
    IsRunning();

  //! Checks whether the job is in failed state.
  //! \return true/false.
  virtual bool
    IsFailed();

  //! Returns the currently cumulated progress value.
  //! \return current cumulative progress.
  virtual int
    CurrentProgress() const;

// Methods to be used by parallel algorithms (should be thread-safe):
public:

  //! Thread-safe method used to increment the progress value by the passed step.
  //! \param[in] progressStep progress value to increment by.
  virtual void
    StepProgress(const int progressStep);

  //! Thread-safe method used to set the progress value.
  //! \param[in] progress progress value to set.
  virtual void
    SetProgress(const int progress);

  //! Thread-safe method used to send a logging message. Normally, this is
  //! not GUI directly as Progress Notifier is designed for usage in
  //! multi-threaded environment.
  //! \param[in] message   message string (normally it is i18n key).
  //! \param[in] severity  message severity (info, warning, error).
  //! \param[in] priority  message priority (normal, high).
  //! \param[in] arguments message arguments (if any).
  virtual void
    SendLogMessage(const TCollection_AsciiString&  message,
                   const ActAPI_LogMessageSeverity severity,
                   const ActAPI_LogMessagePriority priority  = Priority_Normal,
                   const ActAPI_LogArguments&      arguments = ActAPI_LogArguments());

  //! Thread-safe method used to send a logging message in a stream form.
  //! Normally, this is not GUI directly as Progress Notifier is designed for
  //! usage in multi-threaded environment.
  //! \param[in] logStream logging stream.
  virtual void
    SendLogMessage(const ActAPI_LogStream& logStream);

// Concurrent collections:
private:

  //! Output stream.
  Standard_OStream& m_out;

  //! Current progress.
#ifdef USE_TBB
  tbb::atomic<int> m_iProgress;
#else
  int m_iProgress;
#endif

// Single-threaded members:
private:

  //! Maximum allowed capacity.
  int m_iCapacity;

  //! Message key for short-description the working algorithm might want
  //! to associate with the Progress Collector.
  TCollection_AsciiString m_msgKey;

  //! Progress status.
  ActAPI_ProgressStatus m_status;

  bool m_SendLogMessageCalled;

private:

  void operator=(const asiTest_ProgressNotifier&) = delete;
  asiTest_ProgressNotifier(const asiTest_ProgressNotifier& pn) = delete;

};

#endif
