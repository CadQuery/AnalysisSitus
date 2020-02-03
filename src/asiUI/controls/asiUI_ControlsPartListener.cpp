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

// asiAlgo includes
#include <asiAlgo_Timer.h>

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
//! \param[in] wControls     listened controls.
//! \param[in] model         Data Model instance.
//! \param[in] pPartViewer   part viewer.
//! \param[in] pHostViewer   host geometry viewer.
//! \param[in] pDomainViewer domain viewer.
//! \param[in] pBrowser      object browser.
//! \param[in] pMainWindow   main Qt window.
//! \param[in] notifier      progress notifier.
asiUI_ControlsPartListener::asiUI_ControlsPartListener(asiUI_ControlsPart*            wControls,
                                                       const Handle(asiEngine_Model)& model,
                                                       asiUI_ViewerPart*              pPartViewer,
                                                       asiUI_ViewerHost*              pHostViewer,
                                                       asiUI_ViewerDomain*            pDomainViewer,
                                                       asiUI_ObjectBrowser*           pBrowser,
                                                       QMainWindow*                   pMainWindow,
                                                       ActAPI_ProgressEntry           notifier)
: QObject        (),
  m_wControls    (wControls),
  m_model        (model),
  m_partViewer   (pPartViewer),
  m_hostViewer   (pHostViewer),
  m_domainViewer (pDomainViewer),
  m_browser      (pBrowser),
  m_mainWindow   (pMainWindow),
  m_progress     (notifier)
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
  if ( m_mainWindow )
    m_mainWindow->setWindowTitle("Analysis of [" + filename + "]");

  this->reinitializeEverything(true, true);

  if ( m_browser )
    m_browser->SetSelectedNode( m_model->GetPartNode()->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on part adding.
void asiUI_ControlsPartListener::onPartAdded(const QString&)
{
  this->reinitializeEverything(false, true);

  if ( m_browser )
    m_browser->SetSelectedNode( m_model->GetPartNode()->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on part modification.
void asiUI_ControlsPartListener::onPartModified()
{
  m_partViewer->PrsMgr()->Actualize(m_model->GetPartNode(), false, false);

  // Re-initialize pickers
  this->reinitializePickers();
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of faces.
void asiUI_ControlsPartListener::onSelectionFacesOn()
{
  // Enable the corresponding selection mode
  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Face);

  // Clean tool viewers
  if ( m_domainViewer )
  {
    m_domainViewer->PrsMgr()->DeRenderAllPresentations();
    m_domainViewer->Repaint();
  }
  //
  if ( m_hostViewer )
  {
    m_hostViewer->PrsMgr()->DeRenderAllPresentations();
    m_hostViewer->Repaint();
  }

  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

  prs->MainActor()->SetPickable(1);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of edges.
void asiUI_ControlsPartListener::onSelectionEdgesOn()
{
  // Enable the corresponding selection mode
  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Edge);

  // Clean tool viewers
  if ( m_domainViewer )
  {
    m_domainViewer->PrsMgr()->DeleteAllPresentations();
    m_domainViewer->Repaint();
  }
  //
  if ( m_hostViewer )
  {
    m_hostViewer->PrsMgr()->DeleteAllPresentations();
    m_hostViewer->Repaint();
  }

  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

  prs->MainActor()->SetPickable(1);
}

//-----------------------------------------------------------------------------

//! Reaction on enabling selection of vertices.
void asiUI_ControlsPartListener::onSelectionVerticesOn()
{
  // Enable the corresponding selection mode
  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Vertex);

  // Clean tool viewers
  if ( m_domainViewer )
  {
    m_domainViewer->PrsMgr()->DeleteAllPresentations();
    m_domainViewer->Repaint();
  }
  //
  if ( m_hostViewer )
  {
    m_hostViewer->PrsMgr()->DeleteAllPresentations();
    m_hostViewer->Repaint();
  }

  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast(m_partViewer->PrsMgr()->GetPresentation( m_model->GetPartNode() ) );

  prs->MainActor()->SetPickable(1);
}

//-----------------------------------------------------------------------------

//! Performs full re-initialization of everything.
//! \param[in] fitAll          indicates whether to fit the scene.
//! \param[in] populateBrowser indicates whether to repopulate browser.
void asiUI_ControlsPartListener::reinitializeEverything(const bool fitAll,
                                                        const bool populateBrowser)
{
  TIMER_NEW
  TIMER_GO

  m_progress.SetMessageKey("Actualize presentations");
  m_progress.Init(1);

  // Set all necessary diagnostic tools
  ActAPI_DataObjectId partId = m_model->GetPartNode()->GetId();
  //
  if ( !m_partViewer->PrsMgr()->IsPresented(partId) )
    m_partViewer->PrsMgr()->SetPresentation( m_model->GetPartNode() );

  // Re-initialize pickers
  this->reinitializePickers();

  // Actualize
  m_partViewer->PrsMgr()->GarbageCollect(); // Derender any dead objects
  m_partViewer->PrsMgr()->Actualize(m_model->GetPartNode(), false, fitAll);

  // Repaint other viewers which may have been affected
  if ( m_hostViewer )
    m_hostViewer->Repaint();
  //
  if ( m_domainViewer )
    m_domainViewer->Repaint();

  // Repopulate object browser
  if ( m_browser && populateBrowser )
    m_browser->Populate();

  m_progress.StepProgress(1);
  m_progress.SetProgressStatus(Progress_Succeeded);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Reinitialize UI & visualization")
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
  m_partViewer->PrsMgr()->SetPickFromList(true);
  m_partViewer->PrsMgr()->SetPickList(pickableNodes);
}
