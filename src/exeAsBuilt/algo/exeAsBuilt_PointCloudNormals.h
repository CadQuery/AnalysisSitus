//-----------------------------------------------------------------------------
// Created on: 05 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_PointCloudNormals_h
#define exeAsBuilt_PointCloudNormals_h

// A-Situs includes
#include <asiAlgo_PointCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

class exeAsBuilt_FlannKdTree;

//-----------------------------------------------------------------------------

//! \todo make this class template.
//!
//! Utility to build normal vectors for all points in the given
//! point cloud.
class exeAsBuilt_PointCloudNormals : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exeAsBuilt_PointCloudNormals, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    exeAsBuilt_PointCloudNormals(ActAPI_ProgressEntry progress,
                                 ActAPI_PlotterEntry  plotter);

  asiAlgo_EXPORT
    exeAsBuilt_PointCloudNormals(const Handle(exeAsBuilt_FlannKdTree)& kdTree,
                                 ActAPI_ProgressEntry                  progress,
                                 ActAPI_PlotterEntry                   plotter);

public:

  asiAlgo_EXPORT void
    Init(const Handle(exeAsBuilt_FlannKdTree)& kdTree);

  asiAlgo_EXPORT void
    Perform();

  asiAlgo_EXPORT bool
    IsDone() const;

  asiAlgo_EXPORT const Handle(asiAlgo_PointCloud<float>)&
    GetResult() const;

protected:

  //! Sets execution status.
  //! \param isDone [in] value to set.
  void setDone(const bool isDone) { m_bDone = isDone; }

protected:

  //! Computed normal vectors.
  Handle(asiAlgo_PointCloud<float>) m_normals;

  //! K-d tree for the working point cloud.
  Handle(exeAsBuilt_FlannKdTree) m_kdTree;

  //! Execution status.
  bool m_bDone;

};

#endif
