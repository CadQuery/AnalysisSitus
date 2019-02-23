//-----------------------------------------------------------------------------
// Created on: 09 February 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiData_DesignLawNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_DesignLawNode::asiData_DesignLawNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,  PID_Name);
  //
  REGISTER_PARAMETER(RealArray, PID_Poles);
  REGISTER_PARAMETER(RealArray, PID_Knots);
  REGISTER_PARAMETER(IntArray,  PID_Mults);
  REGISTER_PARAMETER(Int,       PID_Degree);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Node.
Handle(ActAPI_INode) asiData_DesignLawNode::Instance()
{
  return new asiData_DesignLawNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param law [in] transient law to initialize the persistent law from.
void asiData_DesignLawNode::Init(const Handle(asiAlgo_DesignLaw)& law)
{
  // Initialize Parameters
  this->InitParameter(PID_Name, "");
  //
  this->SetPoles  ( law->ChangePoles() );
  this->SetKnots  ( law->ChangeKnots() );
  this->SetMults  ( law->ChangeMults() );
  this->SetDegree ( law->ChangePoles()->Length() - 1 );
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_DesignLawNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_DesignLawNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets poles.
//! \param poles [in] poles to set.
void asiData_DesignLawNode::SetPoles(const Handle(HRealArray)& poles)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Poles) )->SetArray(poles);
}

//! \return poles.
Handle(HRealArray) asiData_DesignLawNode::GetPoles() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Poles) )->GetArray();
}

//! Sets knots.
//! \param knots [in] knots to set.
void asiData_DesignLawNode::SetKnots(const Handle(HRealArray)& knots)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Knots) )->SetArray(knots);
}

//! \return knots.
Handle(HRealArray) asiData_DesignLawNode::GetKnots() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Knots) )->GetArray();
}

//! Sets multiplicities.
//! \param mults [in] multiplicities to set.
void asiData_DesignLawNode::SetMults(const Handle(HIntArray)& mults)
{
  ActParamTool::AsIntArray( this->Parameter(PID_Mults) )->SetArray(mults);
}

//! \return multiplicities.
Handle(HIntArray) asiData_DesignLawNode::GetMults() const
{
  return ActParamTool::AsIntArray( this->Parameter(PID_Mults) )->GetArray();
}

//! Sets degree.
//! \param degree [in] degree to set.
void asiData_DesignLawNode::SetDegree(const int& degree)
{
  ActParamTool::AsInt( this->Parameter(PID_Degree) )->SetValue(degree);
}

//! \return degree.
int asiData_DesignLawNode::GetDegree() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Degree) )->GetValue();
}

//! \return transient representation of the persistent data.
Handle(asiAlgo_DesignLaw) asiData_DesignLawNode::AsLaw() const
{
  return new asiAlgo_DesignLaw( this->GetPoles(), this->GetKnots()->Last() );
}
