//-----------------------------------------------------------------------------
// Created on: 30 December 2016
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
