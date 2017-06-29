//-----------------------------------------------------------------------------
// Created on: 27 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
  // Clean up the cumulative collections
  m_CMap.clear();
  m_logger->Clear();

  // Reset other members
  m_status          = Progress_Undefined;
  m_iCapacity       = 0;
  m_msgKey          = TCollection_AsciiString();
  m_bIsCancellation = false;

  // Inform observers that progress is reset
  m_pSignal->EmitStatusChanged(Progress_Undefined);
  m_pSignal->EmitMessageUpdated();
  m_pSignal->EmitProgressUpdated();
}

//! Initializes Progress Notifier and sets up the capacity value.
//! \param capacity [in] capacity value to set.
void asiUI_ProgressNotifier::Init(const int capacity)
{
  // Clean up the cumulative collections
  m_CMap.clear();

  // NOTICE:
  // --------------------------------------------------------------------
  // Notice that we do not clean up the internal Logger here as it might
  // contain unprocessed messages still (!)
  // --------------------------------------------------------------------

  // Reset other members
  m_status    = Progress_Running;
  m_iCapacity = capacity;

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

//! Collects the currently cumulated progress summing up the progress
//! values for all registered tasks.
//! \return cumulative progress value.
int asiUI_ProgressNotifier::SummaryProgress() const
{
  int result = 0;
  //
  for ( CMap::const_iterator cit = m_CMap.begin(); cit != m_CMap.end(); cit++ )
  {
    result += cit->second;
  }

  return result;
}

//! Dumps the contents of the internal thread-safe collector slots to the
//! passed output stream.
//! \param out [in/out] output stream to dump data to.
void asiUI_ProgressNotifier::DumpProgressMap(Standard_OStream& out) const
{
  for ( CMap::const_iterator cit = m_CMap.begin(); cit != m_CMap.end(); cit++ )
  {
    out << cit->first << " :: " << cit->second << "\n";
  }
}

//! Use this method to report the next progress value in your task. The passed
//! value will be added into cumulative progress collection.
//! \param taskID       [in] ID of the task reporting the progress.
//! \param stepProgress [in] next progress value.
void asiUI_ProgressNotifier::StepProgress(const int taskID,
                                          const int stepProgress)
{
  if ( m_CMap.find(taskID) == m_CMap.end() )
    m_CMap[taskID] = stepProgress;
  else
    m_CMap[taskID] += stepProgress;

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
