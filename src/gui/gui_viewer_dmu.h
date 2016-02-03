//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_dmu_h
#define gui_viewer_dmu_h

// A-Situs includes
#include <analysis_situs.h>

// GUI includes
#include <gui_viewer.h>

// Visualization includes
#include <visu_interactor_style_pick.h>
#include <visu_pick_callback.h>
#include <visu_rotation_callback.h>

// VTK includes
#include <vtkOrientationMarkerWidget.h>

//! Viewer for dMU.
class gui_viewer_dmu : public gui_viewer
{
  Q_OBJECT

public:

  gui_viewer_dmu(QWidget* parent = NULL);
  virtual ~gui_viewer_dmu();

public:

  void Repaint();

public slots:

  void onResetView();

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
