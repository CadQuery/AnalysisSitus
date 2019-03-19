//-----------------------------------------------------------------------------
// Created on: 22 June 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoExe_Graph_h
#define ottoExe_Graph_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiUI includes
#include <asiUI_VtkWindow.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <vtkGraph.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextWidget.h>
#pragma warning(pop)

//! Visualizes dependency graph.
class ottoExe_Graph : public QObject
{
  Q_OBJECT

public:

  ottoExe_Graph(const Handle(asiEngine_Model)& model);

  ~ottoExe_Graph();

public:

  void
    Render();

  void
    RenderEventCallback();

protected:

  vtkSmartPointer<vtkGraph>
    convertToGraph();

protected slots:

  void onViewerClosed();
  void onVertexPicked(const vtkIdType);

protected:

  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_VtkWindow*        m_pWidget;    //!< Widget.
  vtkTextWidget*          m_textWidget; //!< Text.
};

#endif
