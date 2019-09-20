//-----------------------------------------------------------------------------
// Created on: 22 August 2019
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
#include <asiData_DeviationNode.h>

// asiAlgo includes
#include <asiAlgo_MeshField.h>

// OpenCascade includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_DeviationNode::asiData_DeviationNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,          PID_Name);
  REGISTER_PARAMETER(Triangulation, PID_Mesh);
  REGISTER_PARAMETER(IntArray,      PID_DistanceFieldIds);
  REGISTER_PARAMETER(RealArray,     PID_DistanceFieldValues);
  REGISTER_PARAMETER(Real,          PID_Tolerance);
  REGISTER_PARAMETER(Real,          PID_ScalarMin);
  REGISTER_PARAMETER(Real,          PID_ScalarMax);
}

//! Returns new DETACHED instance of Surface Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Surface Node.
Handle(ActAPI_INode) asiData_DeviationNode::Instance()
{
  return new asiData_DeviationNode();
}

//! Performs initial actions required to make Surface Node WELL-FORMED.
void asiData_DeviationNode::Init()
{
  // Initialize Parameters.
  this->InitParameter(PID_Name,      "Name",        "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Tolerance, "Allowance",   "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_ScalarMin, "Min. scalar", "", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_ScalarMax, "Max. scalar", "", ParameterFlag_IsVisible, true);

  // Set default values.
  this->SetMeshWithScalars( asiAlgo_Mesh() );
  //
  ActParamTool::AsReal( this->Parameter(PID_Tolerance) )->SetValue(  0.1 );
  ActParamTool::AsReal( this->Parameter(PID_ScalarMin) )->SetValue( -Precision::Infinite() );
  ActParamTool::AsReal( this->Parameter(PID_ScalarMax) )->SetValue(  Precision::Infinite() );
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_DeviationNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param[in] N name to set.
void asiData_DeviationNode::SetName(const TCollection_ExtendedString& N)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(N);
}

//-----------------------------------------------------------------------------

void asiData_DeviationNode::SetMeshWithScalars(const asiAlgo_Mesh& mesh)
{
  ActParamTool::AsTriangulation( this->Parameter(PID_Mesh) )->SetTriangulation(mesh.triangulation);

  // Prepare array of node ids.
  if ( mesh.fields.size() )
  {
    Handle(asiAlgo_MeshNScalarField)
      field = Handle(asiAlgo_MeshNScalarField)::DownCast(mesh.fields[0]);

    // Prepare arrays.
    const int numScalars = field->data.Extent();
    //
    Handle(HIntArray)  nodesArr  = new HIntArray  (0, numScalars - 1, 0);
    Handle(HRealArray) valuesArr = new HRealArray (0, numScalars - 1, 0.);

    // Populate arrays.
    int idx = 0;
    for ( asiAlgo_MeshNScalarField::t_data::Iterator it(field->data); it.More(); it.Next(), ++idx )
    {
      const int    nodeId = it.Key();
      const double scalar = it.Value();

      nodesArr->SetValue(idx, nodeId);
      valuesArr->SetValue(idx, scalar);
    }

    // Store data.
    ActParamTool::AsIntArray( this->Parameter(PID_DistanceFieldIds) )->SetArray(nodesArr);
    ActParamTool::AsRealArray( this->Parameter(PID_DistanceFieldValues) )->SetArray(valuesArr);
  }
  else
  {
    // Nullify arrays.
    ActParamTool::AsIntArray( this->Parameter(PID_DistanceFieldIds) )->SetArray(NULL);
    ActParamTool::AsRealArray( this->Parameter(PID_DistanceFieldValues) )->SetArray(NULL);
  }
}
