//-----------------------------------------------------------------------------
// Created on: 25 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_DihedralAngle_h
#define asiAlgo_DihedralAngle_h

// asiAlgo includes
#include <asiAlgo_FeatureAngle.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_DihedralAngle, ActAPI_IAlgorithm)

//! Utility to analyze dihedral angles.
class asiAlgo_DihedralAngle : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DihedralAngle, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_DihedralAngle(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT asiAlgo_FeatureAngle
    AngleBetweenFaces(const TopoDS_Face&          F,
                      const TopoDS_Face&          G,
                      const bool                  allowSmooth,
                      TopTools_IndexedMapOfShape& commonEdges) const;

public:

  //! Sets a priori known common edge.
  //! \param[in] commonEdge common edge.
  void SetCommonEdge(const TopoDS_Edge& commonEdge)
  {
    m_commonEdge = commonEdge;
  }

protected:

  TopoDS_Edge m_commonEdge; //!< Optional a priori known common edge.

};

#endif
