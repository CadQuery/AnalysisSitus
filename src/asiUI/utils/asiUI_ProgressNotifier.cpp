//-----------------------------------------------------------------------------
// Created on: 27 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

// Own include
#include <asiUI_ProgressNotifier.h>

//! ctor.
asiUI_ProgressNotifier::asiUI_ProgressNotifier()
{
  m_pSignal = new Signal();
  m_logger  = new asiAlgo_Logger();
  //
  this->Reset();
}

/* =========================================================================
 *  Section: Methods to use in a single-threaded context
 *  Purpose: E.g. having a single GUI thread responsive to user events,
 *           you can call these methods in that thread, while it is
 *           prohibited to call them in the background working thread
 *           containing several tasks
 * ========================================================================= */

//! Resets Progress Notifier.
void asiUI_ProgressNotifier::Reset()
{
  m_logger->Clear();

  // Reset other members
  m_status          = Progress_Undefined;
  m_iCapacity       = 0;
  m_msgKey          = TCollection_AsciiString();
  m_bIsCancellation = false;
  m_iProgress       = 0;

  // Inform observers that progress is reset
  m_pSignal->EmitStatusChanged(Progress_Undefined);
  m_pSignal->EmitMessageUpdated();
  m_pSignal->EmitProgressUpdated();
}

//! Initializes Progress Notifier and sets up the capacity value.
//! \param capacity [in] capacity value to set.
void asiUI_ProgressNotifier::Init(const int capacity)
{
  // NOTICE:
  // --------------------------------------------------------------------
  // Notice that we do not clean up the internal Logger here as it might
  // contain unprocessed messages still (!)
  // --------------------------------------------------------------------

  // Reset other members
  m_status    = Progress_Running;
  m_iCapacity = capacity;
  m_iProgress = 0;

  // Inform observers that progress gets running
  m_pSignal->EmitStatusChanged(Progress_Running);
  m_pSignal->EmitProgressUpdated();
}

//! \return capacity value.
int asiUI_ProgressNotifier::Capacity() const
{
  return m_iCapacity;
}

//! \return true if the progress scale is infinite.
bool asiUI_ProgressNotifier::IsInfinite() const
{
  return m_iCapacity == INT_MAX;
}

//! Sets message key for the Progress Notifier.
//! \param msgKey [in] message key to set.
void asiUI_ProgressNotifier::SetMessageKey(const TCollection_AsciiString& msgKey)
{
  m_msgKey = msgKey;

  // Inform observers that progress message is changed
  m_pSignal->EmitMessageUpdated();
}

//! \return associated message key.
TCollection_AsciiString asiUI_ProgressNotifier::MessageKey() const
{
  return m_msgKey;
}

//! Sets the progress status to the given value.
//! \param status [in] progress status to set.
void asiUI_ProgressNotifier::SetProgressStatus(const ActAPI_ProgressStatus status)
{
  m_status = status;

  // Inform observers that progress status is changed
  m_pSignal->EmitStatusChanged(status);
}

//! \return progress status.
ActAPI_ProgressStatus asiUI_ProgressNotifier::ProgressStatus() const
{
  return m_status;
}

//! Sends cancellation requests to the task context.
void asiUI_ProgressNotifier::Cancel()
{
  m_bIsCancellation = true;
}

//! Checks whether the cancellation is in progress. This method is designed
//! to be invoked within algorithms.
//! \return true/false.
bool asiUI_ProgressNotifier::IsCancelling()
{
  return m_bIsCancellation;
}

//! Checks whether the job is in running state.
//! \return true/false.
bool asiUI_ProgressNotifier::IsRunning()
{
  return (m_status == Progress_Running);
}

//! Checks whether the job is in failed state.
//! \return true/false.
bool asiUI_ProgressNotifier::IsFailed()
{
  return (m_status == Progress_Failed);
}

/* =========================================================================
 *  Section: Thread-safe methods
 * ========================================================================= */

//! \return current progress.
int asiUI_ProgressNotifier::CurrentProgress() const
{
  return m_iProgress;
}

//! Use this method to report the next progress value.
//! \param stepProgress [in] next progress value.
void asiUI_ProgressNotifier::StepProgress(const int stepProgress)
{
  m_iProgress += stepProgress;

  // Inform observers that progress value is updated
  m_pSignal->EmitProgressUpdated();
}

//! Use this method to report the next progress value.
//! \param progress [in] next progress value.
void asiUI_ProgressNotifier::SetProgress(const int progress)
{
  m_iProgress = progress;

  // Inform observers that progress value is updated
  m_pSignal->EmitProgressUpdated();
}

//! Thread-safe method used to send a logging message. Normally, it is
//! not a GUI stuff directly as Progress Notifier is designed for usage in
//! multi-threaded environment.
//!
//! \param message   [in] message string (normally it is i18n key).
//! \param severity  [in] message severity (info, warning, error).
//! \param priority  [in] message priority (normal, high).
//! \param arguments [in] message arguments (if any).
void asiUI_ProgressNotifier::SendLogMessage(const TCollection_AsciiString&  message,
                                            const ActAPI_LogMessageSeverity severity,
                                            const ActAPI_LogMessagePriority priority,
                                            const ActAPI_LogArguments&      arguments)
{
  switch ( severity )
  {
    case Severity_Information:
      m_logger->Info(message, priority, arguments);
      break;
    case Severity_Notice:
      m_logger->Notice(message, priority, arguments);
      break;
    case Severity_Warning:
      m_logger->Warn(message, priority, arguments);
      break;
    case Severity_Error:
      m_logger->Error(message, priority, arguments);
      break;
    default:
      break;
  }

  // Inform observers that new logging message is available
  m_pSignal->EmitLogMessageSent();
}

//! Thread-safe method used to send a logging message in a stream form.
//! Normally, it is not a GUI stuff directly as Progress Notifier is designed
//! for usage in multi-threaded environment.
//!
//! \param logStream [in] logger stream.
void asiUI_ProgressNotifier::SendLogMessage(const ActAPI_LogStream& logStream)
{
  this->SendLogMessage( logStream.Text(),
                        logStream.Severity(),
                        logStream.Priority(),
                        logStream.Args() );
}
