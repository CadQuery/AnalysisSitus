//-----------------------------------------------------------------------------
// Created on: 06 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiAlgo_SmallEdges.h>

// OCCT includes
#include <ShapeFix_Wireframe.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_SmallEdges::asiAlgo_SmallEdges(const TopoDS_Shape&  masterCAD,
                                       ActAPI_ProgressEntry progress,
                                       ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_input   = masterCAD;
  m_reShape = new ShapeBuild_ReShape();
}

//-----------------------------------------------------------------------------

bool asiAlgo_SmallEdges::Perform(const double len,
                                 const double ang_deg)
{
  //ShapeFix_Wireframe healer(m_input);
  //healer.SetPrecision(len);
  //healer.SetLimitAngle(ang_deg); // Feature angle.
  //healer.SetContext(m_reShape);
  ////
  //healer.FixSmallEdges();

  //// Set output.
  //m_output = healer.Shape();

  ShapeUpgrade_UnifySameDomain unify(m_input, true, false, true);
  unify.SetLinearTolerance(len);
  unify.SetAngularTolerance(ang_deg/180.0*M_PI);
  unify.Build();

  // Set output.
  m_output = unify.Shape();

  return true; // Success.
}
