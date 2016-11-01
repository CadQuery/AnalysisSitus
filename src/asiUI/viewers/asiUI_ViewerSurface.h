//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_Viewer_surface_h
#define asiUI_Viewer_surface_h

// A-Situs includes
#include <analysis_situs.h>

// GUI includes
#include <asiUI_Viewer.h>

//! Viewer for surface.
class asiUI_Viewer_surface : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_Viewer_surface(QWidget* parent = NULL);
  virtual ~asiUI_Viewer_surface();

public:

  void Repaint();

public slots:

  void onResetView();

};

#endif
