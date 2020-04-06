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
#include <asiData_ThicknessNode.h>

// asiAlgo includes
#include <asiAlgo_MeshField.h>

// OpenCascade includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

asiData_ThicknessNode::asiData_ThicknessNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,          PID_Name);
  REGISTER_PARAMETER(Triangulation, PID_Mesh);
  REGISTER_PARAMETER(IntArray,      PID_ThicknessFieldIds);
  REGISTER_PARAMETER(RealArray,     PID_ThicknessFieldValues);
  REGISTER_PARAMETER(Real,          PID_ScalarMin);
  REGISTER_PARAMETER(Real,          PID_ScalarMax);
  REGISTER_PARAMETER(Bool,          PID_IsCustomDir);
  REGISTER_PARAMETER(Real,          PID_Dx);
  REGISTER_PARAMETER(Real,          PID_Dy);
  REGISTER_PARAMETER(Real,          PID_Dz);
  REGISTER_PARAMETER(TreeFunction,  PID_CheckThicknessFunc);
}

//-----------------------------------------------------------------------------

void asiData_ThicknessNode::Init()
{
  // Initialize Parameters.
  this->InitParameter(PID_Name,        "Name",             "",                  ParameterFlag_IsVisible, true);
  this->InitParameter(PID_ScalarMin,   "Min. scalar",      "MinScalarReadOnly", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_ScalarMax,   "Max. scalar",      "MaxScalarReadOnly", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_IsCustomDir, "Custom direction", "",                  ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Dx,          "Dx",               "",                  ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Dy,          "Dy",               "",                  ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Dz,          "Dz",               "",                  ParameterFlag_IsVisible, true);

  // Set default values.
  this->SetMeshWithScalars( asiAlgo_Mesh() );
  //
  ActParamTool::AsReal( this->Parameter(PID_ScalarMin) )   ->SetValue(-Precision::Infinite() );
  ActParamTool::AsReal( this->Parameter(PID_ScalarMax) )   ->SetValue( Precision::Infinite() );
  ActParamTool::AsBool( this->Parameter(PID_IsCustomDir) ) ->SetValue( false );
  ActParamTool::AsReal( this->Parameter(PID_Dx) )          ->SetValue( 0. );
  ActParamTool::AsReal( this->Parameter(PID_Dy) )          ->SetValue( 0. );
  ActParamTool::AsReal( this->Parameter(PID_Dz) )          ->SetValue( 1. );
}

//-----------------------------------------------------------------------------

TCollection_ExtendedString asiData_ThicknessNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

void asiData_ThicknessNode::SetName(const TCollection_ExtendedString& N)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(N);
}

//-----------------------------------------------------------------------------

void asiData_ThicknessNode::SetMesh(const Handle(Poly_Triangulation)& mesh)
{
  ActParamTool::AsTriangulation( this->Parameter(PID_Mesh) )->SetTriangulation(mesh);
}

//-----------------------------------------------------------------------------

void asiData_ThicknessNode::SetMeshWithScalars(const asiAlgo_Mesh& mesh)
{
  this->SetMesh(mesh.triangulation);

  // Prepare array of ids.
  if ( mesh.fields.size() )
  {
    Handle(asiAlgo_MeshScalarField)
      field = Handle(asiAlgo_MeshScalarField)::DownCast(mesh.fields[0]);

    // Prepare arrays.
    const int numScalars = field->data.Extent();
    //
    Handle(HIntArray)  elemArr   = new HIntArray  (0, numScalars - 1, 0);
    Handle(HRealArray) valuesArr = new HRealArray (0, numScalars - 1, 0.);

    // Populate arrays.
    int idx = 0;
    for ( asiAlgo_MeshScalarField::t_data::Iterator it(field->data);
          it.More(); it.Next(), ++idx )
    {
      const int    elemId = it.Key();
      const double scalar = it.Value();

      elemArr->SetValue(idx, elemId);
      valuesArr->SetValue(idx, scalar);
    }

    // Store data.
    ActParamTool::AsIntArray( this->Parameter(PID_ThicknessFieldIds) )->SetArray(elemArr);
    ActParamTool::AsRealArray( this->Parameter(PID_ThicknessFieldValues) )->SetArray(valuesArr);
  }
  else
  {
    // Nullify arrays.
    ActParamTool::AsIntArray( this->Parameter(PID_ThicknessFieldIds) )->SetArray(nullptr);
    ActParamTool::AsRealArray( this->Parameter(PID_ThicknessFieldValues) )->SetArray(nullptr);
  }
}
