//-----------------------------------------------------------------------------
// Created on: 26 January 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_ReorientNorms_h
#define asiAlgo_ReorientNorms_h

// asiAlgo includes
#include <asiAlgo_PointCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <gp_Pln.hxx>

//-----------------------------------------------------------------------------

//! Utility to reorient normal field with respect to a sample vector.
class asiAlgo_ReorientNorms : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ReorientNorms, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_ReorientNorms(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Perform(const asiAlgo_PointCloud<float>& input,
            const int                        refNormIndex,
            asiAlgo_PointCloud<float>&       output);

};

#endif
