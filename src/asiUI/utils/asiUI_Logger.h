//-----------------------------------------------------------------------------
// Created on: 30 December 2016
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

#ifndef asiUI_Logger_h
#define asiUI_Logger_h

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_Logger.h>

// Active Data includes
#include <ActAPI_ILogger.h>

// Qt includes
#include <QTextEdit>

//! Logging tool connecting algorithmic logger with GUI.
class asiUI_Logger : public ActAPI_ILogger
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_Logger, ActAPI_ILogger)

public:

  asiUI_EXPORT
    asiUI_Logger(QTextEdit* pLogWindow = NULL);

  asiUI_EXPORT virtual
    ~asiUI_Logger();

public:

  asiUI_EXPORT void
    SetLogWindow(QTextEdit* pLogWindow);

  asiUI_EXPORT QTextEdit*
    GetLogWindow() const;

// Logging kernel:
public:

  asiUI_EXPORT virtual void
    Info(const TCollection_AsciiString&    message,
         const ActAPI_LogMessagePriority   priority  = Priority_Normal,
         const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& timeStamp = NULL);

  asiUI_EXPORT virtual void
    Notice(const TCollection_AsciiString&    message,
           const ActAPI_LogMessagePriority   priority  = Priority_Normal,
           const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
           const Handle(Standard_Transient)& timeStamp = NULL);

  asiUI_EXPORT virtual void
    Warn(const TCollection_AsciiString&    message,
         const ActAPI_LogMessagePriority   priority  = Priority_Normal,
         const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& timeStamp = NULL);

  asiUI_EXPORT virtual void
    Error(const TCollection_AsciiString&    message,
          const ActAPI_LogMessagePriority   priority  = Priority_Normal,
          const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
          const Handle(Standard_Transient)& timeStamp = NULL);

// Custom logging interface:
public:

  asiUI_EXPORT virtual void
    Info(const QString&                    message,
         const ActAPI_LogMessagePriority   priority  = Priority_Normal,
         const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& timeStamp = NULL);

  asiUI_EXPORT virtual void
    Notice(const QString&                    message,
           const ActAPI_LogMessagePriority   priority  = Priority_Normal,
           const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
           const Handle(Standard_Transient)& timeStamp = NULL);

  asiUI_EXPORT virtual void
    Warn(const QString&                    message,
         const ActAPI_LogMessagePriority   priority  = Priority_Normal,
         const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& timeStamp = NULL);

  asiUI_EXPORT virtual void
    Error(const QString&                    message,
          const ActAPI_LogMessagePriority   priority  = Priority_Normal,
          const ActAPI_LogArguments&        arguments = ActAPI_LogArguments(),
          const Handle(Standard_Transient)& timeStamp = NULL);

// Internal methods:
private:

  QString chooseString(const Handle(Standard_Transient)& val) const;

  template<typename T>
  QString toString(const Handle(Standard_Transient)& val) const;

  QString toString(const Handle(ActAux_TimeStamp)& timeStamp) const;

  void logMessage(const TCollection_AsciiString&    message,
                  const ActAPI_LogMessageSeverity   severity,
                  const ActAPI_LogMessagePriority   priority,
                  const ActAPI_LogArguments&        arguments,
                  const Handle(Standard_Transient)& timeStamp) const;

  void logMessage(const QString&                    message,
                  const ActAPI_LogMessageSeverity   severity,
                  const ActAPI_LogMessagePriority   priority,
                  const ActAPI_LogArguments&        arguments,
                  const Handle(Standard_Transient)& timeStamp) const;

  void putText(const QString&, const QColor&) const;

private:

  //! Logging Window.
  QTextEdit* m_widget;

};

#endif
