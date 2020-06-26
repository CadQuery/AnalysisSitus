//-----------------------------------------------------------------------------
// Created on: 26 June 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
#include <asiData_FaceNodeBase.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_FaceNodeBase::asiData_FaceNodeBase() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(Selection, PID_SelectedFaces);
}

//! Performs initial actions required to make Face Node WELL-FORMED.
void asiData_FaceNodeBase::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name", "", ParameterFlag_IsVisible, true);

  // Set default values to primitive Parameters
  this->SetSelectedFace(0);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_FaceNodeBase::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_FaceNodeBase::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets index of the active face.
//! \param[in] faceId index of the active face.
void asiData_FaceNodeBase::SetSelectedFace(const int faceId)
{
  Handle(ActData_SelectionParameter)
    param = ActParamTool::AsSelection( this->Parameter(PID_SelectedFaces) );

  if ( faceId <= 0 )
  {
    param->SetMask(nullptr);
  }
  else
  {
    // Prepare map.
    Handle(TColStd_HPackedMapOfInteger) hmap = param->GetMask();
    //
    if ( hmap.IsNull() )
      hmap = new TColStd_HPackedMapOfInteger;
    //
    hmap->ChangeMap().Clear();
    hmap->ChangeMap().Add(faceId);

    // Set as a parameter value.
    param->SetMask(hmap);
  }
}

//! Sets indices of the active faces.
//! \param[in] faceIds indices of the active faces to set.
void asiData_FaceNodeBase::SetSelectedFaces(const TColStd_PackedMapOfInteger& faceIds)
{
  Handle(ActData_SelectionParameter)
    param = ActParamTool::AsSelection( this->Parameter(PID_SelectedFaces) );

  // Set as a parameter value.
  param->SetMask( new TColStd_HPackedMapOfInteger(faceIds) );
}

//! Sets indices of the active faces.
//! \param[in] faceIds indices of the active faces to set.
void asiData_FaceNodeBase::SetSelectedFaces(const std::vector<int>& faceIds)
{
  TColStd_PackedMapOfInteger mask;
  for ( auto fit = faceIds.cbegin(); fit != faceIds.cend(); ++fit )
    mask.Add(*fit);

  this->SetSelectedFaces(mask);
}

//! \return any face index among those stored.
int asiData_FaceNodeBase::GetAnySelectedFace() const
{
  Handle(TColStd_HPackedMapOfInteger) hmap = this->GetSelectedFaces();
  //
  if ( hmap.IsNull() )
    return 0; // invalid face id

  return hmap->Map().GetMinimalMapped();
}

//! \return indices of the active faces.
Handle(TColStd_HPackedMapOfInteger) asiData_FaceNodeBase::GetSelectedFaces() const
{
  return ActParamTool::AsSelection( this->Parameter(PID_SelectedFaces) )->GetMask();
}
