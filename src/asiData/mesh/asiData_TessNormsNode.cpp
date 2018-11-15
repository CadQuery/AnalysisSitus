//-----------------------------------------------------------------------------
// Created on: 05 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiData_TessNormsNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_TessNormsNode::asiData_TessNormsNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(IntArray,  PID_IDs);
  REGISTER_PARAMETER(RealArray, PID_Vectors);
  REGISTER_PARAMETER(Bool,      PID_IsElemental);
}

//-----------------------------------------------------------------------------

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_TessNormsNode::Instance()
{
  return new asiData_TessNormsNode();
}

//-----------------------------------------------------------------------------

//! Performs initial actions required to make Node WELL-FORMED.
//! \param[in] ids     element IDs.
//! \param[in] vectors vector coordinates.
void asiData_TessNormsNode::Init(const Handle(HIntArray)&  ids,
                                 const Handle(HRealArray)& vectors)
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  this->SetIDs(ids);
  this->SetVectors(vectors);
  this->SetIsElemental(true);
}

//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_TessNormsNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Sets name for the Node.
//! \param[in] name name to set.
void asiData_TessNormsNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

//! Sets array of mesh element IDs having vector values associated.
//! \param[in] ids array to set.
void asiData_TessNormsNode::SetIDs(const Handle(HIntArray)& ids)
{
  ActData_ParameterFactory::AsIntArray( this->Parameter(PID_IDs) )->SetArray(ids);
}

//-----------------------------------------------------------------------------

//! Accessor for the array of mesh element IDs having vector values associated.
//! \return requested array.
Handle(HIntArray) asiData_TessNormsNode::GetIDs() const
{
  return ActData_ParameterFactory::AsIntArray( this->Parameter(PID_IDs) )->GetArray();
}

//-----------------------------------------------------------------------------

//! Sets array of mesh elemental vectors.
//! \param[in] vectors array to set.
void asiData_TessNormsNode::SetVectors(const Handle(HRealArray)& vectors)
{
  ActData_ParameterFactory::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(vectors);
}

//-----------------------------------------------------------------------------

//! Accessor for the array of mesh elemental vectors.
//! \return requested array.
Handle(HRealArray) asiData_TessNormsNode::GetVectors() const
{
  return ActData_ParameterFactory::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();
}

//-----------------------------------------------------------------------------

//! Sets the flag indicating that the stored vector field corresponds to mesh
//! elements rather than to mesh nodes.
//! \param[in] isElemental value to set.
void asiData_TessNormsNode::SetIsElemental(const bool isElemental)
{
  ActData_ParameterFactory::AsBool( this->Parameter(PID_IsElemental) )->SetValue(isElemental);
}

//-----------------------------------------------------------------------------

//! \return true if the stored vector field corresponds to mesh elements.
bool asiData_TessNormsNode::IsElemental() const
{
  return ActData_ParameterFactory::AsBool( this->Parameter(PID_IsElemental) )->GetValue();
}
