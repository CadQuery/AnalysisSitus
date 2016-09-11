//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef journal_notifier_h
#define journal_notifier_h

// Journaling includes
#include <journal_logger.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// TBB includes
#include <concurrent_unordered_map.h>

// Qt include
#include <QObject>

DEFINE_STANDARD_HANDLE(journal_notifier, ActAPI_IProgressNotifier)

//! Notification tool to send algorithmic messages.
class journal_notifier : public ActAPI_IProgressNotifier
{
  class signal;

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(journal_notifier, ActAPI_IProgressNotifier)

public:

  journal_notifier();

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
  //! \param theCapacity [in] capacity to set (infinite by default: INT_MAX).
  virtual void
    Init(const int theCapacity = INT_MAX);

  //! Returns the capacity value.
  //! \return requested capacity value.
  virtual int
    Capacity() const;

  //! Returns true if the capacity value is infinite.
  //! \return true/false.
  virtual bool
    IsInfinite() const;

  //! Sets message localization key.
  //! \param theMsgKey [in] localization key to set.
  virtual void
    SetMessageKey(const TCollection_AsciiString& theMsgKey);

  //! Returns message localization key.
  //! \return localization key.
  virtual TCollection_AsciiString
    MessageKey() const;

  //! Sets the ultimate progress status for the job.
  //! \param theStatus [in] progress status to set.
  virtual void
    SetProgressStatus(const ActAPI_ProgressStatus theStatus);

  //! Returns current progress status.
  //! \return the ultimate progress status.
  virtual ActAPI_ProgressStatus
    ProgressStatus() const;

  //! Requests job cancellation.
  virtual void
    Cancel();

  //! Checks whether the job is being canceled.
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
    SummaryProgress() const;

  void
    DumpProgressMap(Standard_OStream& theOS) const;

// Methods to be used by parallel algorithms (should be thread-safe):
public:

  //! Thread-safe method used to increment the progress value associated with
  //! the given task ID by the passed step.
  //! \param theTaskID [in] ID of the task to increment the progress for.
  //! \param theProgressStep [in] progress value to increment by.
  virtual void
    StepProgress(const int theTaskID,
                 const int theProgressStep);

  //! Thread-safe method used to send a logging message. Normally, this is
  //! not GUI directly as Progress Notifier is designed for usage in
  //! multi-threaded environment.
  //! \param theMessage [in] message string (normally it is i18n key).
  //! \param theSeverity [in] message severity (info, warning, error).
  //! \param thePriority [in] message priority (normal, high).
  //! \param theArguments [in] message arguments (if any).
  virtual void
    SendLogMessage(const TCollection_AsciiString&  theMessage,
                   const ActAPI_LogMessageSeverity theSeverity,
                   const ActAPI_LogMessagePriority thePriority  = Priority_Normal,
                   const ActAPI_LogArguments&      theArguments = ActAPI_LogArguments());

  //! Thread-safe method used to send a logging message in a stream form.
  //! Normally, this is not GUI directly as Progress Notifier is designed for
  //! usage in multi-threaded environment.
  //! \param theLogStream [in] logging stream.
  virtual void
    SendLogMessage(const ActAPI_LogStream& theLogStream);

public:

  //! \return wrapped QObject radiating signals.
  inline signal* Signal() const
  {
    return m_pSignal;
  }

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
  Handle(journal_logger) m_logger;

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
  unsigned int m_bIsCancellation;

  //! Signal notifier.
  signal* m_pSignal;

};

//-----------------------------------------------------------------------------

//! Emits notification signals.
class journal_notifier::signal : public QObject
{
  Q_OBJECT

public:

  //! Emits "StatusChanged" signal.
  //! \param theStatus [in] the updated status.
  inline void EmitStatusChanged(const ActAPI_ProgressStatus theStatus)
  {
    emit StatusChanged(theStatus);
  }

  //! Emits "MessageUpdated" signal.
  inline void EmitMessageUpdated()
  {
    emit MessageUpdated();
  }

  //! Emits "LogMessageSent" signal.
  inline void EmitLogMessageSent()
  {
    emit LogMessageSent();
  }

  //! Emits "ProgressUpdated" signal.
  inline void EmitProgressUpdated()
  {
    emit ProgressUpdated();
  }

signals:

  //! The signal is emitted when progress status changes.
  //! \param theStatus [in] the updated status.
  void StatusChanged(const int theStatus);

  //! The signal is emitted when message key updated.
  void MessageUpdated();

  //! The signal is emitted when logging message is sent.
  void LogMessageSent();

  //! The signal is emitted when progress updated.
  void ProgressUpdated();

};

#endif
