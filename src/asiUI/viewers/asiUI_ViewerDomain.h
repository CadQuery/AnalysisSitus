//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerDomain_h
#define asiUI_ViewerDomain_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_PDomainCallback.h>
#include <asiUI_PickCallback.h>
#include <asiUI_Viewer.h>

// A-Situs (engine) includes
#include <asiEngine_Model.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for face domain.
class asiUI_ViewerDomain : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_ViewerDomain(const Handle(asiEngine_Model)& model,
                     QWidget*                       parent = NULL);
  //
  virtual ~asiUI_ViewerDomain();

public:

  void Repaint();

public slots:

  void onResetView();
  void onDomainPicked();
  void onKillEdges();
  void onJoinEdges();

signals:

  void partModified();

private:

  Handle(asiEngine_Model)                m_model;          //!< Data Model instance.
  vtkSmartPointer<asiUI_PDomainCallback> m_domainCallback; //!< Callback for domain operations.
  vtkSmartPointer<asiUI_PickCallback>    m_pickCallback;   //!< Pick callback.
  vtkSmartPointer<vtkTextWidget>         m_textWidget;     //!< Annotation.

};

#endif
