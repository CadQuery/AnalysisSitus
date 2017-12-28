//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiUI_ViewerPartListener.h>

// asiUI includes
#include <asiUI_BgColorDialog.h>
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_InvertFaces.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_PartNodeInfo.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// VTK includes
#include <vtkCamera.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] surface viewer.
//! \param model          [in] Data Model instance.
//! \param progress       [in] progress notifier.
//! \param plotter        [in] imperative plotter.
asiUI_ViewerPartListener::asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                                                   asiUI_ViewerDomain*            wViewerDomain,
                                                   asiUI_ViewerHost*           wViewerSurface,
                                                   const Handle(asiEngine_Model)& model,
                                                   ActAPI_ProgressEntry           progress,
                                                   ActAPI_PlotterEntry            plotter)
: QObject          (),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model),
  m_progress       (progress),
  m_plotter        (plotter)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerPartListener::~asiUI_ViewerPartListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_ViewerPartListener::Connect()
{
  connect( m_wViewerPart, SIGNAL ( facePicked(const asiVisu_PickResult&) ),
           this,          SLOT   ( onFacePicked(const asiVisu_PickResult&) ) );
  //
  connect( m_wViewerPart, SIGNAL ( edgePicked(const asiVisu_PickResult&) ),
           this,          SLOT   ( onEdgePicked(const asiVisu_PickResult&) ) );
  //
  connect( m_wViewerPart, SIGNAL ( contextMenu(const QPoint&) ),
           this,          SLOT   ( onContextMenu(const QPoint&) ) );
}

//-----------------------------------------------------------------------------

