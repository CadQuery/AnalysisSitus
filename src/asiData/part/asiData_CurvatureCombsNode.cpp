//-----------------------------------------------------------------------------
// Created on: 29 January 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018, Sergey Slyadnev
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
#include <asiData_CurvatureCombsNode.h>

// Active Data includes
#include <ActAux_ArrayUtils.h>
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_CurvatureCombsNode::asiData_CurvatureCombsNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Points);
  REGISTER_PARAMETER(BoolArray, PID_PointsStatuses);
  REGISTER_PARAMETER(RealArray, PID_Parameters);
  REGISTER_PARAMETER(RealArray, PID_Curvatures);
  REGISTER_PARAMETER(RealArray, PID_Combs);
  REGISTER_PARAMETER(Reference, PID_RefCurve);
  REGISTER_PARAMETER(Real,      PID_ScaleFactor);
}

//-----------------------------------------------------------------------------

//! Returns new DETACHED instance of Curvature Combs Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Curvature Combs Node.
Handle(ActAPI_INode) asiData_CurvatureCombsNode::Instance()
{
  return new asiData_CurvatureCombsNode();
}

//-----------------------------------------------------------------------------

//! Performs initial actions required to make Curvature Combs Node WELL-FORMED.
void asiData_CurvatureCombsNode::Init()
{
  // Initialize Parameters
  this->InitParameter(PID_Name,           "Name");
  this->InitParameter(PID_Points,         "Discretization points");
  this->InitParameter(PID_PointsStatuses, "Statuses of discretization points");
  this->InitParameter(PID_Parameters,     "Discretization parameters");
  this->InitParameter(PID_Curvatures,     "Curvatures");
  this->InitParameter(PID_Combs,          "Comb vectors");
  this->InitParameter(PID_ScaleFactor,    "Scale factor");

  // Initialize arrays
  ActParamTool::AsRealArray( this->Parameter(PID_Points) )         ->SetArray(NULL);
  ActParamTool::AsBoolArray( this->Parameter(PID_PointsStatuses) ) ->SetArray(NULL);
  ActParamTool::AsRealArray( this->Parameter(PID_Parameters) )     ->SetArray(NULL);
  ActParamTool::AsRealArray( this->Parameter(PID_Curvatures) )     ->SetArray(NULL);
  ActParamTool::AsRealArray( this->Parameter(PID_Combs) )          ->SetArray(NULL);

  // Set default values
  this->SetScaleFactor(1.0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_CurvatureCombsNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_CurvatureCombsNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::SetPoints(const std::vector<gp_Pnt>& points)
{
  // Prepare the array of coordinates
  Handle(HRealArray) coords;
  //
  if ( points.size() != 0 )
    ActAux_ArrayUtils::ToCoords3d<gp_Pnt>(points, coords);

  // Store in OCAF
  ActParamTool::AsRealArray( this->Parameter(PID_Points) )->SetArray(coords);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::GetPoints(std::vector<gp_Pnt>& points) const
{
  // Get array of coordinates from OCAF
  Handle(HRealArray)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Points) )->GetArray();
  //
  if ( coords.IsNull() )
    return;

  ActAux_ArrayUtils::FromCoords3d<gp_Pnt>(coords, points);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::SetPointsStatuses(const std::vector<bool>& statuses)
{
  // Prepare the array of flags
  Handle(HBoolArray) flags;
  //
  if ( statuses.size() != 0 )
  {
    flags = new HBoolArray(0, int ( statuses.size() ) - 1);
    //
    for ( int i = 0; i < flags->Length(); ++i )
      flags->ChangeValue(i) = statuses[i];
  }

  // Store in OCAF
  ActParamTool::AsBoolArray( this->Parameter(PID_PointsStatuses) )->SetArray(flags);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::GetPointsStatuses(std::vector<bool>& statuses) const
{
  // Get array of flags from OCAF
  Handle(HBoolArray)
    flags = ActParamTool::AsBoolArray( this->Parameter(PID_PointsStatuses) )->GetArray();
  //
  if ( flags.IsNull() )
    return;

  for ( int i = flags->Lower(); i <= flags->Upper(); ++i )
    statuses.push_back( flags->Value(i) );
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::SetParameters(const std::vector<double>& params)
{
  // Prepare the array of parameter values
  Handle(HRealArray) values;
  //
  if ( params.size() != 0 )
    ActAux_ArrayUtils::ToRealArray(params, values);

  // Store in OCAF
  ActParamTool::AsRealArray( this->Parameter(PID_Parameters) )->SetArray(values);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::GetParameters(std::vector<double>& params) const
{
  // Get array of parameter values from OCAF
  Handle(HRealArray)
    values = ActParamTool::AsRealArray( this->Parameter(PID_Parameters) )->GetArray();
  //
  if ( values.IsNull() )
    return;

  // Fill the vector
  ActAux_ArrayUtils::FromRealArray(values, params);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::SetCurvatures(const std::vector<double>& curvatures)
{
  // Prepare the array of curvature values
  Handle(HRealArray) values;
  //
  if ( curvatures.size() != 0 )
    ActAux_ArrayUtils::ToRealArray(curvatures, values);

  // Store in OCAF
  ActParamTool::AsRealArray( this->Parameter(PID_Curvatures) )->SetArray(values);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::GetCurvatures(std::vector<double>& curvatures) const
{
  // Get array of curvature values from OCAF
  Handle(HRealArray)
    values = ActParamTool::AsRealArray( this->Parameter(PID_Curvatures) )->GetArray();
  //
  if ( values.IsNull() )
    return;

  // Fill the vector
  ActAux_ArrayUtils::FromRealArray(values, curvatures);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::SetCombs(const std::vector<gp_Vec>& combs)
{
  // Prepare the array of coordinates
  Handle(HRealArray) coords;
  //
  if ( combs.size() != 0 )
    ActAux_ArrayUtils::ToCoords3d<gp_Vec>(combs, coords);

  // Store in OCAF
  ActParamTool::AsRealArray( this->Parameter(PID_Combs) )->SetArray(coords);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::GetCombs(std::vector<gp_Vec>& combs) const
{
  // Get array of coordinates from OCAF
  Handle(HRealArray)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Combs) )->GetArray();
  //
  if ( coords.IsNull() )
    return;

  ActAux_ArrayUtils::FromCoords3d<gp_Vec>(coords, combs);
}

//-----------------------------------------------------------------------------

void asiData_CurvatureCombsNode::SetScaleFactor(const double scaleFactor)
{
  ActParamTool::AsReal( this->Parameter(PID_ScaleFactor) )->SetValue(scaleFactor);
}

//-----------------------------------------------------------------------------

double asiData_CurvatureCombsNode::GetScaleFactor() const
{
  return ActParamTool::AsReal( this->Parameter(PID_ScaleFactor) )->GetValue();
}
