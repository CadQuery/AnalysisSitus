//-----------------------------------------------------------------------------
// Created on: 07 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// This file is a part of Quaoar library. It has no license protection. You
// are free to use it the way you like, including incorporating of Quaoar
// sources and libraries into commercial applications.
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

// Windows includes
#include <Windows.h>

// VTK includes
#include <QVTKWidget.h>

// Qt includes
#include <QApplication.h>
#include <QMainWindow.h>

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QMainWindow* pMain = new QMainWindow();
  QVTKWidget* pWidget = new QVTKWidget();
  //pMain->setCentralWidget(pWidget);
  pMain->show();

  // TODO: NYI
  return app.exec();
}
