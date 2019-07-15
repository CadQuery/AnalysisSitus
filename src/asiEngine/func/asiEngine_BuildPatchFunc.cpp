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

// asiEngine includes
#include <asiEngine_RE.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

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

  // Collect coedges.
  std::vector<Handle(asiData_ReCoEdgeNode)> coedges;

  // Get min number of knots.
  const int
    minNumKnots = Handle(ActData_IntParameter)::DownCast( inputs->First() )->GetValue();

  // Collect coedges.
  int       currIdx    = 1;
  const int numCoedges = (inputs->Length() + 1) / 2;
  //
  for ( ActAPI_HParameterList::Iterator it(*inputs); it.More(); it.Next(), ++currIdx )
  {
    if ( currIdx == 1 ) // Skip min number of knots.
      continue;

    if ( currIdx > numCoedges + 1 )
      break;

    const Handle(ActAPI_IUserParameter)& uParam = it.Value();
    //
    Handle(asiData_ReCoEdgeNode)
      coedge = Handle(asiData_ReCoEdgeNode)::DownCast( uParam->GetNode() );

    if ( !coedge.IsNull() )
      coedges.push_back(coedge);
    else
      break;
  }

  /* =============
   *  Build patch
   * ============= */

  asiEngine_RE reApi(M, m_progress/*, m_plotter*/);

  // Fill Coons.
  Handle(Geom_BSplineSurface) bsurf;
  //
  if ( !reApi.FillPatchCoons(coedges, minNumKnots, bsurf) )
    return 1;

  /* =======================
   *  Set output Parameters
   * ======================= */

  // Set the surface to the output Parameter indirectly using the Patch Node.
  Handle(asiData_RePatchNode)
    patchNode = Handle(asiData_RePatchNode)::DownCast( outputs->First()->GetNode() );
  //
  patchNode->SetSurface(bsurf);

  return 0; // Success.
}

//-----------------------------------------------------------------------------

bool
  asiEngine_BuildPatchFunc::validateInput(const Handle(ActAPI_HParameterList)& inputs) const
{
  // Check the number of arguments.
  const int numArgs = inputs->Length();
  if ( numArgs % 2 != 1 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Number of arguments is even.");
    return false;
  }

  const int numCoedges = (numArgs - 1) / 2;
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
    else if ( currIdx <= numCoedges + 1 )
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
