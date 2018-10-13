//-----------------------------------------------------------------------------
// Created on: 21 July 2016
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
#include <asiAlgo_EulerPoincare.h>

// Geometry includes
#include <asiAlgo_Utils.h>

//! Checks the passed shape against Euler-Poincare formula. Notice that
//! genus of the manifold has to be selected by user as we have no idea
//! how to perform this tricky genus analysis.
//! \param[in] shape    shape to check.
//! \param[in] genus    genus of the manifold.
//! \param[in] progress progress notifier.
//! \return true if the check has passed, false -- otherwise.
bool asiAlgo_EulerPoincare::Check(const TopoDS_Shape&  shape,
                                  const int            genus,
                                  ActAPI_ProgressEntry progress)
{
  // Summary
  int nbCompsolids = 0,
      nbCompounds  = 0,
      nbSolids     = 0,
      nbShells     = 0,
      nbFaces      = 0,
      nbWires      = 0,
      nbOuterWires = 0,
      nbInnerWires = 0,
      nbEdges      = 0,
      nbDegenEdges = 0,
      nbVertexes   = 0;

  // Extract summary
  asiAlgo_Utils::ShapeSummary(shape,
                              nbCompsolids,
                              nbCompounds,
                              nbSolids,
                              nbShells,
                              nbFaces,
                              nbWires,
                              nbOuterWires,
                              nbInnerWires,
                              nbEdges,
                              nbDegenEdges,
                              nbVertexes);

  const int v = nbVertexes;
  const int e = nbEdges - nbDegenEdges;
  const int f = nbFaces;
  const int s = nbShells;
  const int h = genus;
  const int r = nbInnerWires;

  const int res = v - e + f - 2*(s - h) - r;

  progress.SendLogMessage(LogInfo(Normal) << "V = %1." << v);
  progress.SendLogMessage(LogInfo(Normal) << "E = %1." << e);
  progress.SendLogMessage(LogInfo(Normal) << "F = %1." << f);
  progress.SendLogMessage(LogInfo(Normal) << "R = %1." << r);
  progress.SendLogMessage(LogInfo(Normal) << "S = %1." << s);
  progress.SendLogMessage(LogInfo(Normal) << "H = %1." << h);
  progress.SendLogMessage(LogInfo(Normal) << "V - E + F - R - 2*(S - H) = %1." << res);

  return (res == 0);
}
