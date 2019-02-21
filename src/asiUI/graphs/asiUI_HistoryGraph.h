//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiUI_HistoryGraph_h
#define asiUI_HistoryGraph_h

// asiUI includes
#include <asiUI_ViewerPart.h>
#include <asiUI_VtkWindow.h>

// asiAlgo includes
#include <asiAlgo_Naming.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextWidget.h>

//! Visualizes history graph.
class asiUI_HistoryGraph : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_HistoryGraph(const Handle(asiEngine_Model)& model,
                       const Handle(asiAlgo_Naming)&  naming,
                       ActAPI_ProgressEntry           progress,
                       ActAPI_PlotterEntry            plotter);

  asiUI_EXPORT
    asiUI_HistoryGraph(const Handle(asiEngine_Model)& model,
                       const Handle(asiAlgo_History)& history,
                       ActAPI_ProgressEntry           progress,
                       ActAPI_PlotterEntry            plotter);

  asiUI_EXPORT
    ~asiUI_HistoryGraph();

public:

  asiUI_EXPORT void
    Render();

  asiUI_EXPORT void
    RenderEventCallback();

protected:

  asiUI_EXPORT vtkSmartPointer<vtkGraph>
    convertToGraph();

protected slots:

  void onViewerClosed();
  void onVertexPicked(const int, const vtkIdType);

protected:

  asiUI_VtkWindow* m_pWidget;    //!< Widget.
  vtkTextWidget*   m_textWidget; //!< Text.
  //
  Handle(asiEngine_Model) m_model;    //!< Data Model instance.
  Handle(asiAlgo_Naming)  m_naming;   //!< Naming service (optional).
  Handle(asiAlgo_History) m_history;  //!< Modification history.
  ActAPI_ProgressEntry    m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry     m_plotter;  //!< Imperative plotter.

};

#endif
