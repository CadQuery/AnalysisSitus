//-----------------------------------------------------------------------------
// Created on: 18 December 2018
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
//    * Neither the name of the copyright holder(s) nor the
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

// cmdMisc includes
#include <cmdMisc.h>

// asiAlgo includes
#include <asiAlgo_BuildCoonsSurf.h>

// asiData includes
#include <asiData_IVCurveNode.h>

#ifdef USE_MOBIUS
  #include <mobius/cascade.h>
  #include <mobius/geom_CoonsSurfaceLinear.h>

  using namespace mobius;

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

Handle(Geom_BSplineCurve) FindBCurve(const TCollection_AsciiString& curveName,
                                     const Handle(asiTcl_Interp)&   interp)
{
  // Find Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(curveName);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << curveName);
    return NULL;
  }
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

  // Get B-curve.
  double f, l;
  Handle(Geom_BSplineCurve)
    occtBCurve = Handle(Geom_BSplineCurve)::DownCast( curveNode->GetCurve(f, l) );
  //
  if ( occtBCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is not a B-spline curve.");
    return NULL;
  }

  return occtBCurve;
}

//-----------------------------------------------------------------------------

int MISC_BuildCoonsLinear(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get rail curves.
  Handle(Geom_BSplineCurve) c0 = FindBCurve(argv[2], interp);
  Handle(Geom_BSplineCurve) c1 = FindBCurve(argv[3], interp);
  Handle(Geom_BSplineCurve) b0 = FindBCurve(argv[4], interp);
  Handle(Geom_BSplineCurve) b1 = FindBCurve(argv[5], interp);

  // Build Coons surface by points.
  {
    t_ptr<t_bcurve> mobC0 = cascade::GetMobiusBCurve(c0);
    t_ptr<t_bcurve> mobC1 = cascade::GetMobiusBCurve(c1);
    t_ptr<t_bcurve> mobB0 = cascade::GetMobiusBCurve(b0);
    t_ptr<t_bcurve> mobB1 = cascade::GetMobiusBCurve(b1);

    t_ptr<geom_CoonsSurfaceLinear>
      coons = new geom_CoonsSurfaceLinear( mobC0, mobC1, mobB0, mobB1,
                                           mobC0->D0( mobC0->GetMinParameter() ),
                                           mobC1->D0( mobC1->GetMinParameter() ),
                                           mobC0->D0( mobC0->GetMaxParameter() ),
                                           mobC1->D0( mobC1->GetMaxParameter() ) );

    // Sample patch.
    TCollection_AsciiString ptsName(argv[1]); ptsName += "-pts";
    cmdMisc::DrawSurfPts(interp, coons, ptsName);
  }

  // Build Coons patch.
  asiAlgo_BuildCoonsSurf buildCoons( c0, c1, b0, b1,
                                     interp->GetProgress(),
                                     interp->GetPlotter() );
  //
  if ( !buildCoons.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Operation failed.");
    return TCL_ERROR;
  }

  // Draw result.
  interp->GetPlotter().REDRAW_SURFACE(argv[1],
                                      buildCoons.GetResult(),
                                      Color_Default);

  return TCL_OK;
}

#endif

//-----------------------------------------------------------------------------

void cmdMisc::Commands_Coons(const Handle(asiTcl_Interp)&      interp,
                             const Handle(Standard_Transient)& data)
{
  cmdMisc_NotUsed(data);

  static const char* group = "cmdMisc";

#if defined USE_MOBIUS

  //-------------------------------------------------------------------------//
  interp->AddCommand("build-coons-linear",
    //
    "build-coons-linear resSurf c0 c1 b0 b1\n"
    "\t Builds bilinear Coons patch on the passed boundary curves.",
    //
    __FILE__, group, MISC_BuildCoonsLinear);

#endif
}
