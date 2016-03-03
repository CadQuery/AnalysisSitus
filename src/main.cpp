//-----------------------------------------------------------------------------
// Created on: 07 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// A-Situs (common) includes
#include <common_model.h>

// A-Situs (GUI) includes
#include <gui_welcome_window.h>

// A-Situs (visualization) includes
#include <visu_calculus_law_prs.h>
#include <visu_geom_face_prs.h>
#include <visu_geom_prs.h>
#include <visu_geom_surf_prs.h>
#include <visu_mesh_prs.h>
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

// Activate object factory for Freetype fonts
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

  Handle(common_model) M = new common_model;
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

  REGISTER_PRESENTATION(visu_mesh_prs)
  REGISTER_PRESENTATION(visu_geom_prs)
  REGISTER_PRESENTATION(visu_geom_face_prs)
  REGISTER_PRESENTATION(visu_geom_surf_prs)
  REGISTER_PRESENTATION(visu_section_prs)
  REGISTER_PRESENTATION(visu_sections_prs)
  REGISTER_PRESENTATION(visu_ubend_prs)
  REGISTER_PRESENTATION(visu_ubend_law_prs)
  REGISTER_PRESENTATION(visu_calculus_law_prs)

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
