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

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param statusBar [in] status bar.
//! \param notifier  [in] progress notifier.
asiUI_ProgressListener::asiUI_ProgressListener(const Handle(asiUI_IStatusBar)&         statusBar,
                                               const Handle(ActAPI_IProgressNotifier)& notifier)
: QObject     (),
  m_statusBar (statusBar),
  m_notifier  (notifier)
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
  //if ( m_ProgressNotifier->ProgressStatus() == Progress_Undefined )
  //  return;

  //if ( !m_ProgressNotifier->IsInfinite() )
  //{
  //  const Standard_Integer Capacity = m_ProgressNotifier->Capacity();
  //  if ( Capacity )
  //  {
  //    Standard_Real Progress = 
  //      (Standard_Real) m_ProgressNotifier->SummaryProgress() / Capacity;

  //    m_IStatusBar->UpdateProgress(Progress * 100);
  //  }
  //  else
  //    m_IStatusBar->UpdateProgress(0);
  //}
  //else
  //{
  //  Standard_Integer Progress = m_ProgressNotifier->SummaryProgress();

  //  // Enrich message with summary step
  //  TCollection_AsciiString
  //    Msg = QStr2AsciiStr( tr( m_ProgressNotifier->MessageKey().ToCString() ) );
  //  Msg += TCollection_AsciiString(" (") + Progress + TCollection_AsciiString(")");

  //  // Set message
  //  m_IStatusBar->SetProgressText(Msg);
  //}
}

//-----------------------------------------------------------------------------

//! Message changed.
void asiUI_ProgressListener::onMessage()
{
  /*TCollection_AsciiString
    Msg = QStr2AsciiStr( tr( m_ProgressNotifier->MessageKey().ToCString() ) );

  m_IStatusBar->SetProgressText(Msg);*/
}

//-----------------------------------------------------------------------------

//! Log message sent.
void asiUI_ProgressListener::onLogMessage()
{
  //ActAPI_LogMessageList aListFromAlgo = m_ProgressNotifier->AlgoLogger()->PopMessageList();

  //// Sort the collection of messages by their timestamps
  //NCollection_QuickSort<ActAPI_LogMessageList, ActAPI_LogMessage>
  //  ::Perform( aListFromAlgo, bbmAlgo_MsgTimeComparator(), 1, aListFromAlgo.Length() );

  //// Show messages to the user
  //this->GetLogger()->Dispatch(aListFromAlgo);
}

//-----------------------------------------------------------------------------

