//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_surface_h
#define gui_viewer_surface_h

// A-Situs includes
#include <analysis_situs.h>

// GUI includes
#include <gui_viewer.h>

//! Viewer for surface.
class gui_viewer_surface : public gui_viewer
{
  Q_OBJECT

public:

  gui_viewer_surface(QWidget* parent = NULL);
  virtual ~gui_viewer_surface();

public:

  void Repaint();

public slots:

  void onResetView();

};

#endif
