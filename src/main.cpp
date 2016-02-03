//-----------------------------------------------------------------------------
// Created on: 07 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// A-Situs (common) includes
#include <common_model.h>

// A-Situs (GUI) includes
#include <gui_welcome_window.h>

// A-Situs (visualization) includes
#include <visu_geom_face_prs.h>
#include <visu_geom_prs.h>
#include <visu_geom_surf_prs.h>
#include <visu_mesh_prs.h>
#include <visu_section_prs.h>
#include <visu_sections_prs.h>
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

// Activate object factory for Freetype fonts
VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QApplication::setWindowIcon( QIcon("D:/projects/analysis_situs/misc/asitus_icon_16x16.png") );

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