//! Status changed.
void asiUI_ProgressListener::onStatus(const int)
{
  //switch ( theStatus )
  //{
  //  case Progress_Running :
  //  {
  //    Handle(bbmAPI_IBrowser)     aBrowser     = m_IDesktop->GetObjectBrowser();
  //    Handle(bbmAPI_IParamEditor) aParamEditor = m_IDesktop->GetParamEditor();
  //    if ( !aBrowser.IsNull() )
  //    {
  //      aBrowser->SetEnabled(Standard_False);
  //    }
  //    if ( !aParamEditor.IsNull() )
  //    {
  //      aParamEditor->SetReadOnly(Standard_True);
  //    }

  //    m_IStatusBar->GetStatusBar()->EnableCancelButton(true);
  //    m_IStatusBar->GetStatusBar()->SetProgressInfinite( IsTrue( m_ProgressNotifier->IsInfinite() ) );

  //    bbmGui_InputEventFilter::Install(this);
  //  }
  //  break;

  //  case Progress_Cancelled :
  //  {
  //    bbmGui_InputEventFilter::Remove(this);

  //    Handle(bbmAPI_IBrowser)     aBrowser     = m_IDesktop->GetObjectBrowser();
  //    Handle(bbmAPI_IParamEditor) aParamEditor = m_IDesktop->GetParamEditor();
  //    if ( !aBrowser.IsNull() )
  //    {
  //      aBrowser->SetEnabled(Standard_True);
  //    }
  //    if ( !aParamEditor.IsNull() )
  //    {
  //      aParamEditor->SetReadOnly(Standard_False);
  //    }

  //    Handle(bbmAPI_IOperation) aCurrentOp = m_IApplication->CurrentOperation();
  //    if ( !aCurrentOp.IsNull() )
  //      aCurrentOp->JobCancelled();

  //    m_IStatusBar->ShowProgress(Standard_False);
  //  }
  //  break;

  //  case Progress_Succeeded :
  //  case Progress_Failed :
  //  {
  //    QCoreApplication::processEvents();

  //    bbmGui_InputEventFilter::Remove(this);

  //    Handle(bbmAPI_IOperation) aCurrentOp = m_IApplication->CurrentOperation();
  //    if ( !aCurrentOp.IsNull() )
  //    {
  //      bbmGui_OverrideCursor aCursorTool; // Cursor -> waiting...

  //      //-----------------------------------------------------------
  //      // Current Operation is alive, so let us invoke its callback
  //      //-----------------------------------------------------------

  //      // Get groundwork Data Model services
  //      Handle(ActData_BaseModel)
  //        MBase = Handle(ActData_BaseModel)::DownCast( m_IApplication->GetModel() );

  //      // Access TxData from Execution Context
  //      ActAPI_TxData TxData = MBase->FuncExecutionCtx()->AccessTxData();

  //      // User might specify custom callback to invoke
  //      bool isUserCallback = false;
  //      if ( !TxData.IsEmpty() )
  //      {
  //        // Access callback name (if any)
  //        TCollection_AsciiString name, cb;
  //        Standard_Integer opID;
  //        TxData >> name >> opID >> cb;

  //        // Attempt to run callback
  //        if ( !cb.IsEmpty() )
  //        {
  //          if ( aCurrentOp->IsKind( STANDARD_TYPE(bbmGui_BaseOp) ) )
  //          {
  //            // Access QObject of implementation
  //            bbmGui_OperationImpl* OpImpl = Handle(bbmGui_BaseOp)::DownCast(aCurrentOp)->GetImplementation();

  //            // Invoke method
  //            isUserCallback = QMetaObject::invokeMethod( OpImpl, cb.ToCString(), Q_ARG(ActAPI_TxData*, &TxData) );
  //          }
  //        }
  //      }

  //      if ( !isUserCallback )
  //        aCurrentOp->JobCompleted();
  //    }
  //    else
  //    {
  //      // Commit transaction here as there is no Operation which can do that
  //      m_IApplication->GetModel()->CommitCommand( ActAPI_TxData() << "APP_AUTO_COMMIT" << -1 );
  //      (LogInfo(Normal) << "APP_AUTO_COMMIT") >> m_IApplication->GetLogger();

  //      m_IDesktop->GetObjectBrowser()->Update();
  //      m_IDesktop->UpdateUI();
  //      this->updatePresentations();
  //    }

  //    Handle(bbmAPI_IBrowser)     aBrowser     = m_IDesktop->GetObjectBrowser();
  //    Handle(bbmAPI_IParamEditor) aParamEditor = m_IDesktop->GetParamEditor();
  //    if ( !aBrowser.IsNull() )
  //    {
  //      aBrowser->SetEnabled(Standard_True);
  //    }
  //    if ( !aParamEditor.IsNull() )
  //    {
  //      aParamEditor->SetReadOnly(Standard_False);
  //    }

  //    m_IStatusBar->ShowProgress(Standard_False);
  //  }
  //  break;

  //  default:
  //    break;
  //}
}

//-----------------------------------------------------------------------------

//! Cancellation requested.
void asiUI_ProgressListener::onCancel()
{
  //if ( m_ProgressNotifier->ProgressStatus() == Progress_Running )
  //  m_ProgressNotifier->Cancel();

  //// disable stop button...
  //m_IStatusBar->GetStatusBar()->EnableCancelButton(false);
}

//-----------------------------------------------------------------------------

//! Information requested.
void asiUI_ProgressListener::onInfo()
{
  // TODO: NYI
}
