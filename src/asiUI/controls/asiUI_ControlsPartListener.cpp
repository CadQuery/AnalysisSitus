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
#include <vtkCellTreeLocator.h>
#include <vtkMapper.h>

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wControls [in] controls.
//! \param model     [in] Data Model instance.
//! \param cf        [in] commonf facilities.
//! \param notifier  [in] progress notifier.
asiUI_ControlsPartListener::asiUI_ControlsPartListener(asiUI_ControlsPart*                   wControls,
                                                       const Handle(asiEngine_Model)&        model,
                                                       const Handle(asiUI_CommonFacilities)& cf,
                                                       ActAPI_ProgressEntry                  notifier)
: QObject     (),
  m_wControls (wControls),
  m_model     (model),
  m_cf        (cf),
  m_notifier  (notifier)
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

  this->reinitializeEverything(true);
}

//-----------------------------------------------------------------------------

//! Reaction on part adding.
void asiUI_ControlsPartListener::onPartAdded(const QString&)
{
  this->reinitializeEverything(false);
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void asiUI_ControlsPartListener::onPartModified()
{
  m_cf->ViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, false);

  // Re-initialize pickers
  this->reinitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of vertices.
void asiUI_ControlsPartListener::onVerticesOn()
{
  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast( m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );
  //
  prs->VerticesOn();

  // Re-initialize pickers as the data set has changed
  m_cf->ViewerPart->PrsMgr()->InitializePickers( Handle(ActAPI_INode)() );
  m_cf->ViewerPart->Repaint();
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of vertices.
void asiUI_ControlsPartListener::onVerticesOff()
{
  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast( m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );
  //
  prs->VerticesOff();

  // Re-initialize pickers as the data set has changed
  m_cf->ViewerPart->PrsMgr()->InitializePickers( Handle(ActAPI_INode)() );
  m_cf->ViewerPart->Repaint();
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
    prs = Handle(asiVisu_PartPrs)::DownCast( m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

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
    prs = Handle(asiVisu_PartPrs)::DownCast(m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

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
    prs = Handle(asiVisu_PartPrs)::DownCast(m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable(1); // Vertices are visualized with contour actor
}

//-----------------------------------------------------------------------------

//! Performs full re-initialization of everything.
//! \param[in] fitAll indicates whether to fit the scene.
void asiUI_ControlsPartListener::reinitializeEverything(const bool fitAll)
{
  m_notifier.SetMessageKey("Actualize presentations");
  m_notifier.Init(1);

  // Set all necessary diagnostic tools
  ActAPI_DataObjectId partId = m_model->GetPartNode()->GetId();
  //
  if ( !m_cf->ViewerPart->PrsMgr()->IsPresented(partId) )
    m_cf->ViewerPart->PrsMgr()->SetPresentation( m_model->GetPartNode() );

  // Re-initialize pickers
  this->reinitializePickers();

  // Actualize
  m_cf->ViewerPart->PrsMgr()->Actualize(m_model->GetPartNode(), false, fitAll);

  // Repaint other viewers which may have been affected
  m_cf->ViewerHost->Repaint();
  m_cf->ViewerDomain->Repaint();

  // Repopulate object browser
  m_cf->ObjectBrowser->Populate();

  m_notifier.StepProgress(1);
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
  //
  m_cf->ViewerPart->PrsMgr()->SetPickFromList(true);
  m_cf->ViewerPart->PrsMgr()->SetPickList(pickableNodes);
}
