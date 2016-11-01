//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerMesh_h
#define asiUI_ViewerMesh_h

// A-Situs includes
#include <asiUI.h>

// Common includes
#include <asiData_Model.h>

// GUI includes
#include <asiUI_Viewer.h>

// Visualization includes
#include <asiUI_InteractorStylePick.h>
#include <asiUI_PickCallback.h>
#include <asiUI_RotationCallback.h>

// VTK includes
#include <vtkOrientationMarkerWidget.h>

//! Viewer for mesh.
class asiUI_ViewerMesh : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_ViewerMesh(QWidget* parent = NULL);
  virtual ~asiUI_ViewerMesh();

public:

  void Repaint();

public slots:

  void onResetView();
  void onMeshNodePicked();

private:

  //! Default interactor style.
  vtkSmartPointer<asiUI_InteractorStylePick> m_interactorStyleDefault;

  //! Pick callback.
  vtkSmartPointer<asiUI_PickCallback> m_pickCallback;

  //! Rotation callback.
  vtkSmartPointer<asiUI_RotationCallback> m_rotoCallback;

  //! Orientation marker widget.
  vtkSmartPointer<vtkOrientationMarkerWidget> m_axesWidget;

};

#endif
