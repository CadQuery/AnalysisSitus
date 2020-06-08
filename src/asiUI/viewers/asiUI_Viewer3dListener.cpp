//-----------------------------------------------------------------------------
// Created on: 29 December 2017
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
#include <asiUI_Viewer3dListener.h>

// asiUI includes
#include <asiUI_BgColorDialog.h>
#include <asiUI_Common.h>

// asiEngine includes
#include <asiEngine_Part.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCamera.h>
#include <vtkGL2PSExporter.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param[in] pViewer  managed 3D viewer.
//! \param[in] model    Data Model instance.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
asiUI_Viewer3dListener::asiUI_Viewer3dListener(asiUI_Viewer*                  pViewer,
                                               const Handle(asiEngine_Model)& model,
                                               ActAPI_ProgressEntry           progress,
                                               ActAPI_PlotterEntry            plotter)
: QObject    (),
  m_pViewer  (pViewer),
  m_model    (model),
  m_progress (progress),
  m_plotter  (plotter)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_Viewer3dListener::~asiUI_Viewer3dListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_Viewer3dListener::Connect()
{
  connect( m_pViewer, SIGNAL ( contextMenu(const QPoint&) ),
           this,      SLOT   ( onContextMenu(const QPoint&) ) );
}

//-----------------------------------------------------------------------------

//! Reaction on context menu invocation.
//! \param[in] globalPos click position in global coordinates.
void asiUI_Viewer3dListener::onContextMenu(const QPoint& globalPos)
{
  if ( m_pViewer->PrsMgr()->GetDefaultInteractorStyle()->IsControlPressed() )
    return;

  // Context menu
  QMenu menu;

  // Actions
  QAction* pPickRotationPoint = nullptr;
  QAction* pChangeBg          = nullptr;
  QAction* pDumpGraphics      = nullptr;
  QAction* pToggleAxes        = nullptr;

  // Action for picking custom rotation point
  if ( m_pViewer->PrsMgr()->GetInteractionMode() != asiVisu_PrsManager::InteractionMode_2D )
    pPickRotationPoint = menu.addAction("Set new focal point");

  // Other actions
  pChangeBg     = menu.addAction("Set background color");
  pDumpGraphics = menu.addAction("Dump graphics");
  pToggleAxes   = menu.addAction("Toggle axes");

  // Let sub-classes populate menu
  this->populateMenu(menu);

  // Execute
  QAction* selectedItem = menu.exec(globalPos);

  // Let sub-classes react
  this->executeAction(selectedItem);

  //---------------------------------------------------------------------------
  // ACTION: set new focal point
  //---------------------------------------------------------------------------
  if ( selectedItem && (selectedItem == pPickRotationPoint) )
  {
    // Take picked position from interactor
    double pickedX = 0.0, pickedY = 0.0;
    m_pViewer->PrsMgr()->GetDefaultInteractorStyle()->GetPickedPos(pickedX, pickedY);

    // Pick world position
    vtkSmartPointer<vtkCellPicker>
      positionPicker = vtkSmartPointer<vtkCellPicker>::New();
    //
    positionPicker->Pick( pickedX, pickedY, 0, m_pViewer->PrsMgr()->GetRenderer() );
    double coord[3];
    positionPicker->GetPickPosition(coord);

    // The idea is to change focal point of the camera to the picked position because the
    // focal point is the center of rotation.

    m_pViewer->PrsMgr()->GetRenderer()->GetActiveCamera()->SetFocalPoint(coord[0], coord[1], coord[2]);
    m_pViewer->Repaint();
  }

  //---------------------------------------------------------------------------
  // ACTION: set background color
  //---------------------------------------------------------------------------
  else if ( selectedItem && (selectedItem == pChangeBg) )
  {
    asiUI_BgColorDialog* pChangeBgDlg = new asiUI_BgColorDialog(m_pViewer);

    // Get renderer
    vtkRenderer* pRenderer = m_pViewer->PrsMgr()->GetRenderer();

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
        pRenderer->SetBackground( aColors[0].redF(),
                                  aColors[0].greenF(),
                                  aColors[0].blueF() );
        //
        pRenderer->SetGradientBackground(false);
      }
      else
      {
        // gradient color fill
        pRenderer->SetBackground( aColors[1].redF(),
                                  aColors[1].greenF(),
                                  aColors[1].blueF() );
        //
        pRenderer->SetBackground2( aColors[0].redF(),
                                   aColors[0].greenF(),
                                   aColors[0].blueF() );
        //
        pRenderer->SetGradientBackground(true);
      }

      m_pViewer->Repaint();
    }
  }
  //---------------------------------------------------------------------------
  // ACTION: dump graphics
  //---------------------------------------------------------------------------
  else if ( selectedItem && selectedItem == pDumpGraphics )
  {
    vtkSmartPointer<vtkGL2PSExporter>
      gl2psExporter = vtkSmartPointer<vtkGL2PSExporter>::New();

    if ( gl2psExporter )
    {
      gl2psExporter->SetRenderWindow( m_pViewer->PrsMgr()->GetRenderWindow() );
      gl2psExporter->Write3DPropsAsRasterImageOff();

      QString
        filename = asiUI_Common::selectGraphicsFile(asiUI_Common::OpenSaveAction_Save).section(".", 0, 0);

      gl2psExporter->SetFilePrefix( QStr2AsciiStr(filename).ToCString() );
      gl2psExporter->SetFileFormatToSVG();
      gl2psExporter->CompressOff();
      gl2psExporter->Write();
    }
  }
  //---------------------------------------------------------------------------
  // ACTION: toggle axes
  //---------------------------------------------------------------------------
  else if ( selectedItem && selectedItem == pToggleAxes )
  {
    const int visible = m_pViewer->PrsMgr()->GetTrihedron()->GetVisibility();
    //
    m_pViewer->PrsMgr()->GetTrihedron()->SetVisibility(visible > 0 ? 0 : 1);

    // Update our render window.
    m_pViewer->PrsMgr()->GetRenderWindow()->Render();
  }
}
