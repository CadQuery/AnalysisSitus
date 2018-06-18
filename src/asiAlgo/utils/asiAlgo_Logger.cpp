//-----------------------------------------------------------------------------
// Created on: 31 March 2016
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
#include <asiAlgo_Logger.h>

// Active Data includes
#include <ActAux_TimeStamp.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

//-----------------------------------------------------------------------------
// THREAD-UNSAFE methods
//-----------------------------------------------------------------------------

//! Default constructor.
asiAlgo_Logger::asiAlgo_Logger() : ActAPI_ILogger()
{}

//! Returns a copy of the list of messages in OCCT form and cleans up the
//! internal thread-safe TBB collection.
//! \return list of logging messages.
ActAPI_LogMessageList asiAlgo_Logger::PopMessageList()
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
unsigned int asiAlgo_Logger::HasErrors()
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
void asiAlgo_Logger::Clear()
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
void asiAlgo_Logger::Info(const TCollection_AsciiString&    theMessage,
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
void asiAlgo_Logger::Notice(const TCollection_AsciiString&    theMessage,
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
void asiAlgo_Logger::Warn(const TCollection_AsciiString&    theMessage,
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
void asiAlgo_Logger::Error(const TCollection_AsciiString&    theMessage,
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
void asiAlgo_Logger::appendMessage(const TCollection_AsciiString&    theMessage,
                                   const ActAPI_LogMessageSeverity   theSeverity,
                                   const ActAPI_LogMessagePriority   thePriority,
                                   const ActAPI_LogArguments&        theArguments,
                                   const Handle(Standard_Transient)& theTimeStamp)
{
  std::cout << "\tLOGGER: " << theMessage.ToCString() << std::endl;
  m_messageQueue.push( ActAPI_LogMessage(thePriority,
                                         theSeverity,
                                         theMessage,
                                         theArguments,
                                         theTimeStamp) );
}
