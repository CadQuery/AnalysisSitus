//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_section_h
#define gui_viewer_section_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_viewer.h>

// A-Situs (visualization) includes
#include <visu_interactor_style_pick.h>
#include <visu_pick_callback.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for a single skinning section.
class gui_viewer_section : public gui_viewer
{
  Q_OBJECT

public:

  gui_viewer_section(QWidget* parent = NULL);
  virtual ~gui_viewer_section();

public:

  void Repaint();

public slots:

  void onResetView();
  void onSectionPicked();

private:

  vtkSmartPointer<visu_pick_callback> m_pickCallback; //!< Pick callback.
  vtkSmartPointer<vtkTextWidget>      m_textWidget;   //!< Annotation.

};

#endif
