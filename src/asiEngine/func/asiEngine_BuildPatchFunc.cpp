//-----------------------------------------------------------------------------
// Created on: 09 July 2019
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
#include <asiEngine_BuildPatchFunc.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// asiEngine includes
#include <asiEngine_RE.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

#if defined USE_MOBIUS
  // Mobius includes
  #include <mobius/cascade.h>
  #include <mobius/geom_ApproxBSurf.h>
  #include <mobius/geom_BuildAveragePlane.h>
  #include <mobius/geom_InterpolateMultiCurve.h>
  #include <mobius/geom_FairBCurve.h>
  #include <mobius/geom_PlaneSurface.h>
  #include <mobius/geom_SkinSurface.h>

  using namespace mobius;
#endif

//-----------------------------------------------------------------------------

#define numArgsBeforeCoedges 4

//-----------------------------------------------------------------------------

Handle(asiEngine_BuildPatchFunc) asiEngine_BuildPatchFunc::Instance()
{
  return new asiEngine_BuildPatchFunc();
}

//-----------------------------------------------------------------------------

const char* asiEngine_BuildPatchFunc::GUID()
{
  return "A7E0D6DA-AC44-44D8-B9A0-01F0AF6F814B";
}

//-----------------------------------------------------------------------------

const char* asiEngine_BuildPatchFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

int asiEngine_BuildPatchFunc::execute(const Handle(ActAPI_HParameterList)& inputs,
                                      const Handle(ActAPI_HParameterList)& outputs,
                                      const Handle(Standard_Transient)&    userData) const
{
  /* ============================
   *  Interpret input Parameters
   * ============================ */

  // User data is a Data Model instance.
  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(userData);
  //
  if ( M.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "User data is not a Data Model instance.");
    return 1; // Error.
  }

  // Vector of coedges.
  std::vector<Handle(asiData_ReCoedgeNode)> coedges;

  // Get Patch Node.
  Handle(asiData_RePatchNode)
    patchNode = Handle(asiData_RePatchNode)::DownCast( outputs->First()->GetNode() );

  m_progress.SendLogMessage( LogNotice(Normal) << "Executing tree function '%1%2'..."
                                               << this->GetName()
                                               << patchNode->RootLabel().Tag() );

  // Get min number of knots.
  const int
    minNumKnots = Handle(ActData_IntParameter)::DownCast( inputs->Value(1) )->GetValue();

  // Get mesh approximation flag.
  const bool
    approxMesh = Handle(ActData_BoolParameter)::DownCast( inputs->Value(2) )->GetValue();

  // Get the value of fairing coefficient.
  const double
    approxLambda = Handle(ActData_RealParameter)::DownCast( inputs->Value(3) )->GetValue();

  // Boolean flag indicating if the visual debugging is enabled.
  m_bPlotterEnabled = Handle(ActData_BoolParameter)::DownCast( inputs->Value(4) )->GetValue();

  m_progress.SendLogMessage( LogInfo(Normal) << "Mesh nodes approximation mode: %1." << approxMesh );
  //
  if ( approxMesh )
    m_progress.SendLogMessage( LogInfo(Normal) << "Fairing coefficient: %1." << approxLambda );

  // Collect coedges.
  int       currIdx    = 1;
  const int numCoedges = (inputs->Length() + numArgsBeforeCoedges) / 2;
  //
  for ( ActAPI_HParameterList::Iterator it(*inputs); it.More(); it.Next(), ++currIdx )
  {
    // Skip min number of knots and other non-coedge inputs.
    if ( currIdx <= numArgsBeforeCoedges )
      continue;

    if ( currIdx > numCoedges + numArgsBeforeCoedges )
      break;

    const Handle(ActAPI_IUserParameter)& uParam = it.Value();
    //
    Handle(asiData_ReCoedgeNode)
      coedge = Handle(asiData_ReCoedgeNode)::DownCast( uParam->GetNode() );

    if ( !coedge.IsNull() )
      coedges.push_back(coedge);
    else
      break;
  }

  /* =============
   *  Build patch
   * ============= */

  asiEngine_RE reApi(M, m_progress, m_bPlotterEnabled ? m_plotter : NULL);

  // Fill Coons.
  Handle(Geom_BSplineSurface) bsurf;
  //
  if ( !reApi.FillPatchCoons(coedges, minNumKnots, bsurf) )
    return 1;

  /* =============================================================
   *  Extract inner mesh nodes if mesh approximation is requested
   * ============================================================= */

  // Extract inner nodes.
  Handle(asiAlgo_BaseCloud<double>) pts;
  //
  if ( approxMesh && !this->extractMeshNodes(M, patchNode, pts) )
    return 1; // Error.

  /* ==================================================
   *  Approximate points with a B-surface if requested
   * ================================================== */

  Handle(Geom_BSplineSurface) approxSurf;
  //
  if ( approxMesh && !this->approxMeshNodes(bsurf, pts, approxLambda, approxSurf) )
    return 1; // Error.

  if ( approxMesh )
    bsurf = approxSurf;

  /* =======================
   *  Set output Parameters
   * ======================= */

  // Set the surface to the output Parameter indirectly using the Patch Node.
  patchNode->SetSurface(bsurf);

  return 0; // Success.
}

//-----------------------------------------------------------------------------

