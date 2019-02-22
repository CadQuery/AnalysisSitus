//-----------------------------------------------------------------------------
// Created on: 07 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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

// Windows includes
#include <Windows.h>

#undef VTK_TEST
#ifndef VTK_TEST

// exe includes
#include <exe_CommonFacilities.h>
#include <exe_Keywords.h>
#include <exe_MainWindow.h>

// asiVisu includes
#include <asiVisu_CalculusLawPrs.h>
#include <asiVisu_CurvatureCombsPrs.h>
#include <asiVisu_GeomBoundaryEdgesPrs.h>
#include <asiVisu_ContourPrs.h>
#include <asiVisu_GeomCurvePrs.h>
#include <asiVisu_GeomEdgePrs.h>
#include <asiVisu_GeomFaceContourPrs.h>
#include <asiVisu_GeomFaceNormsPrs.h>
#include <asiVisu_GeomFacePrs.h>
#include <asiVisu_PartPrs.h>
#include <asiVisu_GeomSurfPrs.h>
#include <asiVisu_IVCurve2dPrs.h>
#include <asiVisu_IVCurvePrs.h>
#include <asiVisu_IVPointSet2dPrs.h>
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVSurfacePrs.h>
#include <asiVisu_IVTessItemPrs.h>
#include <asiVisu_IVTextItemPrs.h>
#include <asiVisu_IVTopoItemPrs.h>
#include <asiVisu_ReEdgePrs.h>
#include <asiVisu_ReVertexPrs.h>
#include <asiVisu_SurfDeviationPrs.h>
#include <asiVisu_TessellationPrs.h>
#include <asiVisu_TessellationNormsPrs.h>
#include <asiVisu_TolerantRangePrs.h>
#include <asiVisu_TriangulationPrs.h>

// asiAlgo includes
#include <asiAlgo_FileFormat.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QMainWindow>
#pragma warning(pop)

// Qt-VTK includes
#include <QVTKWidget.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#pragma warning(pop)

// VTK init
#include <vtkAutoInit.h>

// OCCT includes
#include <OSD_Environment.hxx>

// Qt includes
#include <QDir>
#include <QTextStream>

