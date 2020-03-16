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

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_ProjectPointOnMesh.h>

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
  m_pFacets                    ( nullptr    ),
  m_pOctree                    ( nullptr    ),
  m_fMinScalar                 ( DBL_MAX    ),
  m_fMaxScalar                 (-DBL_MAX    ),
  m_bExtractPoints             ( false      ),
  m_strategy                   ( SS_OnInOut ) // all
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline.
}

//-----------------------------------------------------------------------------

asiVisu_OctreeSource::~asiVisu_OctreeSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_OctreeSource::SetInputFacets(asiAlgo_BVHFacets* pFacets)
{
  m_pFacets = pFacets;
  //
  this->Modified();
}

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

void asiVisu_OctreeSource::SetExtractPoints(const bool isOn)
{
  m_bExtractPoints = isOn;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

bool asiVisu_OctreeSource::IsExtractPoints() const
{
  return m_bExtractPoints;
}

//-----------------------------------------------------------------------------

void asiVisu_OctreeSource::SetSamplingStrategy(const int strategy)
{
  m_strategy = strategy;
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

  m_fMinScalar =  DBL_MAX;
  m_fMaxScalar = -DBL_MAX;

  /* ================
   *  Prepare output.
   * ================ */

  // Get the output unstructured grid data from the information vector.
  vtkUnstructuredGrid* pOutputGrid = vtkUnstructuredGrid::GetData(outputVector);
  pOutputGrid->Allocate();
  pOutputGrid->SetPoints( vtkSmartPointer<vtkPoints>::New() );

  /* ================================
   *  Add cells (vertices or voxels).
   * ================================ */

  if ( m_bExtractPoints )
  {
    if ( m_pFacets == nullptr )
    {
      vtkWarningMacro( << "No BVH: projection is impossible." );
    }

    // Allocate an array for vectors
    vtkSmartPointer<vtkDoubleArray>
      vectorsArr = asiVisu_Utils::InitDoubleVectorArray(ARRNAME_POINTCLOUD_VECTORS);
    //
    pOutputGrid->GetPointData()->SetVectors(vectorsArr);

    // Allocate an array of scalars.
    vtkSmartPointer<vtkDoubleArray> scalarsArr = asiVisu_Utils::InitDoubleArray(ARRNAME_VOXEL_N_SCALARS);
    //
    pOutputGrid->GetPointData()->SetScalars(scalarsArr);

    // Prepare the projection tool.
    asiAlgo_ProjectPointOnMesh projection(m_pFacets);

    // Gather points and normals.
    m_points  = new asiAlgo_BaseCloud<double>;
    m_vectors = new asiAlgo_BaseCloud<double>;
    std::vector<double> scalars;
    //
    this->samplePoints( m_pOctree, &projection, m_points.get(), m_vectors.get(), scalars );

    // Add cells.
    for ( int e = 0; e < m_points->GetNumberOfElements(); ++e )
    {
      this->registerVertexWithNormAndScalar(m_points->GetElement(e),
                                            m_vectors->GetElement(e),
                                            scalars[e],
                                            pOutputGrid);
    }
  }
  else
  {
    // Prepare array for nodal scalars.
    vtkPointData*                   pPointData = pOutputGrid->GetPointData();
    vtkSmartPointer<vtkDoubleArray> scalarsArr = asiVisu_Utils::InitDoubleArray(ARRNAME_VOXEL_N_SCALARS);
    //
    pPointData->SetScalars(scalarsArr);

    // Add cells.
    this->addVoxels(m_pOctree, pOutputGrid);
  }

  return 1;
}

//-----------------------------------------------------------------------------

void asiVisu_OctreeSource::samplePoints(void*                       pNode,
                                        asiAlgo_ProjectPointOnMesh* pProj,
                                        asiAlgo_BaseCloud<double>*  pPts,
                                        asiAlgo_BaseCloud<double>*  pNorms,
                                        std::vector<double>&        scalars)
{
  if ( pNode == nullptr || pProj == nullptr )
    return;

#if defined USE_MOBIUS
  mobius::poly_SVO* pMobNode = static_cast<mobius::poly_SVO*>(pNode);

  if ( pMobNode->IsLeaf() ) /* Only leaves are sampled */
  {
    const bool isOn  = mobius::poly_DistanceField::IsZeroCrossing(pMobNode);
    const bool isIn  = mobius::poly_DistanceField::IsIn(pMobNode);
    const bool isOut = mobius::poly_DistanceField::IsOut(pMobNode);

    if ( isOn  && (m_strategy & SS_On) ||
         isIn  && (m_strategy & SS_In) ||
         isOut && (m_strategy & SS_Out) )
    {
      // Get center point.
      gp_XYZ
        point = 0.125 * ( mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP0() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP1() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP2() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP3() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP4() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP5() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP6() )
                        + mobius::cascade::GetOpenCascadeXYZ( pMobNode->GetP7() ) );
      //
      gp_XYZ norm;

      if ( isOn )
      {
        // Project.
        point = pProj->Perform(point).XYZ();

        // Get the norm vector to store with the point.
        const int facetInd = pProj->GetFacetIds().size() ? pProj->GetFacetIds()[0] : -1;
        //
        if ( facetInd != -1 )
        {
          const asiAlgo_BVHFacets::t_facet& facet = pProj->GetBVH()->GetFacet(facetInd);

          // Set norm vector to be stored with the point cloud.
          norm = facet.N.XYZ();
        }
      }

      // Evaluate SVO node.
      const double sc = ( isOn ? 0. : pMobNode->Eval( mobius::cascade::GetMobiusPnt(point) ) );
      //
      m_fMinScalar = Min(m_fMinScalar, sc);
      m_fMaxScalar = Max(m_fMaxScalar, sc);

      // Populate outputs.
      pPts->AddElement(point); // Add point.
      pNorms->AddElement(norm); // Add normal vector.
      scalars.push_back(sc); // Add scalar.
    }
  }
  else
  {
    for ( size_t k = 0; k < 8; ++k )
    {
      this->samplePoints(pMobNode->GetChild(k), pProj, pPts, pNorms, scalars);
    }
  }
#else
  vtkErrorMacro( << "Mobius SVO data structure is not available." );
#endif
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
    const bool isOn  = mobius::poly_DistanceField::IsZeroCrossing(pMobNode);
    const bool isIn  = mobius::poly_DistanceField::IsIn(pMobNode);
    const bool isOut = mobius::poly_DistanceField::IsOut(pMobNode);

    if ( isOn  && (m_strategy & SS_On) ||
         isIn  && (m_strategy & SS_In) ||
         isOut && (m_strategy & SS_Out) )
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
  {
    for ( size_t k = 0; k < 8; ++k )
    {
      this->addVoxels( pMobNode->GetChild(k), pData );
    }
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

vtkIdType asiVisu_OctreeSource::registerVertex(const gp_Pnt&        point,
                                               vtkUnstructuredGrid* pData)
{
  std::vector<vtkIdType> pids =
  {
    this->addPoint(point, pData),
  };

  // Register vertex cell.
  vtkIdType cellID = pData->InsertNextCell(VTK_VERTEX, 1, &pids[0]);

  return cellID;
}

//-----------------------------------------------------------------------------

vtkIdType
    asiVisu_OctreeSource::registerVertexWithNormAndScalar(const gp_Pnt&        point,
                                                          const gp_Vec&        norm,
                                                          const double         scalar,
                                                          vtkUnstructuredGrid* pData)
{
  // Get array of vectors.
  vtkDoubleArray*
    pVectorsArr = vtkDoubleArray::SafeDownCast( pData->GetPointData()->GetArray(ARRNAME_POINTCLOUD_VECTORS) );

  // Get array of scalars.
  vtkDoubleArray*
    pScalarsArr = vtkDoubleArray::SafeDownCast( pData->GetPointData()->GetArray(ARRNAME_VOXEL_N_SCALARS) );

  std::vector<vtkIdType> pids =
  {
    this->addPoint(point, pData),
  };

  // Register vertex cell.
  vtkIdType cellID = pData->InsertNextCell(VTK_VERTEX, 1, &pids[0]);

  // Add vector to the array.
  const double normCoords[3] = { norm.X(), norm.Y(), norm.Z() };
  pVectorsArr->InsertTypedTuple(cellID, normCoords);

  // Add scalar to the array.
  pScalarsArr->InsertTypedTuple(cellID, &scalar);

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
