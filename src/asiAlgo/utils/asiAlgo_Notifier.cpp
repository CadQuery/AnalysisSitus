//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_Notifier.h>

#undef COUT_OUTPUT

//! Constructor.
asiAlgo_Notifier::asiAlgo_Notifier() : ActAPI_IProgressNotifier()
{
  m_pSignal = new signal();
  m_logger  = new asiAlgo_Logger();

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
//! \param theMsgKey [in] message key to set.
void asiAlgo_Notifier::Reset()
{
  // Clean up the cumulative collections
  m_CMap.clear();
  m_logger->Clear();

  // Reset other members
  m_status          = Progress_Undefined;
  m_iCapacity       = 0;
  m_msgKey          = TCollection_AsciiString();
  m_bIsCancellation = Standard_False;

  // Inform observers that progress is reset
  m_pSignal->EmitStatusChanged(Progress_Undefined);
  m_pSignal->EmitMessageUpdated();
  m_pSignal->EmitProgressUpdated();
}

//! Initializes Progress Notifier and sets up the capacity value.
//! \param theCapacity [in] capacity value to set.
void asiAlgo_Notifier::Init(const int theCapacity)
{
  // Clean up the cumulative collections
  m_CMap.clear();

  // ...
  // Notice that we do not clean up the internal Logger here as it might
  // contain unprocessed messages still (!)
  // ...

  // Reset other members
  m_status    = Progress_Running;
  m_iCapacity = theCapacity;

  // Inform observers that progress gets running
  m_pSignal->EmitStatusChanged(Progress_Running);
  m_pSignal->EmitProgressUpdated();
}

//! Returns the capacity value.
//! \return capacity value.
int asiAlgo_Notifier::Capacity() const
{
  return m_iCapacity;
}

//! Returns true if the progress scale is infinite.
//! \return true/false.
bool asiAlgo_Notifier::IsInfinite() const
{
  return m_iCapacity == INT_MAX;
}

//! Sets message key for the Progress Notifier.
//! \param theMsgKey [in] message key to set.
void asiAlgo_Notifier::SetMessageKey(const TCollection_AsciiString& theMsgKey)
{
  m_msgKey = theMsgKey;

  // Inform observers that progress message is changed
  m_pSignal->EmitMessageUpdated();
}

//! Accessor for the message key associated with the Progress Notifier.
//! \return associated message key.
TCollection_AsciiString asiAlgo_Notifier::MessageKey() const
{
  return m_msgKey;
}

//! Sets the progress status to the given value.
//! \param theStatus [in] progress status to set.
void asiAlgo_Notifier::SetProgressStatus(const ActAPI_ProgressStatus theStatus)
{
  m_status = theStatus;

  // Inform observers that progress status is changed
  m_pSignal->EmitStatusChanged(theStatus);
}

//! Accessor for the progress status.
//! \return progress status.
ActAPI_ProgressStatus asiAlgo_Notifier::ProgressStatus() const
{
  return m_status;
}

//! Sends cancellation requests to the task context.
void asiAlgo_Notifier::Cancel()
{
  m_bIsCancellation = Standard_True;
}

//! Checks whether the cancellation is in progress.
//! \return true/false.
bool asiAlgo_Notifier::IsCancelling()
{
  return m_bIsCancellation;
}

//! Checks whether the job is in running state.
//! \return true/false.
bool asiAlgo_Notifier::IsRunning()
{
  return (m_status == Progress_Running);
}

//! Checks whether the job is in failed state.
//! \return true/false.
bool asiAlgo_Notifier::IsFailed()
{
  return (m_status == Progress_Failed);
}

/* =========================================================================
 *  Section: Thread-safe methods
 * ========================================================================= */

//! Collects the currently cumulated progress summing up the progress
//! values for all registered tasks.
//! \return cumulative progress value.
int asiAlgo_Notifier::SummaryProgress() const
{
  int aResult = 0;
  for ( CMap::const_iterator cit = m_CMap.begin(); cit != m_CMap.end(); cit++ )
  {
    aResult += cit->second;
  }

  return aResult;
}

//! Dumps the contents of the internal thread-safe collecting slots to the
//! passed output stream.
//! \param theOS [in/out] output stream to dump data to.
void asiAlgo_Notifier::DumpProgressMap(Standard_OStream& theOS) const
{
  for ( CMap::const_iterator cit = m_CMap.begin(); cit != m_CMap.end(); cit++ )
  {
    theOS << cit->first << " :: " << cit->second << "\n";
  }
}

//! Use this method to report the next progress value in your task. The passed
//! value will be added into cumulative progress collection.
//! \param theTaskID       [in] ID of the task reporting the progress.
//! \param theStepProgress [in] next progress value.
void asiAlgo_Notifier::StepProgress(const int theTaskID,
                                    const int theStepProgress)
{
  if ( m_CMap.find(theTaskID) == m_CMap.end() )
    m_CMap[theTaskID] = theStepProgress;
  else
    m_CMap[theTaskID] += theStepProgress;

#if defined COUT_OUTPUT
  const int perc = ( (double) this->SummaryProgress() / this->Capacity() )*100;
  std::cout << "... \tprogress: " << perc << "%" << std::endl;
#endif

  // Inform observers that progress value is updated
  m_pSignal->EmitProgressUpdated();
}

//! Thread-safe method used to send a logging message. Normally, this is
//! not GUI directly as Progress Notifier is designed for usage in
//! multi-threaded environment.
//! \param theMessage   [in] message string (normally it is i18n key).
//! \param theSeverity  [in] message severity (info, warning, error).
//! \param thePriority  [in] message priority (normal, high).
//! \param theArguments [in] message arguments (if any).
void asiAlgo_Notifier::SendLogMessage(const TCollection_AsciiString&  theMessage,
                                             const ActAPI_LogMessageSeverity theSeverity,
                                             const ActAPI_LogMessagePriority thePriority,
                                             const ActAPI_LogArguments&      theArguments)
{
  switch ( theSeverity )
  {
    case Severity_Information:
      m_logger->Info(theMessage, thePriority, theArguments);
#if defined COUT_OUTPUT
      std::cout << "... \tinfo: " << theMessage << std::endl;
#endif
      break;
    case Severity_Notice:
      m_logger->Notice(theMessage, thePriority, theArguments);
#if defined COUT_OUTPUT
      std::cout << "... \tnotice: " << theMessage << std::endl;
#endif
      break;
    case Severity_Warning:
      m_logger->Warn(theMessage, thePriority, theArguments);
#if defined COUT_OUTPUT
      std::cout << "... \twarning: " << theMessage << std::endl;
#endif
      break;
    case Severity_Error:
      m_logger->Error(theMessage, thePriority, theArguments);
#if defined COUT_OUTPUT
      std::cout << "... \terror: " << theMessage << std::endl;
#endif
      break;
    default:
      break;
  }

  // Inform observers that new logging message is available
  m_pSignal->EmitLogMessageSent();
}

//! Thread-safe method used to send a logging message in a stream form.
//! Normally, this is not GUI directly as Progress Notifier is designed for
//! usage in multi-threaded environment.
//! \param theMessage   [in] message string (normally it is i18n key).
//! \param theSeverity  [in] message severity (info, warning, error).
//! \param thePriority  [in] message priority (normal, high).
//! \param theArguments [in] message arguments (if any).
void asiAlgo_Notifier::SendLogMessage(const ActAPI_LogStream& theLogStream)
{
  this->SendLogMessage( theLogStream.Text(),
                        theLogStream.Severity(),
                        theLogStream.Priority(),
                        theLogStream.Args() );
}
