//-----------------------------------------------------------------------------
// Created on: 26 January 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_ReorientNorms.h>

//-----------------------------------------------------------------------------

//! ctor.
//! \param progress [in] progress notifier.
//! \param plotter  [in] imperative plotter.
asiAlgo_ReorientNorms::asiAlgo_ReorientNorms(ActAPI_ProgressEntry progress,
                                             ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

//! Performs reorientation of normals with respect to the reference normal
//! vector.
//! \param input        [in]  input normal field.
//! \param refNormIndex [in]  index of the reference normal.
//! \param output       [out] output normal field.
bool asiAlgo_ReorientNorms::Perform(const asiAlgo_PointCloud<float>& input,
                                    const int                        refNormIndex,
                                    asiAlgo_PointCloud<float>&       output)
{
  float refx, refy, refz;
  input.GetPoint(refNormIndex, refx, refy, refz);
  gp_Vec refNorm(refx, refy, refz);

  // Prepare a deep copy (TODO: this may be improved in future)
  input.CopyTo(output);

  // Decide for each single normal whether to invert it
  //for ( int i = 0; i < output.GetNumberOfPoints(); ++i )
  //{
  //  if ( i == refNormIndex )
  //    continue;

  //  float nx, ny, nz;
  //  output.GetPoint(i, nx, ny, nz);
  //  gp_Vec norm(nx, ny, nz);
  //  //
  //  if ( refNorm.Dot(norm) < 0 )
  //    output.SetPoint(i, -nx, -ny, -nz);
  //}

  return true;
}
