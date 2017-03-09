//-----------------------------------------------------------------------------
// Created on: 09 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PlateOnEdges_h
#define asiAlgo_PlateOnEdges_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineSurface.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a plate surface on the given edge set.
class asiAlgo_PlateOnEdges : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PlateOnEdges, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_PlateOnEdges(const Handle(asiAlgo_AAG)& aag,
                         ActAPI_ProgressEntry       progress,
                         ActAPI_PlotterEntry        plotter);

public:

  asiAlgo_EXPORT bool
    Build(const TColStd_PackedMapOfInteger& edgeIndices,
          Handle(Geom_BSplineSurface)&      support,
          TopoDS_Face&                      result);

protected:

  Handle(asiAlgo_AAG) m_aag; //!< AAG.

};

#endif