//! Reaction on face picking.
//! \param pickRes [in] pick result.
void asiUI_ViewerPartListener::onFacePicked(const asiVisu_PickResult& pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes.GetPickedActor() );
  //
  if ( pickRes.GetPickedActor() && !nodeInfo )
    return;

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->Actualize(geom_n->GetFaceRepresentation().get(), false, true);
  //
  if ( m_wViewerSurface )
    m_wViewerSurface->PrsMgr()->Actualize(geom_n->GetSurfaceRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on edge picking.
//! \param pickRes [in] pick result.
void asiUI_ViewerPartListener::onEdgePicked(const asiVisu_PickResult& pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes.GetPickedActor() );
  //
  if ( pickRes.GetPickedActor() && !nodeInfo )
    return;

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->Actualize(geom_n->GetEdgeRepresentation().get(), false, true);
  //
  if ( m_wViewerSurface )
    m_wViewerSurface->PrsMgr()->Actualize(geom_n->GetCurveRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on context menu invocation.
//! \param globalPos [in] click position in global coordinates.
void asiUI_ViewerPartListener::onContextMenu(const QPoint& globalPos)
{
  // Get highlighted faces
  TColStd_PackedMapOfInteger faceIndices;
  asiEngine_Part( m_model, m_wViewerPart->PrsMgr() ).GetHighlightedFaces(faceIndices);

  // Get Part Node
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Part Node is null or bad-formed" );
    return;
  }

  // Context menu
  QMenu menu;

  // Actions
  QAction* pSaveBREPAction    = NULL;
  QAction* pShowNormsAction   = NULL;
  QAction* pInvertFacesAction = NULL;
  QAction* pShowOriContour    = NULL;
  QAction* pPickRotationPoint = NULL;
  QAction* pChangeBg          = NULL;

  // Action for picking custom rotation point
  pPickRotationPoint = menu.addAction("Set new focal point");
  pChangeBg          = menu.addAction("Set background color");

  // Prepare the context menu items
  if ( faceIndices.Extent() )
  {
    menu.addSeparator();
    //
    pSaveBREPAction = menu.addAction("Save to BREP...");
    //
    if ( m_wViewerPart->PrsMgr()->IsPresentable( STANDARD_TYPE(asiData_FaceNormsNode) ) )
      pShowNormsAction = menu.addAction("Show face normals");
    if ( m_wViewerPart->PrsMgr()->IsPresentable( STANDARD_TYPE(asiData_FaceContourNode) ) )
      pShowOriContour = menu.addAction("Show face oriented contour");
    //
    pInvertFacesAction = menu.addAction("Invert faces");
  }

  // Let sub-classes populate menu
  this->populateMenu(menu);

  // Execute
  QAction* selectedItem = menu.exec(globalPos);

  // Let sub-classes react
  this->executeAction(selectedItem);

  // Default reactions
  if ( selectedItem && selectedItem == pPickRotationPoint )
  {
    // Take picked position from interactor
    double pickedX = 0.0, pickedY = 0.0;
    m_wViewerPart->PrsMgr()->GetDefaultInteractorStyle()->GetPickedPos(pickedX, pickedY);

    // Pick world position
    vtkSmartPointer<vtkWorldPointPicker>
      worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    //
    worldPicker->Pick( pickedX, pickedY, 0, m_wViewerPart->PrsMgr()->GetRenderer() );
    double coord[3];
    worldPicker->GetPickPosition(coord);

    // The idea is to change focal point of the camera to the picked position because the
    // focal point is the center of rotation.

    m_wViewerPart->PrsMgr()->GetRenderer()->GetActiveCamera()->SetFocalPoint(coord[0], coord[1], coord[2]);
    m_wViewerPart->Repaint();
  }
  else if ( selectedItem && selectedItem == pChangeBg )
  {
    asiUI_BgColorDialog* pChangeBgDlg = new asiUI_BgColorDialog(m_wViewerPart);

    // Get renderer
    vtkRenderer* pRenderer = m_wViewerPart->PrsMgr()->GetRenderer();

    if ( !pRenderer->GetGradientBackground() )
    {
      double* aSolid = pRenderer->GetBackground();
      QColor aSolidClr;
      aSolidClr.setRedF(aSolid[0]);
      aSolidClr.setGreenF(aSolid[1]);
      aSolidClr.setBlueF(aSolid[2]);

      pChangeBgDlg->SetFillType(asiUI_BgColorDialog::FT_Solid);
      pChangeBgDlg->SetColor(asiUI_BgColorDialog::CLR_Solid, aSolidClr);
    }
    else
    {
      double* aGradientStart = pRenderer->GetBackground2();
      double* aGradientEnd   = pRenderer->GetBackground();
      pChangeBgDlg->SetFillType(asiUI_BgColorDialog::FT_Gradient);

      QColor aStartClr, aEndClr;
      aStartClr.setRedF(aGradientStart[0]);
      aStartClr.setGreenF(aGradientStart[1]);
      aStartClr.setBlueF(aGradientStart[2]);

      aEndClr.setRedF(aGradientEnd[0]);
      aEndClr.setGreenF(aGradientEnd[1]);
      aEndClr.setBlueF(aGradientEnd[2]);

      pChangeBgDlg->SetFillType(asiUI_BgColorDialog::FT_Gradient);
      pChangeBgDlg->SetColor(asiUI_BgColorDialog::CLR_GradientStart, aStartClr);
      pChangeBgDlg->SetColor(asiUI_BgColorDialog::CLR_GradientEnd, aEndClr);
    }

    if ( pChangeBgDlg->exec() )
    {
      // save and apply new colors
      QColor aColors[2];
      if ( pChangeBgDlg->GetFillType() == asiUI_BgColorDialog::FT_Solid )
      {
        aColors[0] = pChangeBgDlg->GetColor(asiUI_BgColorDialog::CLR_Solid);
      }
      else
      {
        aColors[0] = pChangeBgDlg->GetColor(asiUI_BgColorDialog::CLR_GradientStart);
        aColors[1] = pChangeBgDlg->GetColor(asiUI_BgColorDialog::CLR_GradientEnd);
      }

      // Apply colors
      if ( !aColors[1].isValid() )
      {
        // solid color fill
        pRenderer->SetBackground(aColors[0].redF(),
            aColors[0].greenF(), aColors[0].blueF());
        pRenderer->SetGradientBackground(false);
      }
      else
      {
        // gradient color fill
        pRenderer->SetBackground(aColors[1].redF(),
            aColors[1].greenF(), aColors[1].blueF());
        pRenderer->SetBackground2(aColors[0].redF(),
            aColors[0].greenF(),  aColors[0].blueF());
        pRenderer->SetGradientBackground(true);
      }
    }
  }
  else if ( selectedItem && selectedItem == pSaveBREPAction )
  {
    // Get highlighted sub-shapes
    TopTools_IndexedMapOfShape selected;
    asiEngine_Part( m_model, m_wViewerPart->PrsMgr() ).GetHighlightedSubShapes(selected);

    // Let user choose a filename
    QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);
    //
    if ( filename.isEmpty() )
      return;

    // Prepare a shape to dump
    TopoDS_Shape shape2Save;
    //
    if ( selected.Extent() == 1 )
      shape2Save = selected(1);
    else
    {
      // Put selected shapes to a compound
      TopoDS_Compound comp;
      BRep_Builder().MakeCompound(comp);
      //
      for ( int k = 1; k <= selected.Extent(); ++k )
        BRep_Builder().Add( comp, selected(k) );
      //
      shape2Save = comp;
    }

    // Save shape
    if ( !asiAlgo_Utils::WriteBRep( shape2Save, QStr2AsciiStr(filename) ) )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Cannot save shape" );
      return;
    }
  }
  else if ( selectedItem && selectedItem == pShowNormsAction )
  {
    TIMER_NEW
    TIMER_GO

    m_wViewerPart->PrsMgr()->Actualize( m_model->GetPartNode()->GetNormsRepresentation() );

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Visualization of normals")
  }
  else if ( selectedItem && selectedItem == pShowOriContour )
  {
    TIMER_NEW
    TIMER_GO

    m_wViewerPart->PrsMgr()->Actualize( m_model->GetPartNode()->GetContourRepresentation() );

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Visualization of oriented contour")
  }
  else if ( selectedItem && selectedItem == pInvertFacesAction )
  {
    TIMER_NEW
    TIMER_GO

    asiAlgo_InvertFaces InvertFaces( part_n->GetAAG() );
    //
    if ( !InvertFaces.Perform(faceIndices) )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Face inversion failed" );
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Invert faces")

    // Update Data Model
    m_model->OpenCommand();
    {
      asiEngine_Part(m_model).Update( InvertFaces.GetResult() );
    }
    m_model->CommitCommand();

    // Actualize
    m_wViewerPart->PrsMgr()->Actualize(part_n);
    m_wViewerPart->PrsMgr()->Actualize( m_model->GetPartNode()->GetNormsRepresentation() );
  }
  else
  {
    // Nothing was chosen
  }
}