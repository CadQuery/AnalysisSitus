//-----------------------------------------------------------------------------
// Created on: 30 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
