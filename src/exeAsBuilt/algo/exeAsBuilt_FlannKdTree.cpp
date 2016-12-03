//-----------------------------------------------------------------------------
// Created on: 24 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_FlannKdTree.h>

//-----------------------------------------------------------------------------

//! ctor.
//! \param pointCloud [in] input point cloud to build a k-d tree for.
exeAsBuilt_FlannKdTree::exeAsBuilt_FlannKdTree(const Handle(asiAlgo_PointCloud<float>)& pointCloud)
: Standard_Transient(), m_pointCloud(pointCloud)
{
  const int numPts = m_pointCloud->GetNumberOfPoints();
  float*   pCoords = &m_pointCloud->ChangeCoords()[0];

  // Initialize FLANN parameters
  m_flannParams           = DEFAULT_FLANN_PARAMETERS;
  m_flannParams.algorithm = FLANN_INDEX_KDTREE;
  m_flannParams.trees     = 1;
  m_flannParams.log_level = FLANN_LOG_INFO;
  m_flannParams.checks    = 64;

  // Build index
  float speedup;
  m_flannIndex = flann_build_index(pCoords, numPts, 3, &speedup, &m_flannParams);
}

//-----------------------------------------------------------------------------

//! Destructor.
exeAsBuilt_FlannKdTree::~exeAsBuilt_FlannKdTree()
{
  flann_free_index(m_flannIndex, &m_flannParams);
}

//-----------------------------------------------------------------------------

//! Searches for the {k} nearest neighbors of the {queryPoint}.
//! \param P                [in]  point of interest.
//! \param k                [in]  number of nearest neighbors to return.
//! \param nearestIndices   [out] indices of the nearest points.
//! \param nearestDistances [out] distances to the nearest points.
void exeAsBuilt_FlannKdTree::Search(const gp_Pnt&       P,
                                    const int           k,
                                    std::vector<int>&   nearestIndices,
                                    std::vector<float>& nearestDistances)
{
  float queryCoords[3] = { P.X(), P.Y(), P.Z() };

  // Do ANN
  flann_find_nearest_neighbors_index(m_flannIndex,
                                     queryCoords,
                                     1,
                                     &nearestIndices[0],
                                     &nearestDistances[0],
                                     k,
                                     &m_flannParams);
}

//-----------------------------------------------------------------------------

//! \return point cloud.
const Handle(asiAlgo_PointCloud<float>)&
  exeAsBuilt_FlannKdTree::GetPointCloud() const
{
    return m_pointCloud;
}
