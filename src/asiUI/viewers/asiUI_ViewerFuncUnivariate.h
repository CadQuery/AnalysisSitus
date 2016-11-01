//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_Viewer_func_univariate_h
#define asiUI_Viewer_func_univariate_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <asiUI_Viewer.h>

// A-Situs (visualization) includes
#include <asiUI_InteractorStylePick.h>
#include <asiUI_PickCallback.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for a univariate function in explicit form.
class asiUI_Viewer_func_univariate : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_Viewer_func_univariate(vtkSmartPointer<asiVisu_PrsManager>& prs_mgr,
                             QWidget*                           parent = NULL);
  virtual ~asiUI_Viewer_func_univariate();

public:

  void Repaint();

public slots:

  void onResetView();

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
