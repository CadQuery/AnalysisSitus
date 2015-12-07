//-----------------------------------------------------------------------------
// Created on: 07 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// A-Situs (GUI) includes
#include <gui_main_window.h>

// A-Situs (visualization) includes
#include <visu_geom_face_prs.h>
#include <visu_geom_prs.h>
#include <visu_geom_surf_prs.h>
#include <visu_mesh_prs.h>

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

  REGISTER_PRESENTATION(visu_geom_prs)
  REGISTER_PRESENTATION(visu_geom_face_prs)
  REGISTER_PRESENTATION(visu_geom_surf_prs)
  REGISTER_PRESENTATION(visu_mesh_prs)

  //---------------------------------------------------------------------------
  // Create main window
  //---------------------------------------------------------------------------

  gui_main_window* pMainWindow = new gui_main_window();
  pMainWindow->show();

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