bool
  asiEngine_BuildPatchFunc::validateInput(const Handle(ActAPI_HParameterList)& inputs) const
{
  // Check the number of arguments.
  const int numArgs = inputs->Length();
  //
  if ( numArgs % 2 != 0 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Number of arguments is unexpected (%1)." << numArgs);
    return false;
  }

  const int numCoedges = (numArgs - numArgsBeforeCoedges) / 2;
  int       currIdx    = 1;

  // Check Parameter types.
  for ( ActAPI_HParameterList::Iterator it(*inputs); it.More(); it.Next(), ++currIdx )
  {
    const Handle(ActAPI_IUserParameter)& uParam = it.Value();

    if ( currIdx == 1 )
    {
      // We expect that "Min number of knots" is connected first.
      if ( !uParam->IsKind( STANDARD_TYPE(ActData_IntParameter) ) )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Input Parameter %1 is not of the expected type %2."
                                                  << currIdx << STANDARD_TYPE(ActData_IntParameter)->Name() );
        return false;
      }
    }
    else if ( currIdx == 2 )
    {
      // We expect that "Approx. nodes" is connected then.
      if ( !uParam->IsKind( STANDARD_TYPE(ActData_BoolParameter) ) )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Input Parameter %1 is not of the expected type %2."
                                                  << currIdx << STANDARD_TYPE(ActData_BoolParameter)->Name() );
        return false;
      }
    }
    else if ( currIdx == 3 )
    {
      // We expect that "Approx. lambda" is connected then.
      if ( !uParam->IsKind( STANDARD_TYPE(ActData_RealParameter) ) )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Input Parameter %1 is not of the expected type %2."
                                                  << currIdx << STANDARD_TYPE(ActData_RealParameter)->Name() );
        return false;
      }
    }
    else if ( currIdx == 3 )
    {
      // We expect that "Enable plotter" is connected then.
      if ( !uParam->IsKind( STANDARD_TYPE(ActData_BoolParameter) ) )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Input Parameter %1 is not of the expected type %2."
                                                  << currIdx << STANDARD_TYPE(ActData_BoolParameter)->Name() );
        return false;
      }
    }
    else if ( currIdx <= numCoedges + numArgsBeforeCoedges )
    {
      // We expect that "SameSense" flags are connected then.
      if ( !uParam->IsKind( STANDARD_TYPE(ActData_BoolParameter) ) )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Input Parameter %1 is not of the expected type %2."
                                                  << currIdx << STANDARD_TYPE(ActData_BoolParameter)->Name() );
        return false;
      }
    }
    else
    {
      // We expect that curves are connected then.
      if ( !uParam->IsKind( STANDARD_TYPE(ActData_ShapeParameter) ) )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Input Parameter %1 is not of the expected type %2."
                                                  << currIdx << STANDARD_TYPE(ActData_ShapeParameter)->Name() );
        return false;
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_BuildPatchFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream();
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_BuildPatchFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape; // Surface patch.
}

//-----------------------------------------------------------------------------

bool asiEngine_BuildPatchFunc::extractMeshNodes(const Handle(asiEngine_Model)&     model,
                                                const Handle(asiData_RePatchNode)& patch,
                                                Handle(asiAlgo_BaseCloud<double>)& pts) const
{
  // Prepare service API.
  asiEngine_RE api(model, m_progress, m_bPlotterEnabled ? m_plotter : NULL);

  // Get triangles captured by contour.
  Handle(Poly_Triangulation) regionTris;
  //
  if ( !api.ExtractBoundedRegion(patch, regionTris) )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Cannot extract the region captured by contour of patch '%1'."
                                              << patch->GetId() );
    return false;
  }

  // Get nodes of the captured region.
  pts = new asiAlgo_BaseCloud<double>;
  //
  for ( int i = 1; i <= regionTris->NbNodes(); ++i )
  {
    const gp_Pnt& P = regionTris->Node(i);
    pts->AddElement( P.X(), P.Y(), P.Z() );
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiEngine_BuildPatchFunc::approxMeshNodes(const Handle(Geom_BSplineSurface)&       initSurf,
                                               const Handle(asiAlgo_BaseCloud<double>)& pts,
                                               const double                             lambda,
                                               Handle(Geom_BSplineSurface)&             resultSurf) const
{
#if defined USE_MOBIUS
  // Convert to Mobius point cloud.
  t_ptr<t_pcloud> mobPts = new t_pcloud;
  //
  for ( int k = 0; k < pts->GetNumberOfElements(); ++k )
    mobPts->AddPoint( cascade::GetMobiusPnt( pts->GetElement(k) ) );

  // Convert to Mobius form.
  t_ptr<t_bsurf> mobInitSurf = cascade::GetMobiusBSurface(initSurf);

  TIMER_NEW
  TIMER_GO

  // Prepare approximation tool.
  geom_ApproxBSurf approx(mobPts, mobInitSurf);

  // Constraint the boundary.
  const int nPolesU = int( mobInitSurf->GetPoles().size() );
  const int nPolesV = int( mobInitSurf->GetPoles()[0].size() );
  //
  for ( int i = 0; i < nPolesU; ++i )
  {
    approx.AddPinnedPole( i, 0 );
    approx.AddPinnedPole( i, nPolesV - 1 );
  }
  //
  for ( int j = 0; j < nPolesV; ++j )
  {
    approx.AddPinnedPole( 0, j );
    approx.AddPinnedPole( nPolesU - 1, j );
  }

  // Approximate.
  if ( !approx.Perform(lambda) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Approximation failed.");
    return false;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Approximate (passed initial surface)")

  // Get result.
  t_ptr<t_bsurf> mobResSurf = approx.GetResult();

  // Convert to OpenCascade B-surface.
  resultSurf = cascade::GetOpenCascadeBSurface(mobResSurf);
  return true;
#else
  m_progress.SendLogMessage(LogErr(Normal) << "Surface approximation module is not available.");
  return false;
#endif
}
