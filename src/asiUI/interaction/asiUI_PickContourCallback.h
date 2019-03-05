//-----------------------------------------------------------------------------
// Created on: 21 November 2017
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

#ifndef asiUI_PickContourCallback_h
#define asiUI_PickContourCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#pragma warning(pop)

// Qt includes
#include <QObject>

//! Callback for interactive contour creation.
class asiUI_PickContourCallback : public QObject,
                                  public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  asiUI_EXPORT static asiUI_PickContourCallback* New();
  //
  vtkTypeMacro(asiUI_PickContourCallback, asiUI_ViewerCallback);

public:

  virtual void Execute(vtkObject*    pCaller,
                       unsigned long eventId,
                       void*         pCallData);

// Transactional methods representing different phases and topological
// situations when contructing a contour:
protected:

  bool
    startNewContour(void* pCallData);

  bool
    startNewContour(const Handle(asiData_ReVertexNode)& source);

  bool
    addContourEdge(void* pCallData);

  bool
    addContourEdge(const Handle(asiData_ReVertexNode)& target);

  bool
    addContourCoEdge(const Handle(asiData_ReEdgeNode)& edge,
                     const bool                        samesense);

  bool
    completeContourCoEdge(const Handle(asiData_ReEdgeNode)& edge,
                          const bool                        samesense);

  bool
    completeContour(const Handle(asiData_ReVertexNode)& target);

  bool
    completeContourRecursively(const Handle(asiData_ReVertexNode)& target,
                               const bool                          buildGeometry,
                               ActAPI_DataObjectIdMap&             visitedVertices);

// Non-transactional methods to operate with low-level data structure:
protected:

  Handle(asiData_ReCoEdgeNode)
    buildNewEdge(const Handle(asiData_ReVertexNode)& target,
                 const bool                          doProjectLine);

  Handle(asiData_ReCoEdgeNode)
    buildNewEdge(const Handle(asiData_ReVertexNode)& target);

  Handle(asiData_ReCoEdgeNode)
    buildNewCoEdge(const Handle(asiData_ReEdgeNode)& edge,
                   const bool                        samesense);

  Handle(asiData_ReEdgeNode)
    findEdgeOnVertices(const Handle(asiData_ReVertexNode)& v1,
                       const Handle(asiData_ReVertexNode)& v2,
                       bool&                               samesense);

  bool
    findEdgesOnVertex(const Handle(asiData_ReVertexNode)&      vertex,
                      const Handle(asiData_ReEdgeNode)&        self2Skip,
                      std::vector<Handle(asiData_ReEdgeNode)>& edges);

  size_t
    chooseLeftTurnEdge(const Handle(asiData_ReEdgeNode)&        currentEdge,
                       const Handle(asiData_ReVertexNode)&      commonVertex,
                       const bool                               isSameSense,
                       std::vector<Handle(asiData_ReEdgeNode)>& candidates);

  Handle(asiData_ReCoEdgeNode)
    getPrevCoEdge(const Handle(asiData_ReEdgeNode)& currentEdge);

  gp_Vec
    getCoEdgeTrailingDir(const Handle(asiData_ReCoEdgeNode)& coedge);

  Handle(asiData_ReVertexNode)
    getPickedVertex(void* pCallData);

  bool
    projectLine(const gp_XYZ&        p1,
                const gp_XYZ&        p2,
                std::vector<gp_XYZ>& pts);

private:

  asiUI_PickContourCallback  (asiUI_Viewer* pViewer);
  ~asiUI_PickContourCallback ();

};

#endif
