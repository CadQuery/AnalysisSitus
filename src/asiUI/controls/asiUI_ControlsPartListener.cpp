//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ControlsPartListener.h>

// asiVisu includes
#include <asiVisu_GeomPrs.h>

// VTK includes
#include <vtkCellTreeLocator.h>
#include <vtkMapper.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wControls      [in] controls.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] host geometry viewer.
//! \param model          [in] Data Model instance.
//! \param notifier       [in] progress notifier.
asiUI_ControlsPartListener::asiUI_ControlsPartListener(asiUI_ControlsPart*            wControls,
                                                       asiUI_ViewerPart*              wViewerPart,
                                                       asiUI_ViewerDomain*            wViewerDomain,
                                                       asiUI_ViewerSurface*           wViewerSurface,
                                                       const Handle(asiEngine_Model)& model,
                                                       ActAPI_ProgressEntry           notifier)
: QObject          (),
  m_wControls      (wControls),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model),
  m_notifier       (notifier)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsPartListener::~asiUI_ControlsPartListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the controls widget.
void asiUI_ControlsPartListener::Connect()
{
  connect( m_wControls, SIGNAL ( partLoaded() ),
           this,        SLOT   ( onPartLoaded() ) );
  //
  connect( m_wControls, SIGNAL ( partModified() ),
           this,        SLOT   ( onPartModified() ) );
  //
  connect( m_wControls, SIGNAL ( verticesOn() ),
           this,        SLOT   ( onVerticesOn() ) );
  //
  connect( m_wControls, SIGNAL ( verticesOff() ),
           this,        SLOT   ( onVerticesOff() ) );
  //
  connect( m_wControls, SIGNAL ( selectionFacesOn() ),
           this,        SLOT   ( onSelectionFacesOn() ) );
  //
  connect( m_wControls, SIGNAL ( selectionEdgesOn() ),
           this,        SLOT   ( onSelectionEdgesOn() ) );
}

//-----------------------------------------------------------------------------

//! Reaction on part loading.
void asiUI_ControlsPartListener::onPartLoaded()
{
  this->reinitializeEverything();
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void asiUI_ControlsPartListener::onPartModified()
{
  this->cleanViewers();
  //
  m_wViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, false);

  // Re-initialize pickers
  this->reinitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of vertices.
void asiUI_ControlsPartListener::onVerticesOn()
{
  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( m_wViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );
  //
  prs->VerticesOn();

  // Re-initialize pickers as the data set has changed
  m_wViewerPart->PrsMgr()->InitializePickers();
  m_wViewerPart->Repaint();
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of vertices.
void asiUI_ControlsPartListener::onVerticesOff()
{
  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( m_wViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );
  //
  prs->VerticesOff();

  // Re-initialize pickers as the data set has changed
  m_wViewerPart->PrsMgr()->InitializePickers();
  m_wViewerPart->Repaint();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of faces.
void asiUI_ControlsPartListener::onSelectionFacesOn()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of edges.
void asiUI_ControlsPartListener::onSelectionEdgesOn()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Cleans up the managed viewers.
void asiUI_ControlsPartListener::cleanViewers()
{
  if ( m_wViewerPart )
    m_wViewerPart->PrsMgr()->DeleteAllPresentations();

  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->DeleteAllPresentations();

  if ( m_wViewerSurface )
    m_wViewerSurface->PrsMgr()->DeleteAllPresentations();
}

//-----------------------------------------------------------------------------

//! Performs full re-initialization of everything.
void asiUI_ControlsPartListener::reinitializeEverything()
{
  m_notifier.SetMessageKey("Actualize presentations");
  m_notifier.Init(1);

  this->cleanViewers();

  // Set all necessary diagnostic tools
  ActAPI_DataObjectId partId = m_model->GetPartNode()->GetId();
  //
  if ( !m_wViewerPart->PrsMgr()->IsPresented(partId) )
    m_wViewerPart->PrsMgr()->SetPresentation( m_model->GetPartNode() );
  //
  m_wViewerPart->PrsMgr()->SetDiagnosticTools(m_notifier, NULL);

  // Re-initialize pickers
  this->reinitializePickers();

  // Actualize
  m_wViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, true);

  m_notifier.StepProgress(1, 1);
  m_notifier.SetProgressStatus(Progress_Succeeded);
}

//-----------------------------------------------------------------------------

//! Reinitialize pickers.
void asiUI_ControlsPartListener::reinitializePickers()
{
  // Set Part Node as the only pickable object. This prevents UI from
  // any reaction on "useless" actors, e.g. normal fields, etc.
  Handle(ActAPI_HNodeList) pickableNodes = new ActAPI_HNodeList;
  pickableNodes->Append( m_model->GetPartNode() );
  m_wViewerPart->PrsMgr()->SetPickFromList(true);
  m_wViewerPart->PrsMgr()->SetPickList(pickableNodes);
}
