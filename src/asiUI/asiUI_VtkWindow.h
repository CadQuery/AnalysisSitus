//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_vtk_window_h
#define gui_vtk_window_h

// Analysis Situs includes
#include <analysis_situs.h>

// QVTK includes
#pragma warning(push, 0)
#include <QVTKWidget.h>
#pragma warning(pop)

//! Sub-class for QVTKWidget to override its closeEvent().
class gui_vtk_window : public QVTKWidget
{
  Q_OBJECT

public:

  ASitus_EXPORT
    gui_vtk_window(QWidget* parent = NULL, Qt::WindowFlags f = 0);

  ASitus_EXPORT
    virtual ~gui_vtk_window();

protected:

  ASitus_EXPORT virtual void
    closeEvent(QCloseEvent* pEvent);

signals:

  void windowClosed();

};


#endif