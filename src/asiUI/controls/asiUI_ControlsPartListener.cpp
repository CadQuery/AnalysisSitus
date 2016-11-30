//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ControlsPartListener.h>

// asiVisu includes
#include <asiVisu_GeomPrs.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wControls      [in] controls.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] host geometry viewer.
//! \param model          [in] Data Model instance.
asiUI_ControlsPartListener::asiUI_ControlsPartListener(asiUI_ControlsPart*            wControls,
                                                       asiUI_ViewerPart*              wViewerPart,
                                                       asiUI_ViewerDomain*            wViewerDomain,
                                                       asiUI_ViewerSurface*           wViewerSurface,
                                                       const Handle(asiEngine_Model)& model)
: QObject          (),
  m_wControls      (wControls),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model)
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
  connect( m_wControls, SIGNAL ( robustOn() ),
           this,        SLOT   ( onRobustOn() ) );
  //
  connect( m_wControls, SIGNAL ( robustOff() ),
           this,        SLOT   ( onRobustOff() ) );
  //
  connect( m_wControls, SIGNAL ( verticesOn() ),
           this,        SLOT   ( onVerticesOn() ) );
  //
  connect( m_wControls, SIGNAL ( verticesOff() ),
           this,        SLOT   ( onVerticesOff() ) );
  //
  connect( m_wControls, SIGNAL ( normalsOn() ),
           this,        SLOT   ( onNormalsOn() ) );
  //
  connect( m_wControls, SIGNAL ( normalsOff() ),
           this,        SLOT   ( onNormalsOff() ) );
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
  m_wViewerPart->PrsMgr()->InitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of robust presentation.
void asiUI_ControlsPartListener::onRobustOn()
{
  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( m_wViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );
  //
  prs->DoRobust(true);
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of robust presentation.
void asiUI_ControlsPartListener::onRobustOff()
{
  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( m_wViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );
  //
  prs->DoRobust(false);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of vertices.
void asiUI_ControlsPartListener::onVerticesOn()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of vertices.
void asiUI_ControlsPartListener::onVerticesOff()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of normals.
void asiUI_ControlsPartListener::onNormalsOn()
{
  // NYI
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of normals.
void asiUI_ControlsPartListener::onNormalsOff()
{
  // NYI
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
  this->cleanViewers();
  //
  m_wViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, true);
  m_wViewerPart->PrsMgr()->InitializePickers();
}
