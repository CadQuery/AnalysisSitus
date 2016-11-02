//-----------------------------------------------------------------------------
// Created on: 07 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// A-Situs (common) includes
#include <asiEngine_Model.h>

// A-Situs (GUI) includes
#include <gui_welcome_window.h>

// A-Situs (visualization) includes
#include <asiVisu_CalculusLawPrs.h>
#include <asiVisu_GeomBoundaryEdgesPrs.h>
#include <asiVisu_GeomContourPrs.h>
#include <asiVisu_GeomCurvePrs.h>
#include <asiVisu_GeomEdgePrs.h>
#include <asiVisu_GeomFacePrs.h>
#include <asiVisu_GeomPrs.h>
#include <asiVisu_GeomSurfPrs.h>
#include <visu_geom_volume_prs.h>
#include <asiVisu_IVCurvePrs.h>
#include <asiVisu_IVPointSet2dPrs.h>
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVSurfacePrs.h>
#include <asiVisu_IVTessItemPrs.h>
#include <asiVisu_IVTextItemPrs.h>
#include <asiVisu_IVTopoItemPrs.h>
#include <asiVisu_MeshPrs.h>
#include <asiVisu_REContoursPrs.h>
#include <asiVisu_REPointsPrs.h>
#include <asiVisu_RESurfacesPrs.h>
#include <visu_section_prs.h>
#include <visu_sections_prs.h>
#include <visu_ubend_law_prs.h>
#include <visu_ubend_prs.h>

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
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

// VTK init
#include <vtkAutoInit.h>

// OCCT includes
#include <OSD_Environment.hxx>

// Activate object factories
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  //
  TCollection_AsciiString ico_fn = OSD_Environment("AS_RESOURCES").Value();
  ico_fn += "/asitus_icon_16x16.png";
  QApplication::setWindowIcon( QIcon( ico_fn.ToCString() ) );

  //---------------------------------------------------------------------------
  // Create Data Model
  //---------------------------------------------------------------------------

  Handle(asiEngine_Model) M = new asiEngine_Model;
  if ( !M->NewEmpty() )
  {
    std::cout << "Cannot create new empty Model" << std::endl;
    return 1;
  }
  //
  M->DisableTransactions();
  {
    M->Populate();
  }
  M->EnableTransactions();

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(asiVisu_MeshPrs)
  REGISTER_PRESENTATION(asiVisu_GeomPrs)
  REGISTER_PRESENTATION(asiVisu_GeomBoundaryEdgesPrs)
  REGISTER_PRESENTATION(asiVisu_GeomCurvePrs)
  REGISTER_PRESENTATION(asiVisu_GeomEdgePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFacePrs)
  REGISTER_PRESENTATION(asiVisu_GeomSurfPrs)
  REGISTER_PRESENTATION(visu_geom_volume_prs)
  REGISTER_PRESENTATION(asiVisu_GeomContourPrs)
  REGISTER_PRESENTATION(asiVisu_REContoursPrs)
  REGISTER_PRESENTATION(asiVisu_REPointsPrs)
  REGISTER_PRESENTATION(asiVisu_RESurfacesPrs)
  REGISTER_PRESENTATION(visu_section_prs)
  REGISTER_PRESENTATION(visu_sections_prs)
  REGISTER_PRESENTATION(visu_ubend_prs)
  REGISTER_PRESENTATION(visu_ubend_law_prs)
  REGISTER_PRESENTATION(asiVisu_CalculusLawPrs)

  // Imperative viewer
  REGISTER_PRESENTATION(asiVisu_IVPointSet2dPrs)
  REGISTER_PRESENTATION(asiVisu_IVPointSetPrs)
  REGISTER_PRESENTATION(asiVisu_IVCurvePrs)
  REGISTER_PRESENTATION(asiVisu_IVSurfacePrs)
  REGISTER_PRESENTATION(asiVisu_IVTessItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTextItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTopoItemPrs)

  //---------------------------------------------------------------------------
  // Create main window
  //---------------------------------------------------------------------------

  gui_welcome_window* pWelcomeScreen = new gui_welcome_window;
  //
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  const int center_x   = ( screenGeometry.width() - pWelcomeScreen->width() ) / 2;
  const int center_y   = ( screenGeometry.height() - pWelcomeScreen->height() ) / 2;
  pWelcomeScreen->move(center_x, center_y); // Move to the center of the screen
  //
  pWelcomeScreen->show();

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
