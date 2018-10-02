//-----------------------------------------------------------------------------
// Created on: 05 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiUI_PartGraph_h
#define asiUI_PartGraph_h

// asiUI includes
#include <asiUI_ViewerPart.h>
#include <asiUI_VtkWindow.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_TopoGraph.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_ListOfShape.hxx>

// VTK includes
#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextWidget.h>

//! Visualizes topology or adjacency graph.
class asiUI_PartGraph : public QObject
{
  Q_OBJECT

public:

  //! Type of graph to visualize (e.g. topology, AAG, etc.).
  enum Regime
  {
    Regime_Topology,
    Regime_AAG
  };

public:

  asiUI_EXPORT
    asiUI_PartGraph(const Handle(asiEngine_Model)& model,
                    asiUI_ViewerPart*              pPartViewer);

  asiUI_EXPORT
    ~asiUI_PartGraph();

public:

  asiUI_EXPORT void
    Render(const vtkSmartPointer<vtkGraph>& graph,
           const TopoDS_Shape&              shape,
           const Regime                     regime,
           const bool                       colorizeLocations);

  asiUI_EXPORT void
    Render(const TopoDS_Shape&               shape,
           const TopTools_IndexedMapOfShape& selectedFaces,
           const Regime                      regime,
           const TopAbs_ShapeEnum            leafType,
           const bool                        colorizeLocations);

  asiUI_EXPORT void
    RenderTopology(const TopoDS_Shape&    shape,
                   const TopAbs_ShapeEnum leafType,
                   const bool             colorizeLocations);

  asiUI_EXPORT void
    RenderAdjacency(const TopoDS_Shape&               shape,
                    const TopTools_IndexedMapOfShape& selectedFaces);

  asiUI_EXPORT void
    RenderAdjacency(const Handle(asiAlgo_AAG)&        aag,
                    const TopTools_IndexedMapOfShape& selectedFaces);

  asiUI_EXPORT void
    RenderEventCallback();

public:

  //! Allows to initialize presentable graph with externally constructed AAG.
  void SetAAG(const Handle(asiAlgo_AAG)& aag) { m_aag = aag; }

  //! Allows to initialize presentable graph with externally defined naming service.
  void SetNaming(const Handle(asiAlgo_Naming)& naming) { m_naming = naming; }

protected:

  asiUI_EXPORT vtkSmartPointer<vtkGraph>
    convertToGraph(const TopoDS_Shape&               shape,
                   const Handle(asiAlgo_AAG)&        aag,
                   const TopTools_IndexedMapOfShape& selectedFaces,
                   const Regime                      regime,
                   const TopAbs_ShapeEnum            leafType);

protected slots:

  void onViewerClosed();
  void onVertexPicked(const int, const TopAbs_ShapeEnum, const vtkIdType);

protected:

  asiUI_VtkWindow*          m_pWidget;       //!< Widget.
  vtkTextWidget*            m_textWidget;    //!< Text.
  vtkTextWidget*            m_summaryWidget; //!< Summary.
  asiUI_ViewerPart*         m_partViewer;    //!< Part viewer.
  //
  Handle(asiAlgo_Naming)    m_naming;        //!< Optional naming service.
  Handle(asiAlgo_TopoGraph) m_topoGraph;     //!< Topology graph.
  Handle(asiAlgo_AAG)       m_aag;           //!< AAG.
  Handle(asiEngine_Model)   m_model;         //!< Data Model instance.
  

};

#endif
