//-----------------------------------------------------------------------------
// Created on: 26 January 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
