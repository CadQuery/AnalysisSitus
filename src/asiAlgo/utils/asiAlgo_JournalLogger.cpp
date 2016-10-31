//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_JournalLogger.h>

// Active Data includes
#include <ActAux_TimeStamp.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

//-----------------------------------------------------------------------------
// THREAD-UNSAFE methods
//-----------------------------------------------------------------------------

//! Default constructor.
asiAlgo_JournalLogger::asiAlgo_JournalLogger() : ActAPI_ILogger()
{}

//! Returns a copy of the list of messages in OCCT form and cleans up the
//! internal thread-safe TBB collection.
//! \return list of logging messages.
ActAPI_LogMessageList asiAlgo_JournalLogger::PopMessageList()
{
  ActAPI_LogMessageList aResultList;
  ActAPI_LogMessage aMsg;
  while ( m_messageQueue.unsafe_size() > 0 )
  {
    m_messageQueue.try_pop(aMsg);
    aResultList.Append(aMsg);
  }
  return aResultList;
}

//! Checks whether the logger contains any error messages.
//! \return true/false.
unsigned int asiAlgo_JournalLogger::HasErrors()
{
  ActAPI_LogMessageList aResultList = this->PopMessageList();
  for ( ActAPI_LogMessageList::Iterator it(aResultList); it.More(); it.Next() )
  {
    if ( it.Value().Severity == Severity_Error )
      return 1;
  }

  return 0;
}

//! Cleans up the internal collection of messages.
void asiAlgo_JournalLogger::Clear()
{
  m_messageQueue.clear();
}

//-----------------------------------------------------------------------------
// THREAD-SAFE logging kernel methods
//-----------------------------------------------------------------------------

//! Appends a logging message with INFORMATION severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_JournalLogger::Info(const TCollection_AsciiString&    theMessage,
                                 const ActAPI_LogMessagePriority   thePriority,
                                 const ActAPI_LogArguments&        theArguments,
                                 const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Information,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with NOTICE severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_JournalLogger::Notice(const TCollection_AsciiString&    theMessage,
                                   const ActAPI_LogMessagePriority   thePriority,
                                   const ActAPI_LogArguments&        theArguments,
                                   const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Notice,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with WARNING severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_JournalLogger::Warn(const TCollection_AsciiString&    theMessage,
                                 const ActAPI_LogMessagePriority   thePriority,
                                 const ActAPI_LogArguments&        theArguments,
                                 const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Warning,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with ERROR severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_JournalLogger::Error(const TCollection_AsciiString&    theMessage,
                                  const ActAPI_LogMessagePriority   thePriority,
                                  const ActAPI_LogArguments&        theArguments,
                                  const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Error,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with the passed severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param theSeverity  [in] severity of the message.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp.
void asiAlgo_JournalLogger::appendMessage(const TCollection_AsciiString&    theMessage,
                                          const ActAPI_LogMessageSeverity   /*theSeverity*/,
                                          const ActAPI_LogMessagePriority   /*thePriority*/,
                                          const ActAPI_LogArguments&        /*theArguments*/,
                                          const Handle(Standard_Transient)& /*theTimeStamp*/)
{
  std::cout << "\tLOGGER: " << theMessage.ToCString() << std::endl;
  //m_messageQueue.push( ActAPI_LogMessage(thePriority,
  //                                       theSeverity,
  //                                       theMessage,
  //                                       theArguments,
  //                                       theTimeStamp) );
}
