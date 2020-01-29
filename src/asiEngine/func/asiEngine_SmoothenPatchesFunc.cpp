//-----------------------------------------------------------------------------
// Created on: 13 August 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

// Own include
#include <asiEngine_SmoothenPatchesFunc.h>

// asiEngine includes
#include <asiEngine_PatchJointAdaptor.h>
#include <asiEngine_RE.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

Handle(asiEngine_SmoothenPatchesFunc) asiEngine_SmoothenPatchesFunc::Instance()
{
  return new asiEngine_SmoothenPatchesFunc();
}

//-----------------------------------------------------------------------------

const char* asiEngine_SmoothenPatchesFunc::GUID()
{
  return "1912E0B8-02C5-42CC-B6FE-D89F3096E38B";
}

//-----------------------------------------------------------------------------

const char* asiEngine_SmoothenPatchesFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

int asiEngine_SmoothenPatchesFunc::execute(const Handle(ActAPI_HParameterList)& inputs,
                                           const Handle(ActAPI_HParameterList)& asiEngine_NotUsed(outputs),
                                           const Handle(Standard_Transient)&    asiEngine_NotUsed(userData)) const
{
  // Get transition edge.
  Handle(asiData_ReEdgeNode)
    edgeNode = Handle(asiData_ReEdgeNode)::DownCast( inputs->Value(1)->GetNode() );
  //
  Handle(Geom_BSplineCurve)
    edgeCurve = Handle(Geom_BSplineCurve)::DownCast( edgeNode->GetCurve() );

  m_progress.SendLogMessage( LogNotice(Normal) << "Executing tree function '%1%2'..."
                                               << this->GetName()
                                               << edgeNode->RootLabel().Tag() );

  // Get left patch.
  Handle(asiData_RePatchNode)
    leftPatchNode = Handle(asiData_RePatchNode)::DownCast( inputs->Value(2)->GetNode() );
  //
  Handle(Geom_BSplineSurface)
    leftPatchSurf = Handle(Geom_BSplineSurface)::DownCast( leftPatchNode->GetSurface() );

  // Get right patch.
  Handle(asiData_RePatchNode)
    rightPatchNode = Handle(asiData_RePatchNode)::DownCast( inputs->Value(3)->GetNode() );
  //
  Handle(Geom_BSplineSurface)
    rightPatchSurf = Handle(Geom_BSplineSurface)::DownCast( rightPatchNode->GetSurface() );

  // Prepare joint adaptor.
  asiAlgo_PatchJointAdaptor jointAdt(edgeCurve, leftPatchSurf, rightPatchSurf,
                                     m_progress/*, m_plotter*/);

  // Extract isos by performing pure geometric analysis.
  Handle(Geom_BSplineCurve) isoLeft, isoRight;
  bool                      isoLeftU, isoRightU, isoLeftMin, isoRightMin, areOpposite;
  //
  if ( !jointAdt.ExtractIsos(isoLeft, isoLeftU, isoLeftMin,
                             isoRight, isoRightU, isoRightMin,
                             areOpposite) )
    return 1; // Failure.

  //m_plotter.REDRAW_CURVE("isoLeft",  isoLeft,  Color_White);
  //m_plotter.REDRAW_CURVE("isoRight", isoRight, Color_White);

  // Make surfaces compatible.
  if ( !jointAdt.UnifySurfaces(isoLeft, isoLeftU, isoRight, isoRightU, areOpposite) )
    return 1; // Failure.
  //
  if ( !jointAdt.ExtractIsos(isoLeft, isoLeftU, isoLeftMin,
                             isoRight, isoRightU, isoRightMin,
                             areOpposite) )
    return 1; // Failure.

  // Align control poles.
  if ( !jointAdt.AlignControlPoles(isoLeft, isoLeftU, isoLeftMin,
                                   isoRight, isoRightU, isoRightMin,
                                   areOpposite) )
    return 1; // Failure.

  // Store the updated surfaces.
  leftPatchNode->SetSurface( jointAdt.GetSurfaceLeft() );
  rightPatchNode->SetSurface( jointAdt.GetSurfaceRight() );

  return 0; // Success.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_SmoothenPatchesFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape  // Transition edge.
                                      << Parameter_Shape  // Left patch.
                                      << Parameter_Shape; // Right patch.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_SmoothenPatchesFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream();
}
