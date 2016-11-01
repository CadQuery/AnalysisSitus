//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_Viewer_domain_h
#define asiUI_Viewer_domain_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <asiUI_Viewer.h>

// A-Situs (visualization) includes
#include <asiUI_PDomainCallback.h>
#include <asiUI_PickCallback.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for face domain.
class asiUI_Viewer_domain : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_Viewer_domain(QWidget* parent = NULL);
  virtual ~asiUI_Viewer_domain();

public:

  void Repaint();

public slots:

  void onResetView();
  void onDomainPicked();
  void onKillEdges();
  void onJoinEdges();

private:

  vtkSmartPointer<asiUI_PDomainCallback> m_domainCallback; //!< Callback for domain operations.
  vtkSmartPointer<asiUI_PickCallback>    m_pickCallback;   //!< Pick callback.
  vtkSmartPointer<vtkTextWidget>         m_textWidget;     //!< Annotation.

};

#endif
