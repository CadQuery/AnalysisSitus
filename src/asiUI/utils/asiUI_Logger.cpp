//-----------------------------------------------------------------------------
// Created on: 30 December 2016
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
#include <asiUI_Logger.h>

// Qt includes
#pragma warning(push, 0)
#include <QColor>
#include <QDateTime>
#include <QRgb>
#include <QScrollBar>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor accepting logging widget.
//! \param pLogWindow [in] logging window.
asiUI_Logger::asiUI_Logger(QTextEdit* pLogWindow)
: ActAPI_ILogger(), m_widget(pLogWindow)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_Logger::~asiUI_Logger()
{}

//-----------------------------------------------------------------------------

//! Sets logging window to operate with.
//! \param pLogWindow [in] logging window to set.
void asiUI_Logger::SetLogWindow(QTextEdit* pLogWindow)
{
  m_widget = pLogWindow;
}

//-----------------------------------------------------------------------------

//! Accessor for the logging window.
//! \return logging window.
QTextEdit* asiUI_Logger::GetLogWindow() const
{
  return m_widget;
}

//-----------------------------------------------------------------------------

//! Puts INFORMATION message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Info(const TCollection_AsciiString&    message,
                        const ActAPI_LogMessagePriority   priority,
                        const ActAPI_LogArguments&        arguments,
                        const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Information, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts NOTICE message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Notice(const TCollection_AsciiString&    message,
                          const ActAPI_LogMessagePriority   priority,
                          const ActAPI_LogArguments&        arguments,
                          const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Notice, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts WARNING message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Warn(const TCollection_AsciiString&    message,
                        const ActAPI_LogMessagePriority   priority,
                        const ActAPI_LogArguments&        arguments,
                        const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Warning, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts ERROR message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Error(const TCollection_AsciiString&     message,
                          const ActAPI_LogMessagePriority   priority,
                          const ActAPI_LogArguments&        arguments,
                          const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Error, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts INFORMATION message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Info(const QString&                    message,
                        const ActAPI_LogMessagePriority   priority,
                        const ActAPI_LogArguments&        arguments,
                        const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Information, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts NOTICE message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Notice(const QString&                    message,
                          const ActAPI_LogMessagePriority   priority,
                          const ActAPI_LogArguments&        arguments,
                          const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Notice, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts WARNING message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Warn(const QString&                    message,
                        const ActAPI_LogMessagePriority   priority,
                        const ActAPI_LogArguments&        arguments,
                        const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Warning, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Puts ERROR message to the Log Window.
//! \param message   [in] message to log.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::Error(const QString&                    message,
                         const ActAPI_LogMessagePriority   priority,
                         const ActAPI_LogArguments&        arguments,
                         const Handle(Standard_Transient)& timeStamp)
{
  this->logMessage(message, Severity_Error, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Attempts to build a string representation of the passed value presuming
//! that it has one of the acceptable primitive types.
//! \param val [in] value to convert to string representation.
//! \return string representation or EMPTY string.
QString asiUI_Logger::chooseString(const Handle(Standard_Transient)& val) const
{
  QString STR_Integer = this->toString<ActAPI_VariableInt>(val);
  if ( !STR_Integer.isEmpty() )
    return STR_Integer;

  QString STR_Real = this->toString<ActAPI_VariableReal>(val);
  if ( !STR_Real.isEmpty() )
    return STR_Real;

  QString STR_String = this->toString<ActAPI_VariableString>(val);
  if ( !STR_String.isEmpty() )
  {
    STR_String = QObject::tr( STR_String.toLatin1() );
    return STR_String;
  }

  return QString::null;
}

//-----------------------------------------------------------------------------

//! Converts the given value to string.
//! \param val [in] value to convert.
//! \return string representation of the passed value.
template<typename T>
QString asiUI_Logger::toString(const Handle(Standard_Transient)& val) const
{
  opencascade::handle<T> hVal = opencascade::handle<T>::DownCast(val);
  if ( hVal.IsNull() )
    return QString::null;

  TCollection_AsciiString str(hVal->Value);
  return AsciiStr2QStr(str);
}

//-----------------------------------------------------------------------------

//! Converts the passed timestamp data first to string representation
//! of QDateTime.
//! \param timeStamp [in] algorithmic timestamp to convert to readable form.
//! \return string representation of timestamp.
QString asiUI_Logger::toString(const Handle(ActAux_TimeStamp)& timeStamp) const
{
  if ( timeStamp.IsNull() )
    return QString::null;

  QDateTime dateTime = asiUI_Common::ToQDateTime(timeStamp);

  return QString("[") + dateTime.toString("hh:mm:ss") + QString("]");
}

//-----------------------------------------------------------------------------

//! Internal logging method.
//! \param message   [in] message to log.
//! \param severity  [in] message severity.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::logMessage(const TCollection_AsciiString&    message,
                              const ActAPI_LogMessageSeverity   severity,
                              const ActAPI_LogMessagePriority   priority,
                              const ActAPI_LogArguments&        arguments,
                              const Handle(Standard_Transient)& timeStamp) const
{
  if ( message.IsEmpty() )
    return;

  QString msg = QObject::tr( message.ToCString() );
  this->logMessage(msg, severity, priority, arguments, timeStamp);
}

//-----------------------------------------------------------------------------

//! Internal logging method.
//! \param message   [in] message to log.
//! \param severity  [in] message severity.
//! \param priority  [in] message priority.
//! \param arguments [in] message arguments.
//! \param timeStamp [in] application-specific timestamp.
void asiUI_Logger::logMessage(const QString&                    message,
                              const ActAPI_LogMessageSeverity   severity,
                              const ActAPI_LogMessagePriority   priority,
                              const ActAPI_LogArguments&        arguments,
                              const Handle(Standard_Transient)& timeStamp) const
{
  Q_UNUSED(priority)

  if ( message.isEmpty() )
    return;

  // Apply arguments
  QString msg = message;
  for ( ActAPI_LogArguments::Iterator it(arguments); it.More(); it.Next() )
  {
    const Handle(Standard_Transient)& TVal = it.Value();
    msg = msg.arg( this->chooseString(TVal) );
  }

  // Get timestamp
  Handle(ActAux_TimeStamp)
    TS = ( timeStamp.IsNull() ? ActAux_TimeStampTool::Generate()
                              : Handle(ActAux_TimeStamp)::DownCast(timeStamp) );

  // Generate severity-dependent prefix
  QString prefix;
  QColor hiliColor;
  if ( severity == Severity_Information )
  {
    prefix    = " [INFO] ";
    hiliColor = QColor(215, 215, 215);
  }
  else if ( severity == Severity_Notice )
  {
    prefix    = " [NOTICE] ";
    hiliColor = QColor(120, 200, 255);
  }
  else if ( severity == Severity_Warning )
  {
    prefix    = " [WARNING] ";
    hiliColor = QColor(250, 170, 70);
  }
  else if ( severity == Severity_Error )
  {
    prefix    = " [ERROR] ";
    hiliColor = QColor(255, 120, 120);
  }

  this->putText(this->toString(TS) + prefix + msg, hiliColor);
}

//-----------------------------------------------------------------------------

//! Puts text to the widget.
//! \param text  [in] string to put.
//! \param color [in] color to use.
void asiUI_Logger::putText(const QString& text, const QColor& color) const
{
  // Get color components.
  QRgb rgb = color.rgb();
  //
  const int red   = qRed(rgb);
  const int green = qGreen(rgb);
  const int blue  = qBlue(rgb);

  // Prepare string to add.
  QString line;
  line += QString("<div style='color:rgb(%1,%2,%3);font-family:Courier New;'>%4</div>")
         .arg(red)
         .arg(green)
         .arg(blue)
         .arg(text);

  // Append line to editor.
  m_widget->append(line);

  // Autoscroll down.
  QScrollBar* pSB = m_widget->verticalScrollBar();
  pSB->setValue( pSB->maximum() );
}
