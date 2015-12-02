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
#include <gui_viewer.h>
#include <gui_viewer2d.h>

// A-Situs (visualization) includes
#include <visu_geom_face_prs.h>
#include <visu_geom_prs.h>
#include <visu_mesh_prs.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QHBoxLayout>
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

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(visu_geom_prs)
  REGISTER_PRESENTATION(visu_geom_face_prs)
  REGISTER_PRESENTATION(visu_mesh_prs)

  //---------------------------------------------------------------------------
  // Initialize viewer
  //---------------------------------------------------------------------------

  gui_viewer* pViewer = new gui_viewer();

  // Set size and show
  QDesktopWidget dw;
  pViewer->setMinimumSize(dw.height()*0.5, dw.height()*0.5);
  pViewer->show();

  //---------------------------------------------------------------------------
  // Initialize viewer 2d
  //---------------------------------------------------------------------------

  gui_viewer2d* pViewer2d = new gui_viewer2d();

  // Set size and show
  QDesktopWidget dw2d;
  pViewer2d->setMinimumSize(dw2d.height()*0.25, dw2d.height()*0.25);
  pViewer2d->show();

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
