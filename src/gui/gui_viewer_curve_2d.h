//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_curve_2d_h
#define gui_viewer_curve_2d_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_viewer.h>

// A-Situs (visualization) includes
#include <visu_interactor_style_pick.h>
#include <visu_pick_callback.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for a two-dimensional parametric curve.
class gui_viewer_curve_2d : public gui_viewer
{
  Q_OBJECT

public:

  gui_viewer_curve_2d(vtkSmartPointer<visu_prs_manager>& prs_mgr,
                      QWidget*                           parent = NULL);
  virtual ~gui_viewer_curve_2d();

public:

  void Repaint();

public slots:

  void onResetView();

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
