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

#undef VTK_TEST
#ifndef VTK_TEST

// exe includes
#include <exe_CommonFacilities.h>
#include <exe_Keywords.h>
#include <exe_MainWindow.h>

// asiVisu includes
#include <asiVisu_CalculusLawPrs.h>
#include <asiVisu_ContourPrs.h>
#include <asiVisu_CurvatureCombsPrs.h>
#include <asiVisu_DeviationPrs.h>
#include <asiVisu_FaceDomainPrs.h>
#include <asiVisu_GeomBoundaryEdgesPrs.h>
#include <asiVisu_GeomCurvePrs.h>
#include <asiVisu_GeomEdgePrs.h>
#include <asiVisu_GeomFaceContourPrs.h>
#include <asiVisu_GeomFaceNormsPrs.h>
#include <asiVisu_GeomSurfPrs.h>
#include <asiVisu_IVCurve2dPrs.h>
#include <asiVisu_IVCurvePrs.h>
#include <asiVisu_IVPointSet2dPrs.h>
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVSurfacePrs.h>
#include <asiVisu_IVTessItemPrs.h>
#include <asiVisu_IVTextItemPrs.h>
#include <asiVisu_IVTopoItemPrs.h>
#include <asiVisu_OctreePrs.h>
#include <asiVisu_PartPrs.h>
#include <asiVisu_ReCoedgePrs.h>
#include <asiVisu_ReEdgePrs.h>
#include <asiVisu_RePatchPrs.h>
#include <asiVisu_ReVertexPrs.h>
#include <asiVisu_SurfDeviationPrs.h>
#include <asiVisu_TessellationPrs.h>
#include <asiVisu_TessellationNormsPrs.h>
#include <asiVisu_TolerantRangePrs.h>
#include <asiVisu_TriangulationPrs.h>

// asiAlgo includes
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_FileFormat.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QSurfaceFormat>
#pragma warning(pop)

// Qt-VTK includes
#include <QVTKWidget.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLRenderWindow.h>
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
#pragma warning(push, 0)
#include <QDir>
#include <QTextStream>
#pragma warning(pop)

// Activate object factories
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkIOExportOpenGL2)
VTK_MODULE_INIT(vtkRenderingGL2PSOpenGL2)

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  // Check whether batch mode is requested.
  std::string scriptFilename;
  const bool
    isBatch = asiExe::GetKeyValue(argc, argv, ASITUS_KW_runscript, scriptFilename);

  //---------------------------------------------------------------------------
  // Create main window as (it will initialize all resources)
  //---------------------------------------------------------------------------

  if ( !isBatch )
  {
    // Needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat::setDefaultFormat( QVTKOpenGLNativeWidget::defaultFormat() );
  }

  // Construct Qt application.
  QApplication app(argc, argv);
  //
  QApplication::setWindowIcon( QIcon(":icons/asitus/asitus_icon_16x16.png") );

  // Construct main window but do not show it to allow off-screen batch.
  exe_MainWindow* pMainWindow = new exe_MainWindow(isBatch);

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

    // Load data dictionary.
    QByteArray dictFilename = resDir + "/asiExeDictionary.xml";
    QString dictFilenameStr = QString::fromLatin1( dictFilename.data() );
    //
    if ( !asiAlgo_Dictionary::Load( QStr2AsciiStr(dictFilenameStr) ) )
    {
      std::cout << "Cannot load data dictionary from "
                << QStr2AsciiStr(dictFilenameStr).ToCString() << std::endl;
    }
  }

  //---------------------------------------------------------------------------
  // Get command line arguments to process in a batch mode
  //---------------------------------------------------------------------------

  for ( int i = 0; i < argc; ++i )
    std::cout << "Passed arg[" << i << "]: " << argv[i] << std::endl;

  // Get Tcl interpeter.
  const Handle(asiTcl_Interp)&
    interp = pMainWindow->Widgets.wConsole->GetInterp();

  if ( isBatch )
  {
    std::cout << "Running Analysis Situs in batch mode..." << std::endl;

    // Execute script.
    const int ret = interp->Eval( asiTcl_SourceCmd( scriptFilename.c_str() ) );

    // Check result.
    if ( ret != TCL_OK )
      std::cout << "Batch mode finished with error code " << ret << "." << std::endl;
    else
      std::cout << "Batch mode finished successfully (error code " << ret << ")." << std::endl;

    return ret;
  }

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(asiVisu_PartPrs)
  REGISTER_PRESENTATION(asiVisu_DeviationPrs)
  REGISTER_PRESENTATION(asiVisu_OctreePrs)
  REGISTER_PRESENTATION(asiVisu_GeomBoundaryEdgesPrs)
  REGISTER_PRESENTATION(asiVisu_GeomCurvePrs)
  REGISTER_PRESENTATION(asiVisu_GeomEdgePrs)
  REGISTER_PRESENTATION(asiVisu_FaceDomainPrs)
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
  REGISTER_PRESENTATION(asiVisu_ReCoedgePrs)
  REGISTER_PRESENTATION(asiVisu_ReEdgePrs)
  REGISTER_PRESENTATION(asiVisu_RePatchPrs)
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
      QApplication::processEvents(QEventLoop::AllEvents, 10000);

      if ( interp->Eval(cmd) != TCL_OK )
        std::cout << "Tcl finished with error." << std::endl;

      QApplication::processEvents(QEventLoop::AllEvents, 10000);

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

// VTK init
#include <vtkAutoInit.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);

int main(int, char *[])
{
  std::cout << "Hello, offscreen rendering!" << std::endl;

   // Create a sphere
   vtkSmartPointer<vtkSphereSource> sphereSource = 
     vtkSmartPointer<vtkSphereSource>::New();

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = 
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // A renderer and render window
  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = 
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetOffScreenRendering( 1 ); 
  renderWindow->AddRenderer(renderer);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(1,1,1); // Background color white

  renderWindow->Render();

  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = 
    vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWindow);
  windowToImageFilter->Update();

  vtkSmartPointer<vtkPNGWriter> writer = 
    vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName("screenshot.png");
  writer->SetInputConnection(windowToImageFilter->GetOutputPort());
  writer->Write();

  return 0;
}

#endif
