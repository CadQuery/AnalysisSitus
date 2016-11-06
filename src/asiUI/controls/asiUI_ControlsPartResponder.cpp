//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ControlsPartResponder.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] host geometry viewer.
//! \param model          [in] Data Model instance.
asiUI_ControlsPartResponder::asiUI_ControlsPartResponder(asiUI_ViewerPart*              wViewerPart,
                                                         asiUI_ViewerDomain*            wViewerDomain,
                                                         asiUI_ViewerSurface*           wViewerSurface,
                                                         const Handle(asiEngine_Model)& model)
: QObject          (),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsPartResponder::~asiUI_ControlsPartResponder()
{}

//-----------------------------------------------------------------------------

//! Connects this responder to the controls widget.
//! \param wControls [in] widget to listen.
void asiUI_ControlsPartResponder::Connect(asiUI_ControlsPart* wControls)
{
  connect( wControls, SIGNAL ( partLoaded() ),
           this,      SLOT   ( onPartLoaded() ) );
  //
  connect( wControls, SIGNAL ( partModified() ),
           this,      SLOT   ( onPartModified() ) );
  //
  connect( wControls, SIGNAL ( verticesOn() ),
           this,      SLOT   ( onVerticesOn() ) );
  //
  connect( wControls, SIGNAL ( verticesOff() ),
           this,      SLOT   ( onVerticesOff() ) );
  //
  connect( wControls, SIGNAL ( normalsOn() ),
           this,      SLOT   ( onNormalsOn() ) );
  //
  connect( wControls, SIGNAL ( normalsOff() ),
           this,      SLOT   ( onNormalsOff() ) );
  //
  connect( wControls, SIGNAL ( selectionFacesOn() ),
           this,      SLOT   ( onSelectionFacesOn() ) );
  //
  connect( wControls, SIGNAL ( selectionEdgesOn() ),
           this,      SLOT   ( onSelectionEdgesOn() ) );
}

//-----------------------------------------------------------------------------

//! Reaction on part loading.
void asiUI_ControlsPartResponder::onPartLoaded()
{
  this->reinitializeEverything();
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void asiUI_ControlsPartResponder::onPartModified()
{
  this->cleanViewers();
  //
  m_wViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, false);
  m_wViewerPart->PrsMgr()->InitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of vertices.
void asiUI_ControlsPartResponder::onVerticesOn()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of vertices.
void asiUI_ControlsPartResponder::onVerticesOff()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of normals.
void asiUI_ControlsPartResponder::onNormalsOn()
{
  // NYI
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of normals.
void asiUI_ControlsPartResponder::onNormalsOff()
{
  // NYI
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of faces.
void asiUI_ControlsPartResponder::onSelectionFacesOn()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of edges.
void asiUI_ControlsPartResponder::onSelectionEdgesOn()
{
  this->onPartModified(); // TODO: this is completely weird
}

//-----------------------------------------------------------------------------

//! Cleans up the managed viewers.
void asiUI_ControlsPartResponder::cleanViewers()
{
  m_wViewerPart    ->PrsMgr()->DeleteAllPresentations();
  m_wViewerDomain  ->PrsMgr()->DeleteAllPresentations();
  m_wViewerSurface ->PrsMgr()->DeleteAllPresentations();
}

//-----------------------------------------------------------------------------

//! Performs full re-initialization of everything.
void asiUI_ControlsPartResponder::reinitializeEverything()
{
  this->cleanViewers();
  //
  m_wViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, true);
  m_wViewerPart->PrsMgr()->InitializePickers();
}
