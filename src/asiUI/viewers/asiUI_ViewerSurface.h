//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerSurface_h
#define asiUI_ViewerSurface_h

// A-Situs includes
#include <asiUI.h>

// GUI includes
#include <asiUI_Viewer.h>

//! Viewer for surface.
class asiUI_ViewerSurface : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_ViewerSurface(QWidget* parent = NULL);
  virtual ~asiUI_ViewerSurface();

public:

  void Repaint();

public slots:

  void onResetView();

};

#endif
