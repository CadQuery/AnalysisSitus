//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_part_h
#define gui_viewer_part_h

// A-Situs includes
#include <analysis_situs.h>

// Common includes
#include <common_model.h>

// GUI includes
#include <gui_viewer.h>

// Visualization includes
#include <visu_interactor_style_pick.h>
#include <visu_pick_callback.h>
#include <visu_rotation_callback.h>

// VTK includes
#include <vtkOrientationMarkerWidget.h>

//! Viewer for part.
class gui_viewer_part : public gui_viewer
{
  Q_OBJECT

public:

  gui_viewer_part(QWidget* parent = NULL);
  virtual ~gui_viewer_part();

public:

  void Repaint();

public slots:

  void onResetView();
  void onSubShapesPicked();

private:

  //! Default interactor style.
  vtkSmartPointer<visu_interactor_style_pick> m_interactorStyleDefault;

  //! Pick callback.
  vtkSmartPointer<visu_pick_callback> m_pickCallback;

  //! Rotation callback.
  vtkSmartPointer<visu_rotation_callback> m_rotoCallback;

  //! Orientation marker widget.
  vtkSmartPointer<vtkOrientationMarkerWidget> m_axesWidget;

};

#endif
