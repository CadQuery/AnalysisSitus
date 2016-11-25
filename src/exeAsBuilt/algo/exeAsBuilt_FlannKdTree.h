//-----------------------------------------------------------------------------
// Created on: 24 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_FlannKdTree_h
#define exeAsBuilt_FlannKdTree_h

// asiAlgo includes
#include <asiAlgo.h>

// Qr includes
#include <QrGeom3D_PositionCloud.h>

// FLANN includes
#include <flann/flann.h>

// OCCT includes
#include <Standard_Type.hxx>

//! Shared structure representing a k-d tree for a three-dimensional point
//! cloud. This class is the "experience box" for FLANN.
class exeAsBuilt_FlannKdTree : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(exeAsBuilt_FlannKdTree, Standard_Transient)

public:

  exeAsBuilt_FlannKdTree(const QrPtr<pcloud>& pointCloud);

protected:

  //! Working point cloud.
  QrPtr<pcloud> m_pointCloud;

  //! FLANN parameters.
  FLANNParameters m_flannParams;

  //! Index is a structure containing k-d tree.
  flann_index_t m_flannIndex;

};

#endif
