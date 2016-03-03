//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_domain_h
#define gui_viewer_domain_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_viewer.h>

//! Viewer for face domain.
class gui_viewer_domain : public gui_viewer
{
  Q_OBJECT

public:

  gui_viewer_domain(QWidget* parent = NULL);
  virtual ~gui_viewer_domain();

public:

  void Repaint();

public slots:

  void onResetView();

};

#endif
