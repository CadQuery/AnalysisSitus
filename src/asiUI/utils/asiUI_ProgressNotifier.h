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

#ifndef asiUI_ProgressNotifier_h
#define asiUI_ProgressNotifier_h

// asiUI includes
#include <asiUI.h>

// asiAlgo includes
#include <asiUI_Logger.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// TBB includes
#include <atomic.h>

// QT includes
#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Progress notification tool.
class asiUI_ProgressNotifier : public ActAPI_IProgressNotifier
{
  class Signal;

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_ProgressNotifier, ActAPI_IProgressNotifier)

// Methods to use in single-threaded context:
public:

  asiUI_EXPORT
    asiUI_ProgressNotifier();

// Thread-unsafe methods:
public:

  asiUI_EXPORT virtual void
    Reset();

  asiUI_EXPORT virtual void
    Init(const int capacity = INT_MAX);

  asiUI_EXPORT virtual int
    Capacity() const;

  asiUI_EXPORT virtual bool
    IsInfinite() const;

  asiUI_EXPORT virtual void
    SetMessageKey(const TCollection_AsciiString& msgKey);

  asiUI_EXPORT virtual TCollection_AsciiString
    MessageKey() const;

  asiUI_EXPORT virtual void
    SetProgressStatus(const ActAPI_ProgressStatus status);

  asiUI_EXPORT virtual ActAPI_ProgressStatus
    ProgressStatus() const;

  asiUI_EXPORT virtual void
    Cancel();

  asiUI_EXPORT virtual bool
    IsCancelling();

  asiUI_EXPORT virtual bool
    IsRunning();

  asiUI_EXPORT virtual bool
    IsFailed();

public:

  //! \return internal algorithmic Logger instance.
  const Handle(asiAlgo_Logger)& AlgoLogger() const
  {
    return m_logger;
  }

  //! \return Qt signal emitting entity. Listeners can connect their slots
  //!         using Qt::AutomaticConnection to notifier signals to receive
  //!         non-delayed signals emitted both in the same thread or from
  //!         the algorithm's threads.
  Signal* SignalNotifier() const
  {
    return m_pSignal;
  }

// Tread-safe methods to be used by algorithms:
public:

  asiUI_EXPORT virtual int
    CurrentProgress() const;

  asiUI_EXPORT virtual void
    StepProgress(const int progressStep);

  asiUI_EXPORT virtual void
    SetProgress(const int progress);

  asiUI_EXPORT virtual void
    SendLogMessage(const TCollection_AsciiString&  message,
                   const ActAPI_LogMessageSeverity severity,
                   const ActAPI_LogMessagePriority priority = Priority_Normal,
                   const ActAPI_LogArguments&      arguments = ActAPI_LogArguments());

  asiUI_EXPORT virtual void
    SendLogMessage(const ActAPI_LogStream& logStream);

// Concurrent collections:
private:

  //! Storage for progress.
  tbb::atomic<int> m_iProgress;

  //! Storage for logging messages.
  Handle(asiAlgo_Logger) m_logger;

// Single-threaded members:
private:

  //! Maximum allowed capacity.
  int m_iCapacity;

  //! Message key for short-description the working algorithm might want
  //! to associate with the Progress Collector.
  TCollection_AsciiString m_msgKey;

  //! Progress status.
  ActAPI_ProgressStatus m_status;

  //! Indicates whether the cancellation is in progress.
  bool m_bIsCancellation;

  //! Signal notifier.
  Signal* m_pSignal;

};

//-----------------------------------------------------------------------------

//! Emitter of progress notification signals.
class asiUI_ProgressNotifier::Signal : public QObject
{
  Q_OBJECT

public:

  //! Emits "StatusChanged" signal.
  //! \param status [in] updated status.
  void EmitStatusChanged(const ActAPI_ProgressStatus status)
  {
    emit StatusChanged(status);
  }

  //! Emits "MessageUpdated" signal.
  void EmitMessageUpdated()
  {
    emit MessageUpdated();
  }

  //! Emits "LogMessageSent" signal.
  void EmitLogMessageSent()
  {
    emit LogMessageSent();
  }

  //! Emits "ProgressUpdated" signal.
  void EmitProgressUpdated()
  {
    emit ProgressUpdated();
  }

signals:

  //! The signal is emitted when progress status changes.
  //! \param status [in] updated status.
  void StatusChanged(const int status);

  //! The signal is emitted when message key updated.
  void MessageUpdated();

  //! The signal is emitted when logging message is sent.
  void LogMessageSent();

  //! The signal is emitted when progress updated.
  void ProgressUpdated();

};

#endif
