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

#ifndef asiUI_ProgressNotifier_h
#define asiUI_ProgressNotifier_h

// asiUI includes
#include <asiUI.h>

// asiAlgo includes
#include <asiUI_Logger.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// TBB includes
#include <concurrent_unordered_map.h>
#include <task.h>

// QT includes
#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Task-oriented progress accumulator providing a simple mean of collecting
//! information about execution progress. Each logical task (e.g. Intel
//! TBB one) publishes its current unitless progress value into a dedicated
//! slot of Notifier instance. In order to consult the current percentage of
//! execution completeness, one should sum up the overall progress values by
//! the entire collection of slots and compare it against the declared
//! collector's capacity. This approach appeals to the simplest case of
//! fully deterministic progress indication, when the number of algorithmic
//! steps is known a priori. If not, you may choose to keep a default value
//! for capacity (infinite number). In the latter case, the capacity is
//! suggested as "unknown" and you can just report the number of processed
//! data chunks instead of percentage calculation.
//!
//! Besides pure accumulation purposes, this class also provides mechanism of
//! messaging based on Qt signals.
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
    SummaryProgress() const;

  asiUI_EXPORT void
    DumpProgressMap(Standard_OStream& out) const;

  asiUI_EXPORT virtual void
    StepProgress(const int taskID,
                 const int progressStep);

  asiUI_EXPORT virtual void
    SendLogMessage(const TCollection_AsciiString&  message,
                   const ActAPI_LogMessageSeverity severity,
                   const ActAPI_LogMessagePriority priority = Priority_Normal,
                   const ActAPI_LogArguments&      arguments = ActAPI_LogArguments());

  asiUI_EXPORT virtual void
    SendLogMessage(const ActAPI_LogStream& logStream);

private:

  //! Short-cut for the used type of concurrent collection. We use TBB
  //! UNORDERED MAP in order to be able to safely iterate and change its
  //! buckets concurrently.
  typedef tbb::concurrent_unordered_map<int, int> CMap;

// Concurrent collections:
private:

  //! Storage for cumulative progress.
  CMap m_CMap;

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
