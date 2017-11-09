//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
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
                                                       asiUI_ViewerHost*              wViewerSurface,
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
  connect( m_wControls, SIGNAL ( partAdded() ),
           this,        SLOT   ( onPartAdded() ) );
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

//! Reaction on part adding.
void asiUI_ControlsPartListener::onPartAdded()
{
  this->reinitializeEverything();
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void asiUI_ControlsPartListener::onPartModified()
{
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
  m_wViewerPart->PrsMgr()->InitializePickers( Handle(ActAPI_INode)() );
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
  m_wViewerPart->PrsMgr()->InitializePickers( Handle(ActAPI_INode)() );
  m_wViewerPart->Repaint();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of faces.
void asiUI_ControlsPartListener::onSelectionFacesOn()
{
  // Enable the corresponding selection mode
  m_wViewerPart->PrsMgr()->SetSelectionMode(SelectionMode_Face);

  // Clean tool viewers
  if ( m_wViewerDomain )
  {
    m_wViewerDomain->PrsMgr()->DeRenderAllPresentations();
    m_wViewerDomain->Repaint();
  }
  //
  if ( m_wViewerSurface )
  {
    m_wViewerSurface->PrsMgr()->DeRenderAllPresentations();
    m_wViewerSurface->Repaint();
  }

  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( m_wViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

  prs->MainActor()->SetPickable(1);
  prs->ContourActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of edges.
void asiUI_ControlsPartListener::onSelectionEdgesOn()
{
  // Enable the corresponding selection mode
  m_wViewerPart->PrsMgr()->SetSelectionMode(SelectionMode_Edge);

  // Clean tool viewers
  if ( m_wViewerDomain )
  {
    m_wViewerDomain->PrsMgr()->DeleteAllPresentations();
    m_wViewerDomain->Repaint();
  }
  //
  if ( m_wViewerSurface )
  {
    m_wViewerSurface->PrsMgr()->DeleteAllPresentations();
    m_wViewerSurface->Repaint();
  }

  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( m_wViewerPart->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable(1);
}

//-----------------------------------------------------------------------------

//! Cleans up the managed viewers.
void asiUI_ControlsPartListener::cleanViewers()
{
  // Erase part with all related data from Part Viewer...
  if ( m_wViewerPart )
    m_wViewerPart->PrsMgr()->DeletePresentation( m_model->GetPartNode(), true );

  // ... Domain Viewer...
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->DeletePresentation( m_model->GetPartNode(), true );

  // ... and Host Viewer.
  if ( m_wViewerSurface )
    m_wViewerSurface->PrsMgr()->DeletePresentation( m_model->GetPartNode(), true );
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

  // Repaint other viewers which may have been affected
  m_wViewerSurface->Repaint();
  m_wViewerDomain->Repaint();

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
  m_wViewerPart->PrsMgr()->SetPickFromList(true);
  m_wViewerPart->PrsMgr()->SetPickList(pickableNodes);
}
