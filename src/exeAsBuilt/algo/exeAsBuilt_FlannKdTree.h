//-----------------------------------------------------------------------------
// Created on: 24 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_FlannKdTree_h
#define exeAsBuilt_FlannKdTree_h

// asiAlgo includes
#include <asiAlgo_PointCloud.h>

// FLANN includes
#include <flann/flann.h>

// OCCT includes
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>

DEFINE_STANDARD_HANDLE(exeAsBuilt_FlannKdTree, Standard_Transient)

//! Shared structure representing a k-d tree for a three-dimensional point
//! cloud. This class is the "experience box" for FLANN.
class exeAsBuilt_FlannKdTree : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exeAsBuilt_FlannKdTree, Standard_Transient)

public:

  exeAsBuilt_FlannKdTree(const Handle(asiAlgo_PointCloud<float>)& pointCloud);

  ~exeAsBuilt_FlannKdTree();

public:

  void Search(const gp_Pnt&       P,
              const int           k,
              std::vector<int>&   nearestIndices,
              std::vector<float>& nearestDistances);

protected:

  //! Working point cloud.
  Handle(asiAlgo_PointCloud<float>) m_pointCloud;

  //! FLANN parameters.
  FLANNParameters m_flannParams;

  //! Index is a structure containing k-d tree.
  flann_index_t m_flannIndex;

};

#endif
