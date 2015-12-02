//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_viewer2d_h
#define gui_viewer2d_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#pragma warning(pop)

//! VTK viewer for 2D.
class gui_viewer2d : public QMainWindow
{
  Q_OBJECT

public:

  ASitus_EXPORT
    gui_viewer2d(QWidget* parent = NULL);

  ASitus_EXPORT virtual
    ~gui_viewer2d();

public:

  ASitus_EXPORT void
    Repaint();

public slots:

  void onResetView();

};

#endif
