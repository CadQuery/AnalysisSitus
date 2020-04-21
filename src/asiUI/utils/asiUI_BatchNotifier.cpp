//-----------------------------------------------------------------------------
// Created on: 19 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

// Own include
#include <asiUI_BatchNotifier.h>

// OCCT includes
#include <Message_MsgFile.hxx>

// TBB includes
#ifdef USE_TBB
#include <spin_mutex.h>
#endif

//-----------------------------------------------------------------------------

asiUI_BatchNotifier::asiUI_BatchNotifier(const Handle(ActAPI_ILogger)& logger)
: ActAPI_IProgressNotifier (),
  m_logger                 (logger)
{
  this->Reset();
}

/* =========================================================================
 *  Section: Methods to use in a single-threaded context
 *  Purpose: E.g. having a single GUI thread responsive to user events,
 *           you can call these methods in that thread, while it is
 *           prohibited to call them in the background working thread
 *           containing several tasks
 * ========================================================================= */

void asiUI_BatchNotifier::Reset()
{
  m_status     = Progress_Undefined;
  m_iCapacity  = 0;
  m_msgKey     = TCollection_AsciiString();
  m_iProgress  = 0;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::Init(const int capacity)
{
  m_status    = Progress_Running;
  m_iCapacity = capacity;
  m_iProgress = 0;
}

//-----------------------------------------------------------------------------

int asiUI_BatchNotifier::Capacity() const
{
  return m_iCapacity;
}

//-----------------------------------------------------------------------------

bool asiUI_BatchNotifier::IsInfinite() const
{
  return m_iCapacity == INT_MAX;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::SetMessageKey(const TCollection_AsciiString& msgKey)
{
  m_msgKey = msgKey;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString asiUI_BatchNotifier::MessageKey() const
{
  return m_msgKey;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::SetProgressStatus(const ActAPI_ProgressStatus status)
{
  m_status = status;

  if ( m_status >= Progress_Succeeded )
  {
    TCollection_AsciiString prefix, verdict;
    //
    if ( m_status == Progress_Succeeded )
    {
      prefix = " + ";
      verdict = "succeeded";
    }
    else if ( m_status == Progress_Failed )
    {
      prefix = " - ";
      verdict = "failed";
    }
    else
    {
      prefix = " ? ";
      verdict = "done";
    }

    this->SendLogMessage(LogInfo(Normal) << "Operation %1." << verdict);
  }
}

//-----------------------------------------------------------------------------

ActAPI_ProgressStatus asiUI_BatchNotifier::ProgressStatus() const
{
  return m_status;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::Cancel()
{
  // Cannot cancel in batch mode.
}

//-----------------------------------------------------------------------------

bool asiUI_BatchNotifier::IsCancelling()
{
  return false;
}

//-----------------------------------------------------------------------------

bool asiUI_BatchNotifier::IsRunning()
{
  return (m_status == Progress_Running);
}

//-----------------------------------------------------------------------------

bool asiUI_BatchNotifier::IsFailed()
{
  return (m_status == Progress_Failed);
}

/* =========================================================================
 *  Section: Thread-safe methods
 * ========================================================================= */

int asiUI_BatchNotifier::CurrentProgress() const
{
  return m_iProgress;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::StepProgress(const int stepProgress)
{
  m_iProgress += stepProgress;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::SetProgress(const int progress)
{
  m_iProgress = progress;
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::SendLogMessage(const TCollection_AsciiString&  message,
                                          const ActAPI_LogMessageSeverity severity,
                                          const ActAPI_LogMessagePriority priority,
                                          const ActAPI_LogArguments&      arguments)
{
  if ( severity == Severity_Information )
    m_logger->Info(message, priority, arguments);
  else if ( severity == Severity_Notice )
    m_logger->Notice(message, priority, arguments);
  else if ( severity == Severity_Warning )
    m_logger->Warn(message, priority, arguments);
  else
    m_logger->Error(message, priority, arguments);
}

//-----------------------------------------------------------------------------

void asiUI_BatchNotifier::SendLogMessage(const ActAPI_LogStream& logStream)
{
  this->SendLogMessage( logStream.Text(),
                        logStream.Severity(),
                        logStream.Priority(),
                        logStream.Args() );
}
