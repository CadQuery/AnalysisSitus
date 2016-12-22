//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
