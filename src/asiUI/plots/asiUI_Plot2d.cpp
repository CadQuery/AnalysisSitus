//-----------------------------------------------------------------------------
// Created on: 14 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiUI_Plot2d.h>

// asiUI includes
#include <asiUI_Common.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkGL2PSExporter.h>
#include <vtkFloatArray.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTable.h>
#include <vtkVersion.h>

// Qt includes
#include <QApplication>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
//! \param[in] parent   parent object.
asiUI_Plot2d::asiUI_Plot2d(ActAPI_ProgressEntry progress,
                           ActAPI_PlotterEntry  plotter,
                           QWidget*             parent)
: QWidget    (parent),
  m_pViewer  (NULL),
  m_progress (progress),
  m_plotter  (plotter)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_Plot2d::~asiUI_Plot2d()
{}

//-----------------------------------------------------------------------------

//! Renders the plot.
//! \param[in] x  arguments.
//! \param[in] fx function values.
void asiUI_Plot2d::Render(const std::vector<double>& x,
                          const std::vector<double>& fx,
                          const std::string&         xLabel,
                          const std::string&         fxLabel,
                          const std::string&         plotTitle)
{
  /* =================
   *  Fill data table
   * ================= */

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
  arrX->SetName( xLabel.c_str() );
  table->AddColumn(arrX);

  vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
  arrC->SetName( fxLabel.c_str() );
  table->AddColumn(arrC);

  // Fill data table.
  const int numPoints = int ( x.size() );
  table->SetNumberOfRows(numPoints);
  //
  for ( int i = 0; i < numPoints; ++i )
  {
    table->SetValue(i, 0, x[i]);
    table->SetValue(i, 1, fx[i]);
  }

  /* =======================
   *  Prepare visualization
   * ======================= */

  // Set up the view.
  m_contextView = vtkSmartPointer<vtkContextView>::New();
  //
  m_contextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  //
  m_contextView->GetRenderWindow()->SetMultiSamples(8);

  // Create Qt window. NOTICE that interactor should be already given to
  // the render window at this stage.
  m_pViewer = new asiUI_VtkWindow();
  m_pViewer->SetRenderWindow( m_contextView->GetRenderWindow() );

  // Add multiple line plots, setting the colors, etc.
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  m_contextView->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);
  //
  line->SetInputData(table, 0, 1);
  line->SetColor(225, 0, 0, 255);
  line->SetWidth(2.0f);
  //
  line->GetXAxis()->SetTitle( xLabel.c_str() );
  line->GetXAxis()->GetTitleProperties()->BoldOff();
  line->GetXAxis()->GetTitleProperties()->SetLineOffset(8);
  line->GetXAxis()->GetLabelProperties()->BoldOff();
  line->GetXAxis()->GetLabelProperties()->SetColor(0.25, 0.25, 0.25);
  line->GetXAxis()->GetLabelProperties()->SetLineOffset(4);
  //
  line->GetYAxis()->SetTitle( fxLabel.c_str() );
  line->GetYAxis()->GetTitleProperties()->BoldOff();
  line->GetYAxis()->GetTitleProperties()->SetLineOffset(8);
  line->GetYAxis()->GetLabelProperties()->BoldOff();
  line->GetYAxis()->GetLabelProperties()->SetColor(0.25, 0.25, 0.25);
  line->GetYAxis()->GetLabelProperties()->SetLineOffset(4);

  /* =====================
   *  Prepare main widget
   * ===================== */

  QPushButton* pDump = new QPushButton("Dump graphics", this);
  QVBoxLayout* pVLay = new QVBoxLayout(this);
  //
  pVLay->addWidget(m_pViewer);
  pVLay->addWidget(pDump);

  /* ===================
   *  Start interaction
   * =================== */

  // Set callback on mouse events: otherwise QVTKWindow is
  // not updated for some reason.
  connect( m_pViewer, SIGNAL( mouseEvent(QMouseEvent*) ),
           this,      SLOT( onMouseEvent() ) );
  //
  connect( m_pViewer, SIGNAL( wheelEventOccurred() ),
           this,      SLOT( onMouseEvent() ) );
  //
  connect( pDump, SIGNAL( clicked() ),
           this,  SLOT( onDumpGraphics() ) );

  m_contextView->GetRenderWindow()->SetWindowName( plotTitle.c_str() );
  //
  m_contextView->GetInteractor()->Initialize();

  this->resize(400, 400);
  this->show();
}

//-----------------------------------------------------------------------------

void asiUI_Plot2d::onMouseEvent()
{
  m_pViewer->repaint();
}

//-----------------------------------------------------------------------------

//! Dumps graphics to file.
void asiUI_Plot2d::onDumpGraphics()
{
  vtkSmartPointer<vtkGL2PSExporter>
    gl2psExporter = vtkSmartPointer<vtkGL2PSExporter>::New();

  if ( gl2psExporter )
  {
    gl2psExporter->SetRenderWindow( m_contextView->GetRenderWindow() );
    gl2psExporter->Write3DPropsAsRasterImageOff();

    QString
      filename = asiUI_Common::selectGraphicsFile(asiUI_Common::OpenSaveAction_Save).section(".", 0, 0);

    gl2psExporter->SetFilePrefix( QStr2AsciiStr(filename).ToCString() );
    gl2psExporter->SetFileFormatToSVG();
    gl2psExporter->CompressOff();
    gl2psExporter->Write();
  }
}
