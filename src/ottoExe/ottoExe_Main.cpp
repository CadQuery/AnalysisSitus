//-----------------------------------------------------------------------------
// Created on: 21 August 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// ottoExe includes
#include <ottoExe_MainWindow.h>

// ottoVisu includes
#include <ottoVisu_LimbPrs.h>

// asiAlgo include
#include <asiAlgo_Dictionary.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiVisu includes
#include <asiVisu_CalculusLawPrs.h>
#include <asiVisu_CurvatureCombsPrs.h>
#include <asiVisu_GeomBoundaryEdgesPrs.h>
#include <asiVisu_ContourPrs.h>
#include <asiVisu_GeomCurvePrs.h>
#include <asiVisu_GeomEdgePrs.h>
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
#include <asiVisu_TriangulationPrs.h>
#include <asiVisu_TolerantRangePrs.h>

// OCCT includes
#include <OSD_Environment.hxx>

// OCC License
//#include <OCCLicense_Activate.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QMainWindow>
#pragma warning(pop)

// Qt-VTK includes
#include <QVTKWidget.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

// VTK init
#include <vtkAutoInit.h>

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
  QApplication app(argc, argv);
  //
  QApplication::setWindowIcon( QIcon(":icons/asitus/asitus_icon_16x16.png") );

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
    QByteArray dictFilename = resDir + "/ottoExeDictionary.xml";
    QString dictFilenameStr = QString::fromLatin1( dictFilename.data() );
    //
    if ( !asiAlgo_Dictionary::Load( QStr2AsciiStr(dictFilenameStr) ) )
    {
      std::cout << "Cannot load data dictionary from "
                << QStr2AsciiStr(dictFilenameStr).ToCString() << std::endl;
    }
  }

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(asiVisu_PartPrs)
  REGISTER_PRESENTATION(asiVisu_GeomBoundaryEdgesPrs)
  REGISTER_PRESENTATION(asiVisu_GeomCurvePrs)
  REGISTER_PRESENTATION(asiVisu_GeomEdgePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFacePrs)
  REGISTER_PRESENTATION(asiVisu_GeomSurfPrs)
  REGISTER_PRESENTATION(asiVisu_ContourPrs)
  REGISTER_PRESENTATION(asiVisu_CalculusLawPrs)
  REGISTER_PRESENTATION(asiVisu_CurvatureCombsPrs)
  REGISTER_PRESENTATION(asiVisu_TolerantRangePrs)
  REGISTER_PRESENTATION(asiVisu_TriangulationPrs)

  // Imperative viewer
  REGISTER_PRESENTATION(asiVisu_IVPointSet2dPrs)
  REGISTER_PRESENTATION(asiVisu_IVPointSetPrs)
  REGISTER_PRESENTATION(asiVisu_IVCurve2dPrs)
  REGISTER_PRESENTATION(asiVisu_IVCurvePrs)
  REGISTER_PRESENTATION(asiVisu_IVSurfacePrs)
  REGISTER_PRESENTATION(asiVisu_IVTessItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTextItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTopoItemPrs)

  // Custom
  REGISTER_PRESENTATION(ottoVisu_LimbPrs)

  //---------------------------------------------------------------------------
  // Create main window
  //---------------------------------------------------------------------------

  // Let Qt do whatever it wants to do before showing UI. This helps
  // to avoid some sort of blinking on launch.
  QApplication::processEvents(QEventLoop::AllEvents, 10000);

  ottoExe_MainWindow* pMainWindow = new ottoExe_MainWindow;
  //
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  const int center_x   = ( screenGeometry.width() - pMainWindow->width() ) / 2;
  const int center_y   = ( screenGeometry.height() - pMainWindow->height() ) / 2;
  //
  pMainWindow->move(center_x/2, center_y/2); // Move to a handy position
  pMainWindow->show();

  //---------------------------------------------------------------------------
  // Activate license for OCC commercial products
  //---------------------------------------------------------------------------

  //OCCLicense_Activate("CANREC-7.2", "CADProcessor         0        0      0    0 94a68f717f74ce89bb568da639e4bffce24fe12cd7f55ec7caf6bf8a6d8fe135 OpenCASCADE / Master key");
  //OCCLicense_Activate("EMESH-7.2",  "CADProcessor         0        0      0    0 a807e08352d3cefc693a55a1d2f8cb9e01cd9423574b1b800ecda9f4188c802c OpenCASCADE / Master key");
  //OCCLicense_Activate("SSP-7.2",    "CADProcessor         0        0      0    0 e1d6978c5321bf481fbee8dc7303214fb14ad518fbb1b3986d6333781ad5070e OpenCASCADE / Master key");

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
