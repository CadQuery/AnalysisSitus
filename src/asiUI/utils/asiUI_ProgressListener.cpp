//-----------------------------------------------------------------------------
// Created on: 29 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ProgressListener.h>

// asiUI includes
#include <asiUI_ProgressNotifier.h>
#include <asiUI_StatusBar.h>
#include <asiUI_StatusBarImpl.h>

// Qt includes
#include <QCoreApplication>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param statusBar [in] status bar.
//! \param notifier  [in] progress notifier.
//! \param logger    [in] logger instance.
asiUI_ProgressListener::asiUI_ProgressListener(const Handle(asiUI_IStatusBar)&         statusBar,
                                               const Handle(ActAPI_IProgressNotifier)& notifier,
                                               const Handle(asiUI_Logger)&             logger)
: QObject     (),
  m_statusBar (statusBar),
  m_notifier  (notifier),
  m_logger    (logger)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ProgressListener::~asiUI_ProgressListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the sender object.
void asiUI_ProgressListener::Connect()
{
  // Get progress signaler
  Handle(asiUI_ProgressNotifier)
    notifierImpl = Handle(asiUI_ProgressNotifier)::DownCast(m_notifier);
  //
  QObject* pSignaler = notifierImpl->SignalNotifier();

  // Get status bar signaler
  Handle(asiUI_StatusBarImpl)
    statusImpl = Handle(asiUI_StatusBarImpl)::DownCast(m_statusBar);
  //
  asiUI_StatusBar* pStatusBar = statusImpl->GetStatusBar();

  connect( pSignaler, SIGNAL ( ProgressUpdated() ),
           this,      SLOT   ( onStep() ),
           Qt::AutoConnection );
  //
  connect( pSignaler, SIGNAL ( MessageUpdated() ),
           this,      SLOT   ( onMessage() ),
           Qt::AutoConnection );
  //
  connect( pSignaler, SIGNAL ( LogMessageSent() ),
           this,      SLOT   ( onLogMessage() ),
           Qt::AutoConnection );
  //
  connect( pSignaler, SIGNAL ( StatusChanged(const int) ),
           this,      SLOT   ( onStatus(const int) ),
           Qt::AutoConnection );
  //
  connect( pStatusBar, SIGNAL ( CancelClicked() ),
           this,       SLOT   ( onCancel() ) );
  //
  connect( pStatusBar, SIGNAL ( InfoClicked() ),
           this,       SLOT   ( onInfo() ) );
}

//-----------------------------------------------------------------------------

//! Progress updated.
void asiUI_ProgressListener::onStep()
{
  // Get status bar signaler
  Handle(asiUI_StatusBarImpl)
    statusImpl = Handle(asiUI_StatusBarImpl)::DownCast(m_statusBar);
  //
  asiUI_StatusBar* pStatusBar = statusImpl->GetStatusBar();

  if ( m_notifier->ProgressStatus() == Progress_Undefined )
    return;

  if ( !m_notifier->IsInfinite() )
  {
    const int Capacity = m_notifier->Capacity();
    if ( Capacity )
    {
      double Progress = (double) m_notifier->SummaryProgress() / Capacity;

      pStatusBar->UpdateProgress(Progress * 100);
    }
    else
      pStatusBar->UpdateProgress(0);
  }
  else
  {
    int Progress = m_notifier->SummaryProgress();

    // Enrich message with summary step
    TCollection_AsciiString Msg = m_notifier->MessageKey();
    Msg += TCollection_AsciiString(" (") + Progress + TCollection_AsciiString(")");

    // Set message
    m_statusBar->SetProgressText(Msg);
  }
  //
  QCoreApplication::processEvents();
}

//-----------------------------------------------------------------------------

//! Message changed.
void asiUI_ProgressListener::onMessage()
{
  m_statusBar->SetProgressText( m_notifier->MessageKey() );
  //
  QCoreApplication::processEvents();
}

//-----------------------------------------------------------------------------

//! Log message sent.
void asiUI_ProgressListener::onLogMessage()
{
  // Get progress signaler
  Handle(asiUI_ProgressNotifier)
    notifierImpl = Handle(asiUI_ProgressNotifier)::DownCast(m_notifier);

  ActAPI_LogMessageList listFromAlgo = notifierImpl->AlgoLogger()->PopMessageList();

  // Sort the collection of messages by their timestamps
  //std::sort( listFromAlgo.begin(), listFromAlgo.end() );

  // Show messages to the user
  m_logger->Dispatch(listFromAlgo);
}

//-----------------------------------------------------------------------------

//! Status changed.
//! \param status [in] new progress status.
void asiUI_ProgressListener::onStatus(const int status)
{
  // Get status bar signaler
  Handle(asiUI_StatusBarImpl)
    statusImpl = Handle(asiUI_StatusBarImpl)::DownCast(m_statusBar);
  //
  asiUI_StatusBar* pStatusBar = statusImpl->GetStatusBar();

  switch (status)
  {
    case Progress_Running:
    {
      pStatusBar->ShowProgress(true);
      pStatusBar->EnableCancelButton(true);
      pStatusBar->SetProgressInfinite( m_notifier->IsInfinite() );
    }
    break;

    case Progress_Cancelled:
    {
      pStatusBar->ShowProgress(false);
    }
    break;

    case Progress_Succeeded:
    case Progress_Failed:
    {
      pStatusBar->ShowProgress(false);
    }
    break;

    default:
      break;
  }
}

//-----------------------------------------------------------------------------

//! Cancellation requested.
void asiUI_ProgressListener::onCancel()
{
  // Get status bar signaler
  Handle(asiUI_StatusBarImpl)
    statusImpl = Handle(asiUI_StatusBarImpl)::DownCast(m_statusBar);
  //
  asiUI_StatusBar* pStatusBar = statusImpl->GetStatusBar();

  if ( m_notifier->ProgressStatus() == Progress_Running )
    m_notifier->Cancel();

  // disable stop button...
  pStatusBar->EnableCancelButton(false);
  //
  QCoreApplication::processEvents();
}

//-----------------------------------------------------------------------------

//! Information requested.
void asiUI_ProgressListener::onInfo()
{
  // TODO: NYI
}
