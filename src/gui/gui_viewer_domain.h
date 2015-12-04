//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
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

  ASitus_EXPORT
    gui_viewer_domain(QWidget* parent = NULL);

  ASitus_EXPORT virtual
    ~gui_viewer_domain();

public:

  ASitus_EXPORT void
    Repaint();

public slots:

  void onResetView();

};

#endif