// Activate object factories
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkIOExportOpenGL2);
VTK_MODULE_INIT(vtkRenderingGL2PSOpenGL2);

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  //---------------------------------------------------------------------------
  // Create main window as (it will initialize all resources)
  //---------------------------------------------------------------------------

  // Construct Qt application.
  QApplication app(argc, argv);
  //
  QApplication::setWindowIcon( QIcon(":icons/asitus/asitus_icon_16x16.png") );

  // Construct main window but do not show it to allow off-screen batch.
  exe_MainWindow* pMainWindow = new exe_MainWindow;

  //---------------------------------------------------------------------------
  // Set essential environment variables
  //---------------------------------------------------------------------------

  QByteArray appRoot = app.applicationDirPath().toUtf8();
  QByteArray resDir  = appRoot + "/resources";
  //
  if ( QDir(resDir).exists() )
  {
    qputenv("CSF_PluginDefaults", resDir);
    qputenv("CSF_ResourcesDefaults", resDir);
  }

  //---------------------------------------------------------------------------
  // Get command line arguments to process in a batch mode
  //---------------------------------------------------------------------------

  for ( int i = 0; i < argc; ++i )
    std::cout << "Passed arg[" << i << "]: " << argv[i] << std::endl;

  // Get Tcl interpeter.
  const Handle(asiTcl_Interp)&
    interp = pMainWindow->Widgets.wConsole->GetInterp();

  // Check whether batch mode is requested.
  std::string scriptFilename;
  const bool isBatch = asiExe::GetKeyValue(argc, argv, ASITUS_KW_runscript, scriptFilename);

  if ( isBatch )
  {
    std::cout << "Running Analysis Situs in batch mode..." << std::endl;

    // Execute script.
    const int ret = interp->Eval( asiTcl_SourceCmd( scriptFilename.c_str() ) );

    // Check result.
    if ( ret != TCL_OK )
      interp->PrintLastError();

    std::cout << "Batch mode finished with error code " << ret << "." << std::endl;

    return ret;
  }

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(asiVisu_PartPrs)
  REGISTER_PRESENTATION(asiVisu_GeomBoundaryEdgesPrs)
  REGISTER_PRESENTATION(asiVisu_GeomCurvePrs)
  REGISTER_PRESENTATION(asiVisu_GeomEdgePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFacePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFaceNormsPrs)
  REGISTER_PRESENTATION(asiVisu_GeomFaceContourPrs)
  REGISTER_PRESENTATION(asiVisu_GeomSurfPrs)
  REGISTER_PRESENTATION(asiVisu_CalculusLawPrs)
  REGISTER_PRESENTATION(asiVisu_CurvatureCombsPrs)
  REGISTER_PRESENTATION(asiVisu_SurfDeviationPrs)
  REGISTER_PRESENTATION(asiVisu_TessellationPrs)
  REGISTER_PRESENTATION(asiVisu_TessellationNormsPrs)
  REGISTER_PRESENTATION(asiVisu_TolerantRangePrs)
  REGISTER_PRESENTATION(asiVisu_TriangulationPrs)
  REGISTER_PRESENTATION(asiVisu_ReEdgePrs)
  REGISTER_PRESENTATION(asiVisu_ReVertexPrs)

  // Imperative viewer.
  REGISTER_PRESENTATION(asiVisu_IVPointSet2dPrs)
  REGISTER_PRESENTATION(asiVisu_IVPointSetPrs)
  REGISTER_PRESENTATION(asiVisu_IVCurve2dPrs)
  REGISTER_PRESENTATION(asiVisu_IVCurvePrs)
  REGISTER_PRESENTATION(asiVisu_IVSurfacePrs)
  REGISTER_PRESENTATION(asiVisu_IVTessItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTextItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTopoItemPrs)

  //---------------------------------------------------------------------------
  // Configure main window
  //---------------------------------------------------------------------------

  // Let Qt do whatever it wants to do before showing UI. This helps
  // to avoid some sort of blinking on launch.
  QApplication::processEvents(QEventLoop::AllEvents, 10000);

  // Move to a handy position.
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  const int center_x   = ( screenGeometry.width() - pMainWindow->width() ) / 2;
  const int center_y   = ( screenGeometry.height() - pMainWindow->height() ) / 2;
  //
  pMainWindow->move(center_x/8, center_y/4);

  // Show main window.
  pMainWindow->show();

  // Set focus on Tcl console.
  pMainWindow->Widgets.wConsole->setFocus();

  //---------------------------------------------------------------------------
  // Process the second argument to open the passed file
  //---------------------------------------------------------------------------

  if ( argc == 2 )
  {
    QStringList qtArgs = QApplication::arguments();
    //
    TCollection_AsciiString
      arg1Str = QStr2AsciiStr( QDir::fromNativeSeparators( qtArgs.at(1) ) );

    // Auto-recognize file format.
    asiAlgo_FileFormat
      format = asiAlgo_FileFormatTool(ASI_FILE_EXT).FormatFromFileContent(arg1Str);
    //
    if ( format == FileFormat_Unknown )
    {
      // Recognize file format from file extension
      format = asiAlgo_FileFormatTool(ASI_FILE_EXT).FormatFromFileExtension(arg1Str);
    }

    // Prepare Tcl command.
    TCollection_AsciiString cmd;
    //
    if ( format == FileFormat_STEP )
    {
      cmd = "load-step"; cmd += " "; cmd += arg1Str;
    }
    else if ( format == FileFormat_BREP )
    {
      cmd = "load-brep"; cmd += " "; cmd += arg1Str;
    }
    else
      std::cout << "Unexpected or not supported file format." << std::endl;

    // Execute command.
    if ( !cmd.IsEmpty() )
    {
      QApplication::processEvents(QEventLoop::AllEvents, 100);

      if ( interp->Eval(cmd) != TCL_OK )
        std::cout << "Tcl finished with error." << std::endl;

      QApplication::processEvents(QEventLoop::AllEvents, 100);

      if ( interp->Eval("fit") != TCL_OK )
        std::cout << "Tcl finished with error." << std::endl;
    }
  }

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}

#else

#include <vtkAxis.h>
#include <vtkTextProperty.h>
#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkFloatArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkPen.h>

// VTK init
#include <vtkAutoInit.h>

// Activate object factories
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)

int main(int, char *[])
{
  // Create a table with some points in it
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
  arrX->SetName("Parameter");
  table->AddColumn(arrX);

  vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
  arrC->SetName("Curvature");
  table->AddColumn(arrC);

  // Fill in the table with some example values
  int numPoints = 69;
  float inc = 7.5 / (numPoints-1);
  table->SetNumberOfRows(numPoints);
  for (int i = 0; i < numPoints; ++i)
  {
    table->SetValue(i, 0, i * inc);
    table->SetValue(i, 1, cos(i * inc));
  }

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();

  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  // Add multiple line plots, setting the colors etc
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 1);
  line->SetColor(225, 0, 0, 255);
  line->SetWidth(2.0f);
  //
  line->GetXAxis()->SetTitle("Parameter");
  line->GetXAxis()->GetTitleProperties()->BoldOff();
  line->GetXAxis()->GetTitleProperties()->SetLineOffset(8);
  line->GetXAxis()->GetLabelProperties()->BoldOff();
  line->GetXAxis()->GetLabelProperties()->SetColor(0.25, 0.25, 0.25);
  line->GetXAxis()->GetLabelProperties()->SetLineOffset(4);
  //
  line->GetYAxis()->SetTitle("Curvature");
  line->GetYAxis()->GetTitleProperties()->BoldOff();
  line->GetYAxis()->GetTitleProperties()->SetLineOffset(8);
  line->GetYAxis()->GetLabelProperties()->BoldOff();
  line->GetYAxis()->GetLabelProperties()->SetColor(0.25, 0.25, 0.25);
  line->GetYAxis()->GetLabelProperties()->SetLineOffset(4);

  view->GetRenderWindow()->SetMultiSamples(8);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}

#endif
