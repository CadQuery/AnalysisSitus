//-----------------------------------------------------------------------------
// Created on: 24 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_FlannKdTree.h>

//! ctor.
//! \param pointCloud [in] input point cloud to build a k-d tree for.
exeAsBuilt_FlannKdTree::exeAsBuilt_FlannKdTree(const Handle(asiAlgo_PointCloud<double>)& pointCloud)
: Standard_Transient(), m_pointCloud(pointCloud)
{
  const int numPts = m_pointCloud->GetNumberOfPoints();
  double*  pCoords = &m_pointCloud->ChangeCoords()[0];

  // Initialize FLANN parameters
  m_flannParams           = DEFAULT_FLANN_PARAMETERS;
  m_flannParams.algorithm = FLANN_INDEX_KDTREE;
  m_flannParams.trees     = 1;
  m_flannParams.log_level = FLANN_LOG_INFO;
  m_flannParams.checks    = 64;

  // Build index
  float speedup;
  //m_flannIndex = flann_build_index(pCoords, numPts, 3, &speedup, &m_flannParams)

}
