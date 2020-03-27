//-----------------------------------------------------------------------------
// Created on: 08 August 2019
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
#include <asiEngine_BuildEdgeFunc.h>

// asiEngine includes
#include <asiEngine_RE.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

#if defined USE_MOBIUS
  // Mobius includes
  #include <mobius/cascade.h>
  #include <mobius/geom_FairBCurve.h>

  using namespace mobius;
#endif

//-----------------------------------------------------------------------------

#if defined USE_MOBIUS

Handle(Geom_BSplineCurve) FairCurve(const Handle(Geom_BSplineCurve)& curve,
                                    const double                     lambda,
                                    ActAPI_ProgressEntry             progress = nullptr)
{
  // Convert to Mobius curve.
  t_ptr<t_bcurve> mobCurve = cascade::GetMobiusBCurve(curve);

  // Perform fairing from Mobius.
  geom_FairBCurve fairing(mobCurve, lambda, nullptr, nullptr);
  //
  if ( !fairing.Perform() )
  {
    progress.SendLogMessage(LogErr(Normal) << "Fairing failed.");
    return nullptr;
  }

  // Get the faired curve.
  const t_ptr<t_bcurve>& mobResult = fairing.GetResult();

  // Convert to OpenCascade curve.
  Handle(Geom_BSplineCurve) result = cascade::GetOpenCascadeBCurve(mobResult);

  return result;
}

#endif

//-----------------------------------------------------------------------------

Handle(asiEngine_BuildEdgeFunc) asiEngine_BuildEdgeFunc::Instance()
{
  return new asiEngine_BuildEdgeFunc();
}

//-----------------------------------------------------------------------------

const char* asiEngine_BuildEdgeFunc::GUID()
{
  return "C2F0BB66-1155-440C-A24A-893599FD9383";
}

//-----------------------------------------------------------------------------

const char* asiEngine_BuildEdgeFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

int asiEngine_BuildEdgeFunc::execute(const Handle(ActAPI_HParameterList)& inputs,
                                     const Handle(ActAPI_HParameterList)& asiEngine_NotUsed(outputs),
                                     const Handle(Standard_Transient)&    asiEngine_NotUsed(userData)) const
{
  // Get Edge Node.
  Handle(asiData_ReEdgeNode)
    edgeNode = Handle(asiData_ReEdgeNode)::DownCast( inputs->Value(1)->GetNode() );

  // Fairing props.
  const bool   toFair = edgeNode->IsFairCurve();
  const double lambda = edgeNode->GetFairingCoeff();

  m_progress.SendLogMessage( LogNotice(Normal) << "Executing tree function '%1%2'..."
                                               << this->GetName()
                                               << edgeNode->RootLabel().Tag() );

  // Approximate with parametric curve.
  std::vector<gp_XYZ> pts;
  edgeNode->GetPolyline(pts);
  //
  Handle(Geom_BSplineCurve) curve;
  if ( !asiAlgo_Utils::ApproximatePoints(pts, 3, 3, edgeNode->GetApproxToler(), curve) )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Cannot approximate edge '%1'."
                                              << edgeNode->GetName() );
    return 1; // Error.
  }

  // Fair curve.
  if ( toFair )
  {
    Handle(Geom_BSplineCurve) fairedBCurve = FairCurve( curve,
                                                        lambda );
    //
    if ( fairedBCurve.IsNull() )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Fairing failed for edge '%1'."
                                                << edgeNode->GetName() );
      return 1; // Error.
    }

    curve = fairedBCurve;
  }

  // Set the result.
  edgeNode->SetCurve(curve);

  return 0; // Success.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream asiEngine_BuildEdgeFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_RealArray // Polyline.
                                      << Parameter_Real      // Tolerance.
                                      << Parameter_Bool      // Whether to fair curve.
                                      << Parameter_Real      // Fairing coefficent.
                                      ;
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream asiEngine_BuildEdgeFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape // Approximated curve.
                                      ;
}
