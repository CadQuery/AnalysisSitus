//-----------------------------------------------------------------------------
// Created on: 24 November 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiVisu_OctreeSource_h
#define asiVisu_OctreeSource_h

// asiVisu includes
#include <asiVisu.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkUnstructuredGridAlgorithm.h>

//! Source of unstructured data representing voxels in form of octree.
class asiVisu_OctreeSource : public vtkUnstructuredGridAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_OctreeSource, vtkUnstructuredGridAlgorithm);

  asiVisu_EXPORT static asiVisu_OctreeSource*
    New();

// Kernel:
public:

  //! Sets octree to visualize.
  //! \param[in] octree octree to visualize.
  asiVisu_EXPORT void
    SetInputOctree(void* octree);

  //! \return input octree.
  asiVisu_EXPORT void*
    GetInputOctree() const;

public:

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

  //! \return min scalar.
  double GetMinScalar() const
  {
    return m_fMinScalar;
  }

  //! \return max scalar.
  double GetMaxScalar() const
  {
    return m_fMaxScalar;
  }

protected:

  //! This method (called by superclass) performs conversion of our native
  //! data structures to VTK unstructured grid.
  //!
  //! \param[in]  request      describes "what" algorithm should do. This is
  //!                          typically just one key such as REQUEST_INFORMATION.
  //! \param[in]  inputVector  inputs of the algorithm.
  //! \param[out] outputVector outputs of the algorithm.
  //! \return status.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

private:

  //! Recursively adds SVO node (a voxel) to the unstructured grid being
  //! constructed.
  //! \param[in]     pNode parent node.
  //! \param[in,out] pData unstructured data set being populated.
  void addVoxels(void*                pNode,
                 vtkUnstructuredGrid* pData);

  //! Adds VTK voxel to the unstructured data set being constructed.
  //!
  //! \param[in]     node0 coordinates of the 1-st node.
  //! \param[in]     node1 coordinates of the 2-nd node.
  //! \param[in]     node2 coordinates of the 3-rd node.
  //! \param[in]     node3 coordinates of the 4-th node.
  //! \param[in]     node4 coordinates of the 5-th node.
  //! \param[in]     node5 coordinates of the 6-th node.
  //! \param[in]     node6 coordinates of the 7-th node.
  //! \param[in]     node7 coordinates of the 8-th node.
  //! \param[in]     sc0   scalar 0.
  //! \param[in]     sc1   scalar 1.
  //! \param[in]     sc2   scalar 2.
  //! \param[in]     sc3   scalar 3.
  //! \param[in]     sc4   scalar 4.
  //! \param[in]     sc5   scalar 5.
  //! \param[in]     sc6   scalar 6.
  //! \param[in]     sc7   scalar 7.
  //! \param[in,out] pData unstructured data set being populated.
  //! \return ID of the just added VTK cell.
  vtkIdType
    registerVoxel(const gp_Pnt&        node0,
                  const gp_Pnt&        node1,
                  const gp_Pnt&        node2,
                  const gp_Pnt&        node3,
                  const gp_Pnt&        node4,
                  const gp_Pnt&        node5,
                  const gp_Pnt&        node6,
                  const gp_Pnt&        node7,
                  const double         sc0,
                  const double         sc1,
                  const double         sc2,
                  const double         sc3,
                  const double         sc4,
                  const double         sc5,
                  const double         sc6,
                  const double         sc7,
                  vtkUnstructuredGrid* pData);

  //! Adds the passed coordinates as another point to the VTK data set.
  //! \param[in]     coords coordinates of the point to add.
  //! \param[in,out] pData  unstructured grid being constructed.
  //! \return ID of the newly added point.
  vtkIdType
    addPoint(const gp_Pnt&        coords,
             vtkUnstructuredGrid* pData);

private:

  //! Default constructor.
  asiVisu_OctreeSource();

  //! Destructor.
  ~asiVisu_OctreeSource();

private:

  asiVisu_OctreeSource(const asiVisu_OctreeSource&) = delete;
  asiVisu_OctreeSource& operator=(const asiVisu_OctreeSource&) = delete;

private:

  //! Octree to convert to VTK unstructured data.
  void* m_pOctree;

  //! Min scalar.
  double m_fMinScalar;

  //! Max scalar.
  double m_fMaxScalar;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

};

#endif
