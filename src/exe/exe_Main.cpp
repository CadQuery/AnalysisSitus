//-----------------------------------------------------------------------------
// Created on: 07 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// exe includes
#include <exe_CommonFacilities.h>
#include <exe_MainWindow.h>

// asiVisu includes
#include <asiVisu_CalculusLawPrs.h>
#include <asiVisu_GeomBoundaryEdgesPrs.h>
#include <asiVisu_GeomContourPrs.h>
#include <asiVisu_GeomCurvePrs.h>
#include <asiVisu_GeomEdgePrs.h>
#include <asiVisu_GeomFaceContourPrs.h>
#include <asiVisu_GeomFaceNormsPrs.h>
#include <asiVisu_GeomFacePrs.h>
#include <asiVisu_GeomPrs.h>
#include <asiVisu_GeomSurfPrs.h>
#include <asiVisu_IVCurvePrs.h>
#include <asiVisu_IVPointSet2dPrs.h>
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVSurfacePrs.h>
#include <asiVisu_IVTessItemPrs.h>
#include <asiVisu_IVTextItemPrs.h>
#include <asiVisu_IVTopoItemPrs.h>

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

// Qt includes
#include <QTextStream>

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
  QApplication::setWindowIcon( QIcon(":icons/asitus/asitus_icon_16x16.png") );

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(asiVisu_GeomPrs)
  REGISTER_PRESENTATION(asiVisu_GeomBoundaryEdgesPrs)
  REGISTER_PRESENTATION(asiVisu_GeomCurvePrs)
  REGISTER_PRESENTATION(asiVisu_GeomEdgePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFacePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFaceNormsPrs)
  REGISTER_PRESENTATION(asiVisu_GeomFaceContourPrs)
  REGISTER_PRESENTATION(asiVisu_GeomSurfPrs)
  REGISTER_PRESENTATION(asiVisu_GeomContourPrs)
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

  exe_MainWindow* pMainWindow = new exe_MainWindow;
  //
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  const int center_x   = ( screenGeometry.width() - pMainWindow->width() ) / 2;
  const int center_y   = ( screenGeometry.height() - pMainWindow->height() ) / 2;
  //
  pMainWindow->move(center_x/2, center_y/2); // Move to a handy position
  pMainWindow->show();

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
