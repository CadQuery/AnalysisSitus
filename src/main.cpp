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

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
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
  QMainWindow* pMain = new QMainWindow();

  //---------------------------------------------------------------------------
  // Initialize viewer
  //---------------------------------------------------------------------------

  QVTKWidget* pViewWidget = new QVTKWidget();

  // Initialize renderer
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->GetActiveCamera()->ParallelProjectionOn();
  renderer->LightFollowCameraOn();
  renderer->TwoSidedLightingOn();
  renderer->SetBackground(0.2, 0.2, 0.2);

  // Initialize Render Window
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  pViewWidget->SetRenderWindow(renderWindow);

  // Initialize Interactor Style instance for normal operation mode
  vtkSmartPointer<vtkInteractorStyleTrackballCamera>
    iStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  // Initialize Render Window Interactor
  vtkSmartPointer<vtkRenderWindowInteractor>
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->SetInteractorStyle(iStyle);
  renderWindowInteractor->Initialize();

  //---------------------------------------------------------------------------
  // Other panels
  //---------------------------------------------------------------------------

  // Widgets and layouts
  gui_control_pane* pControlPane = new gui_control_pane(pMain);
  QWidget*          pBasePane    = new QWidget();
  QHBoxLayout*      pBaseLayout  = new QHBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pControlPane);
  pBaseLayout->addWidget(pViewWidget);
  pBasePane->setLayout(pBaseLayout);
  pBasePane->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  pMain->setCentralWidget(pBasePane);

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  pMain->show();
  return app.exec();
}
