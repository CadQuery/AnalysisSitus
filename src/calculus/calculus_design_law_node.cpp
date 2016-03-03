//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <calculus_design_law_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
calculus_design_law_node::calculus_design_law_node() : ActData_BaseNode()
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
Handle(ActAPI_INode) calculus_design_law_node::Instance()
{
  return new calculus_design_law_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
//! \param law [in] transient law to initialize the persistent law from.
void calculus_design_law_node::Init(const Handle(calculus_design_law)& law)
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
TCollection_ExtendedString calculus_design_law_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void calculus_design_law_node::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets poles.
//! \param poles [in] poles to set.
void calculus_design_law_node::SetPoles(const Handle(HRealArray)& poles)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Poles) )->SetArray(poles);
}

//! \return poles.
Handle(HRealArray) calculus_design_law_node::GetPoles() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Poles) )->GetArray();
}

//! Sets knots.
//! \param knots [in] knots to set.
void calculus_design_law_node::SetKnots(const Handle(HRealArray)& knots)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Knots) )->SetArray(knots);
}

//! \return knots.
Handle(HRealArray) calculus_design_law_node::GetKnots() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Knots) )->GetArray();
}

//! Sets multiplicities.
//! \param mults [in] multiplicities to set.
void calculus_design_law_node::SetMults(const Handle(HIntArray)& mults)
{
  ActParamTool::AsIntArray( this->Parameter(PID_Mults) )->SetArray(mults);
}

//! \return multiplicities.
Handle(HIntArray) calculus_design_law_node::GetMults() const
{
  return ActParamTool::AsIntArray( this->Parameter(PID_Mults) )->GetArray();
}

//! Sets degree.
//! \param degree [in] degree to set.
void calculus_design_law_node::SetDegree(const int& degree)
{
  ActParamTool::AsInt( this->Parameter(PID_Degree) )->SetValue(degree);
}

//! \return degree.
int calculus_design_law_node::GetDegree() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Degree) )->GetValue();
}

//! \return transient representation of the persistent data.
Handle(calculus_design_law) calculus_design_law_node::AsLaw() const
{
  return new calculus_design_law( this->GetPoles(), this->GetKnots()->Last() );
}
