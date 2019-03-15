//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiUI_ControlsPartListener.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCellTreeLocator.h>
#include <vtkMapper.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param[in] wControls controls.
//! \param[in] cf        common facilities.
asiUI_ControlsPartListener::asiUI_ControlsPartListener(asiUI_ControlsPart*                   wControls,
                                                       const Handle(asiUI_CommonFacilities)& cf)
: QObject     (),
  m_wControls (wControls),
  m_cf        (cf)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsPartListener::~asiUI_ControlsPartListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the controls widget.
void asiUI_ControlsPartListener::Connect()
{
  connect( m_wControls, SIGNAL ( partLoaded(const QString&) ),
           this,        SLOT   ( onPartLoaded(const QString&) ) );
  //
  connect( m_wControls, SIGNAL ( partAdded(const QString&) ),
           this,        SLOT   ( onPartAdded(const QString&) ) );
  //
  connect( m_wControls, SIGNAL ( partModified() ),
           this,        SLOT   ( onPartModified() ) );
  //
  connect( m_wControls, SIGNAL ( selectionFacesOn() ),
           this,        SLOT   ( onSelectionFacesOn() ) );
  //
  connect( m_wControls, SIGNAL ( selectionEdgesOn() ),
           this,        SLOT   ( onSelectionEdgesOn() ) );
  //
  connect( m_wControls, SIGNAL ( selectionVerticesOn() ),
           this,        SLOT   ( onSelectionVerticesOn() ) );
}

//-----------------------------------------------------------------------------

//! Reaction on part loading.
//! \param[in] filename filename of the loaded part.
void asiUI_ControlsPartListener::onPartLoaded(const QString& filename)
{
  if ( m_cf->MainWindow )
    m_cf->MainWindow->setWindowTitle("Analysis of [" + filename + "]");

  this->reinitializeEverything(true, true);

  if ( m_cf->ObjectBrowser )
    m_cf->ObjectBrowser->SetSelectedNode( m_cf->Model->GetPartNode()->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on part adding.
void asiUI_ControlsPartListener::onPartAdded(const QString&)
{
  this->reinitializeEverything(false, true);

  if ( m_cf->ObjectBrowser )
    m_cf->ObjectBrowser->SetSelectedNode( m_cf->Model->GetPartNode()->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void asiUI_ControlsPartListener::onPartModified()
{
  m_cf->ViewerPart->PrsMgr()->Actualize(m_cf->Model->GetPartNode(), false, false);

  // Re-initialize pickers
  this->reinitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of faces.
void asiUI_ControlsPartListener::onSelectionFacesOn()
{
  // Enable the corresponding selection mode
  m_cf->ViewerPart->PrsMgr()->SetSelectionMode(SelectionMode_Face);

  // Clean tool viewers
  if ( m_cf->ViewerDomain )
  {
    m_cf->ViewerDomain->PrsMgr()->DeRenderAllPresentations();
    m_cf->ViewerDomain->Repaint();
  }
  //
  if ( m_cf->ViewerHost )
  {
    m_cf->ViewerHost->PrsMgr()->DeRenderAllPresentations();
    m_cf->ViewerHost->Repaint();
  }

  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast( m_cf->ViewerPart->PrsMgr()->GetPresentation( m_cf->Model->GetPartNode() ) );

  prs->MainActor()->SetPickable(1);
  prs->ContourActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of edges.
void asiUI_ControlsPartListener::onSelectionEdgesOn()
{
  // Enable the corresponding selection mode
  m_cf->ViewerPart->PrsMgr()->SetSelectionMode(SelectionMode_Edge);

  // Clean tool viewers
  if ( m_cf->ViewerDomain )
  {
    m_cf->ViewerDomain->PrsMgr()->DeleteAllPresentations();
    m_cf->ViewerDomain->Repaint();
  }
  //
  if ( m_cf->ViewerHost )
  {
    m_cf->ViewerHost->PrsMgr()->DeleteAllPresentations();
    m_cf->ViewerHost->Repaint();
  }

  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast(m_cf->ViewerPart->PrsMgr()->GetPresentation( m_cf->Model->GetPartNode() ) );

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable(1);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of vertices.
void asiUI_ControlsPartListener::onSelectionVerticesOn()
{
  // Enable the corresponding selection mode
  m_cf->ViewerPart->PrsMgr()->SetSelectionMode(SelectionMode_Vertex);

  // Clean tool viewers
  if ( m_cf->ViewerDomain )
  {
    m_cf->ViewerDomain->PrsMgr()->DeleteAllPresentations();
    m_cf->ViewerDomain->Repaint();
  }
  //
  if ( m_cf->ViewerHost )
  {
    m_cf->ViewerHost->PrsMgr()->DeleteAllPresentations();
    m_cf->ViewerHost->Repaint();
  }

  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast(m_cf->ViewerPart->PrsMgr()->GetPresentation( m_cf->Model->GetPartNode() ) );

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable(1); // Vertices are visualized with contour actor
}

//-----------------------------------------------------------------------------

//! Performs full re-initialization of everything.
//! \param[in] fitAll          indicates whether to fit the scene.
//! \param[in] populateBrowser indicates whether to repopulate browser.
void asiUI_ControlsPartListener::reinitializeEverything(const bool fitAll,
                                                        const bool populateBrowser)
{
  m_cf->Progress.SetMessageKey("Actualize presentations");
  m_cf->Progress.Init(1);

  // Set all necessary diagnostic tools
  ActAPI_DataObjectId partId = m_cf->Model->GetPartNode()->GetId();
  //
  if ( !m_cf->ViewerPart->PrsMgr()->IsPresented(partId) )
    m_cf->ViewerPart->PrsMgr()->SetPresentation( m_cf->Model->GetPartNode() );

  // Re-initialize pickers
  this->reinitializePickers();

  // Actualize
  m_cf->ViewerPart->PrsMgr()->GarbageCollect(); // Derender any dead objects
  m_cf->ViewerPart->PrsMgr()->Actualize(m_cf->Model->GetPartNode(), false, fitAll);

  // Repaint other viewers which may have been affected
  if ( m_cf->ViewerHost )
    m_cf->ViewerHost->Repaint();
  //
  if ( m_cf->ViewerDomain )
    m_cf->ViewerDomain->Repaint();

  // Repopulate object browser
  if ( m_cf->ObjectBrowser && populateBrowser )
    m_cf->ObjectBrowser->Populate();

  m_cf->Progress.StepProgress(1);
  m_cf->Progress.SetProgressStatus(Progress_Succeeded);
}

//-----------------------------------------------------------------------------

//! Reinitialize pickers.
void asiUI_ControlsPartListener::reinitializePickers()
{
  // Set Part Node as the only pickable object. This prevents UI from
  // any reaction on "useless" actors, e.g. normal fields, etc.
  Handle(ActAPI_HNodeList) pickableNodes = new ActAPI_HNodeList;
  pickableNodes->Append( m_cf->Model->GetPartNode() );
  //
  m_cf->ViewerPart->PrsMgr()->SetPickFromList(true);
  m_cf->ViewerPart->PrsMgr()->SetPickList(pickableNodes);
}
