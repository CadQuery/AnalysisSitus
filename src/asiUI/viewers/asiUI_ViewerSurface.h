//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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

  asiUI_EXPORT
    asiUI_ViewerSurface(QWidget* parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ViewerSurface();

public:

  void Repaint();

public slots:

  void onResetView();

};

#endif
