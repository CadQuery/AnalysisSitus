//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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

#ifndef asiVisu_TriangulationSource_h
#define asiVisu_TriangulationSource_h

// asiVisu includes
#include <asiVisu_MeshPrimitive.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <Poly_Triangulation.hxx>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! \todo Free links are not recognized yet.
//! \todo Dangling links are not recognized yet.
//! \todo Border nodes are not recognized yet.
//!
//! Source of polygonal data representing surface triangulation.
class asiVisu_TriangulationSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_TriangulationSource, vtkPolyDataAlgorithm);

  asiVisu_EXPORT static asiVisu_TriangulationSource*
    New();

// Kernel:
public:

  //! Sets triangulation to visualize.
  //! \param triangulation [in] triangulation to visualize.
  asiVisu_EXPORT void
    SetInputTriangulation(const Handle(Poly_Triangulation)& triangulation);

  //! \return initial triangulation.
  asiVisu_EXPORT const Handle(Poly_Triangulation)&
    GetInputTriangulation() const;

public:

  //! Enables collecting vertices in the polygonal source. Vertices are not
  //! essential in visualization but occupy a certain amount of memory.
  void CollectVerticesModeOn()
  {
    m_bVerticesOn = true;
    //
    this->Modified();
  }

  //! Disables collecting vertices in the polygonal source. Vertices are not
  //! essential in visualization but occupy a certain amount of memory.
  void CollectVerticesModeOff()
  {
    m_bVerticesOn = false;
    //
    this->Modified();
  }

  //! Enables collecting edges in the polygonal source. Edges are not
  //! essential in visualization but occupy a certain amount of memory.
  void CollectEdgesModeOn()
  {
    m_bEdgesOn = true;
    //
    this->Modified();
  }

  //! Disables collecting edges in the polygonal source. Edges are not
  //! essential in visualization but occupy a certain amount of memory.
  void CollectEdgesModeOff()
  {
    m_bEdgesOn = false;
    //
    this->Modified();
  }

  //! Initializes source with diagnostic tools: progress notifier and
  //! imperative plotter.
  //! \param progress [in] progress notifier.
  //! \param plotter  [in] imperative plotter.
  void SetDiagnosticTools(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter)
  {
    m_progress = progress;
    m_plotter  = plotter;
  }

protected:

  //! This method (called by superclass) performs conversion of OCCT
  //! data structures to VTK polygonal representation.
  //!
  //! \param request      [in]  describes "what" algorithm should do. This is
  //!                           typically just one key such as REQUEST_INFORMATION.
  //! \param inputVector  [in]  inputs of the algorithm.
  //! \param outputVector [out] outputs of the algorithm.
  //! \return status.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

private:

  //! Creates or takes an existing mesh node by its ID.
  //!
  //! \param nodeID   [in]     node ID.
  //! \param polyData [in,out] output polygonal data.
  //! \return internal VTK ID for the corresponding point.
  vtkIdType
    findMeshNode(const int    nodeID,
                 vtkPolyData* polyData);

  //! Adds the mesh element with to VTK polygonal data.
  //!
  //! \param elemId   [in]     1-based element ID in the original Poly_Triangulation.
  //! \param nodeID1  [in]     ID of the 1-st node.
  //! \param nodeID2  [in]     ID of the 2-nd node.
  //! \param nodeID3  [in]     ID of the 3-rd node.
  //! \param polyData [in,out] polygonal data being populated.
  //! \return ID of the just added VTK cell.
  vtkIdType
    registerFacet(const int    elemId,
                  const int    nodeID1,
                  const int    nodeID2,
                  const int    nodeID3,
                  vtkPolyData* polyData);

  //! Registers the passed mesh nodes as free nodes by constructing a
  //! dedicated VTK cell of VTK_POLY_VERTEX type.
  //!
  //! \param nodeIDs  [in]     collection of free node IDs.
  //! \param polyData [in,out] polygonal data being populated.
  //! \return ID of the registered cell.
  vtkIdType
    registerFreeNodesCell(const TColStd_PackedMapOfInteger& nodeIDs,
                          vtkPolyData*                      polyData);

  //! Registers the passed mesh nodes as mesh link by constructing a
  //! dedicated VTK cell of VTK_LINE type.
  //!
  //! \param nodeID1  [in]     ID of the first node.
  //! \param nodeID2  [in]     ID of the second node.
  //! \param type     [in]     primitive type.
  //! \param polyData [in,out] polygonal data being populated.
  //! \return ID of the registered cell.
  vtkIdType
    registerLinkCell(const int                   nodeID1,
                     const int                   nodeID2,
                     const asiVisu_MeshPrimitive type,
                     vtkPolyData*                polyData);

  //! Registers the passed mesh node as a VTK vertex cell.
  //!
  //! \param nodeID   [in]     ID of the mesh node.
  //! \param type     [in]     primitive type.
  //! \param polyData [in,out] polygonal data being populated.
  //! \return ID of the registered cell.
  vtkIdType
    registerNodeCell(const int                   nodeID,
                     const asiVisu_MeshPrimitive type,
                     vtkPolyData*                polyData);

private:

  //! Default constructor.
  asiVisu_TriangulationSource();

  //! Destructor.
  ~asiVisu_TriangulationSource();

private:

  asiVisu_TriangulationSource(const asiVisu_TriangulationSource&);
  asiVisu_TriangulationSource& operator=(const asiVisu_TriangulationSource&);

private:
  
  //! Triangulation to convert to VTK polygonal data.
  Handle(Poly_Triangulation) m_triangulation;

  //! Registered VTK points.
  NCollection_DataMap<int, vtkIdType> m_regPoints;

  //! Indicates whether vertices are registered as VTK cells.
  bool m_bVerticesOn;

  //! Indicates whether edges are registered as VTK cells.
  bool m_bEdgesOn;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

};

#endif
