//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef journal_logger_h
#define journal_logger_h

// Analysis Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActAPI_ILogger.h>

// TBB includes
#include <concurrent_queue.h>

DEFINE_STANDARD_HANDLE(journal_logger, ActAPI_ILogger)

//! Logger for algorithmic messages.
class journal_logger : public ActAPI_ILogger
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(journal_logger, ActAPI_ILogger)

// Methods to use in single-threaded context:
public:

  ASitus_EXPORT
    journal_logger();

  ASitus_EXPORT ActAPI_LogMessageList
    PopMessageList();

  ASitus_EXPORT void
    Clear();

  ASitus_EXPORT unsigned int
    HasErrors();

// THREAD-SAFE logging kernel methods:
public:

  ASitus_EXPORT virtual void
    Info(const TCollection_AsciiString&    theMessage,
         const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
         const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& theTimeStamp = NULL);

  ASitus_EXPORT virtual void
    Notice(const TCollection_AsciiString&    theMessage,
           const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
           const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
           const Handle(Standard_Transient)& theTimeStamp = NULL);

  ASitus_EXPORT virtual void
    Warn(const TCollection_AsciiString&    theMessage,
         const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
         const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& theTimeStamp = NULL);

  ASitus_EXPORT virtual void
    Error(const TCollection_AsciiString&    theMessage,
          const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
          const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
          const Handle(Standard_Transient)& theTimeStamp = NULL);

// Internal methods:
protected:

  virtual void appendMessage(const TCollection_AsciiString&    theMessage,
                             const ActAPI_LogMessageSeverity   theSeverity,
                             const ActAPI_LogMessagePriority   thePriority,
                             const ActAPI_LogArguments&        theArguments,
                             const Handle(Standard_Transient)& theTimeStamp);

private:

  //! Short-cut for queue of messages in TBB thread-safe form.
  typedef tbb::concurrent_queue<ActAPI_LogMessage> _MessageQueue;

private:

  _MessageQueue m_messageQueue; //!< Storage for messages.

};

#endif
