//-----------------------------------------------------------------------------
// Created on: 26 January 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
bool asiAlgo_ReorientNorms::Perform(const asiAlgo_BaseCloud<float>& input,
                                    const int                       refNormIndex,
                                    asiAlgo_BaseCloud<float>&       output)
{
  float refx, refy, refz;
  input.GetElement(refNormIndex, refx, refy, refz);
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
