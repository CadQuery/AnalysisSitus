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

// Own include
#include <asiVisu_OctreeSource.h>

// asiVisu includes
#include <asiVisu_MeshUtils.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkCellTypes.h>
#include <vtkDataObject.h>
#include <vtkDoubleArray.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>

#if defined USE_MOBIUS
  #include <mobius/cascade.h>
  #include <mobius/poly_DistanceField.h>
  #include <mobius/poly_SVO.h>
#endif

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_OctreeSource)

//-----------------------------------------------------------------------------

asiVisu_OctreeSource::asiVisu_OctreeSource()
: vtkUnstructuredGridAlgorithm ( ),
  m_pOctree                    ( nullptr ),
  m_fMinScalar                 ( DBL_MAX ),
  m_fMaxScalar                 (-DBL_MAX ),
  m_bZeroCrossingOnly          ( false   )
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline.
}

//-----------------------------------------------------------------------------

asiVisu_OctreeSource::~asiVisu_OctreeSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_OctreeSource::SetInputOctree(void* octree)
{
  m_pOctree = octree;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

void* asiVisu_OctreeSource::GetInputOctree() const
{
  return m_pOctree;
}

//-----------------------------------------------------------------------------

void asiVisu_OctreeSource::SetZeroCrossingOnly(const bool isOn)
{
  m_bZeroCrossingOnly = isOn;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

int asiVisu_OctreeSource::RequestData(vtkInformation*        asiVisu_NotUsed(request),
                                      vtkInformationVector** asiVisu_NotUsed(inputVector),
                                      vtkInformationVector*  outputVector)
{
  if ( !m_pOctree )
  {
    vtkErrorMacro( << "Invalid input: nullptr octree." );
    return 0;
  }

  /* ================
   *  Prepare output.
   * ================ */

  // Get the output unstructured grid data from the information vector.
  vtkUnstructuredGrid* pOutputGrid = vtkUnstructuredGrid::GetData(outputVector);
  pOutputGrid->Allocate();
  pOutputGrid->SetPoints( vtkSmartPointer<vtkPoints>::New() );

  // Prepare array for nodal scalars.
  vtkPointData*                   pPointData = pOutputGrid->GetPointData();
  vtkSmartPointer<vtkDoubleArray> scalarsArr = asiVisu_Utils::InitDoubleArray(ARRNAME_VOXEL_N_SCALARS);
  //
  pPointData->SetScalars(scalarsArr);

  /* ============
   *  Add voxels.
   * ============ */

  this->addVoxels(m_pOctree, pOutputGrid);

  return 1;
}

//-----------------------------------------------------------------------------

void asiVisu_OctreeSource::addVoxels(void*                pNode,
                                     vtkUnstructuredGrid* pData)
{
  if ( !pNode )
    return;

#if defined USE_MOBIUS
  mobius::poly_SVO* pMobNode = static_cast<mobius::poly_SVO*>(pNode);

  if ( pMobNode->IsLeaf() )
  {
    if ( (  m_bZeroCrossingOnly && mobius::poly_DistanceField::IsZeroCrossing(pMobNode) ) ||
           !m_bZeroCrossingOnly )
    {
      const double sc0 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(0, 0, 0) );
      const double sc1 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(1, 0, 0) );
      const double sc2 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(0, 1, 0) );
      const double sc3 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(1, 1, 0) );
      //
      const double sc4 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(0, 0, 1) );
      const double sc5 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(1, 0, 1) );
      const double sc6 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(0, 1, 1) );
      const double sc7 = pMobNode->GetScalar( mobius::poly_SVO::GetCornerID(1, 1, 1) );

      m_fMinScalar = Min(m_fMinScalar, sc0);
      m_fMinScalar = Min(m_fMinScalar, sc1);
      m_fMinScalar = Min(m_fMinScalar, sc2);
      m_fMinScalar = Min(m_fMinScalar, sc3);
      m_fMinScalar = Min(m_fMinScalar, sc4);
      m_fMinScalar = Min(m_fMinScalar, sc5);
      m_fMinScalar = Min(m_fMinScalar, sc6);
      m_fMinScalar = Min(m_fMinScalar, sc7);

      m_fMaxScalar = Max(m_fMaxScalar, sc0);
      m_fMaxScalar = Max(m_fMaxScalar, sc1);
      m_fMaxScalar = Max(m_fMaxScalar, sc2);
      m_fMaxScalar = Max(m_fMaxScalar, sc3);
      m_fMaxScalar = Max(m_fMaxScalar, sc4);
      m_fMaxScalar = Max(m_fMaxScalar, sc5);
      m_fMaxScalar = Max(m_fMaxScalar, sc6);
      m_fMaxScalar = Max(m_fMaxScalar, sc7);

      this->registerVoxel( mobius::cascade::GetOpenCascadePnt( pMobNode->GetP0() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP1() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP2() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP3() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP4() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP5() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP6() ),
                           mobius::cascade::GetOpenCascadePnt( pMobNode->GetP7() ),
                           sc0, sc1, sc2, sc3, sc4, sc5, sc6, sc7,
                           pData );
    }
  }
  else
    for ( size_t k = 0; k < 8; ++k )
    {
      this->addVoxels( pMobNode->GetChild(k), pData );
    }
#else
  vtkErrorMacro( << "Mobius SVO data structure is not available." );
#endif
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_OctreeSource::registerVoxel(const gp_Pnt&        node0,
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
                                      vtkUnstructuredGrid* pData)
{
  std::vector<vtkIdType> pids =
  {
    this->addPoint(node0, pData),
    this->addPoint(node1, pData),
    this->addPoint(node2, pData),
    this->addPoint(node3, pData),
    //
    this->addPoint(node4, pData),
    this->addPoint(node5, pData),
    this->addPoint(node6, pData),
    this->addPoint(node7, pData)
  };

  // Set scalars.
  vtkDoubleArray*
    pScalarsArr = vtkDoubleArray::SafeDownCast( pData->GetPointData()->GetArray(ARRNAME_VOXEL_N_SCALARS) );
  //
  pScalarsArr->InsertTypedTuple(pids[0], &sc0);
  pScalarsArr->InsertTypedTuple(pids[1], &sc1);
  pScalarsArr->InsertTypedTuple(pids[2], &sc2);
  pScalarsArr->InsertTypedTuple(pids[3], &sc3);
  pScalarsArr->InsertTypedTuple(pids[4], &sc4);
  pScalarsArr->InsertTypedTuple(pids[5], &sc5);
  pScalarsArr->InsertTypedTuple(pids[6], &sc6);
  pScalarsArr->InsertTypedTuple(pids[7], &sc7);

  // Register voxel cell.
  vtkIdType cellID = pData->InsertNextCell(VTK_VOXEL, 8, &pids[0]);

  return cellID;
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_OctreeSource::addPoint(const gp_Pnt&        coords,
                                 vtkUnstructuredGrid* pData)
{
  // Access points array.
  vtkPoints*
    points = pData->GetPoints();

  // Add the point to the VTK data set.
  vtkIdType
    resPid = points->InsertNextPoint( coords.X(), coords.Y(), coords.Z() );

  return resPid;
}
