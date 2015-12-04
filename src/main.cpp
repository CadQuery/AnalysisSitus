//-----------------------------------------------------------------------------
// Created on: 07 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// A-Situs (GUI) includes
#include <gui_control_pane.h>
#include <gui_object_browser.h>
#include <gui_viewer_domain.h>
#include <gui_viewer_part.h>
#include <gui_viewer_surface.h>

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
  // Initialize viewer for part
  //---------------------------------------------------------------------------

  gui_viewer_part* pViewerPart = new gui_viewer_part();

  // Desktop used for sizing
  QDesktopWidget desktop;

  // Set size and show
  pViewerPart->setMinimumSize(desktop.height()*0.5, desktop.height()*0.5);
  pViewerPart->show();

  //---------------------------------------------------------------------------
  // Initialize viewer for face domain
  //---------------------------------------------------------------------------

  gui_viewer_domain* pViewerDomain = new gui_viewer_domain();

  // Set size and show
  pViewerDomain->setMinimumSize(desktop.height()*0.4, desktop.height()*0.3);
  pViewerDomain->show();

  //---------------------------------------------------------------------------
  // Initialize viewer for host surface
  //---------------------------------------------------------------------------

  gui_viewer_surface* pViewerSurf = new gui_viewer_surface();

  // Set size and show
  pViewerSurf->setMinimumSize(desktop.height()*0.4, desktop.height()*0.3);
  pViewerSurf->show();

  //---------------------------------------------------------------------------
  // Create tree view
  //---------------------------------------------------------------------------

  gui_object_browser* pTree = new gui_object_browser();
  pTree->show();

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
