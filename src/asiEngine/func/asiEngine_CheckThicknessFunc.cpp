//-----------------------------------------------------------------------------
// Created on: 03 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiEngine_CheckThicknessFunc.h>

// asiData includes
#include <asiData_ThicknessNode.h>

// asiAlgo includes
#include <asiAlgo_CheckThickness.h>
#include <asiAlgo_Timer.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

int asiEngine_CheckThicknessFunc::execute(const Handle(ActAPI_HParameterList)& inputs,
                                          const Handle(ActAPI_HParameterList)& outputs,
                                          const Handle(Standard_Transient)&) const
{
  ActAPI_ProgressEntry progress = this->GetProgressNotifier();

  /* ============================
   *  Interpret input Parameters.
   * ============================ */

  // Get mesh.
  Handle(ActData_TriangulationParameter)
    trisParam = ActParamTool::AsTriangulation( inputs->Value(1) );
  //
  Handle(Poly_Triangulation) tris = trisParam->GetTriangulation();

  // Get Thickness Node.
  Handle(asiData_ThicknessNode)
    TN = Handle(asiData_ThicknessNode)::DownCast( trisParam->GetNode() );

  // Custom direction.
  const bool
    isCustomDir = ActParamTool::AsBool( TN->Parameter(asiData_ThicknessNode::PID_IsCustomDir) )->GetValue();
  //
  const double
    dx = ActParamTool::AsReal( TN->Parameter(asiData_ThicknessNode::PID_Dx) )->GetValue();
  const double
    dy = ActParamTool::AsReal( TN->Parameter(asiData_ThicknessNode::PID_Dy) )->GetValue();
  const double
    dz = ActParamTool::AsReal( TN->Parameter(asiData_ThicknessNode::PID_Dz) )->GetValue();

  gp_Vec dirVec(dx, dy, dz);
  //
  if ( dirVec.Magnitude() < 1.e-6 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Undefined direction.");
    return 1; // Error.
  }

  /* ===================
   *  Analyze thickness.
   * =================== */

  TIMER_NEW
  TIMER_GO

  // Initialize the algorithm.
  asiAlgo_CheckThickness algo(tris, m_progress, m_plotter);
  //
  algo.SetIsCustomDir(isCustomDir);
  algo.SetCustomDir( gp_Dir(dx, dy, dz) );

  // Perform.
  if ( !algo.Perform_RayMethod() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Thickness analysis failed.");
    return 1; // Error.
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(progress, "Check thickness")

  /* =======================
   *  Set output Parameters.
   * ======================= */

  // Set the thickness field.
  TN->SetMeshWithScalars( algo.GetThicknessField() );

  // Set the extreme thickness values.
  ActParamTool::AsReal( outputs->Value(1) )->SetValue( algo.GetMinThickness() );
  ActParamTool::AsReal( outputs->Value(2) )->SetValue( algo.GetMaxThickness() );

  return 0; // Success.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_CheckThicknessFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Triangulation // Mesh of a CAD part to check.
                                      << Parameter_Bool          // Is custom direction.
                                      << Parameter_Real          // X component of the custom direction.
                                      << Parameter_Real          // Y component of the custom direction.
                                      << Parameter_Real          // Z component of the custom direction.
  ;
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_CheckThicknessFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Real  // Min thickness.
                                      << Parameter_Real; // Max thickness.
}
