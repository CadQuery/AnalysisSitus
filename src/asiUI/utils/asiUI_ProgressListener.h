//-----------------------------------------------------------------------------
// Created on: 29 December 2016 (getting closer to NY)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_ProgressListener_h
#define asiUI_ProgressListener_h

// asiUI includes
#include <asiUI_IStatusBar.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#include <QObject>

//! Default slots for reaction on algorithmic events.
class asiUI_ProgressListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ProgressListener(const Handle(asiUI_IStatusBar)&         statusBar,
                           const Handle(ActAPI_IProgressNotifier)& notifier);

  asiUI_EXPORT virtual
    ~asiUI_ProgressListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected slots:

  asiUI_EXPORT void onStep       ();
  asiUI_EXPORT void onMessage    ();
  asiUI_EXPORT void onLogMessage ();
  asiUI_EXPORT void onStatus     (const int);
  asiUI_EXPORT void onCancel     ();
  asiUI_EXPORT void onInfo       ();

protected:

  Handle(asiUI_IStatusBar)         m_statusBar; //!< Status bar.
  Handle(ActAPI_IProgressNotifier) m_notifier;  //!< Progress notifier.

};

#endif
