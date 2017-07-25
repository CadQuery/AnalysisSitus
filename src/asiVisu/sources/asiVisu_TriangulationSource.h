//-----------------------------------------------------------------------------
// Created on: 11 July 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
    registerFreeNodesCell(const NCollection_List<int>& nodeIDs,
                          vtkPolyData*                 polyData);

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

  //! Indicates whether edges are registered as VTK cells.
  bool m_bEdgesOn;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

};

#endif
