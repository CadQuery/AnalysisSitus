//-----------------------------------------------------------------------------
// Created on: 31 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Logger_h
#define asiAlgo_Logger_h

// Analysis Situs includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_ILogger.h>

// TBB includes
#include <concurrent_queue.h>

DEFINE_STANDARD_HANDLE(asiAlgo_Logger, ActAPI_ILogger)

//! Logger for algorithmic messages.
class asiAlgo_Logger : public ActAPI_ILogger
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Logger, ActAPI_ILogger)

// Methods to use in single-threaded context:
public:

  asiAlgo_EXPORT
    asiAlgo_Logger();

  asiAlgo_EXPORT ActAPI_LogMessageList
    PopMessageList();

  asiAlgo_EXPORT void
    Clear();

  asiAlgo_EXPORT unsigned int
    HasErrors();

// THREAD-SAFE logging kernel methods:
public:

  asiAlgo_EXPORT virtual void
    Info(const TCollection_AsciiString&    theMessage,
         const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
         const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& theTimeStamp = NULL);

  asiAlgo_EXPORT virtual void
    Notice(const TCollection_AsciiString&    theMessage,
           const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
           const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
           const Handle(Standard_Transient)& theTimeStamp = NULL);

  asiAlgo_EXPORT virtual void
    Warn(const TCollection_AsciiString&    theMessage,
         const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
         const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& theTimeStamp = NULL);

  asiAlgo_EXPORT virtual void
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
