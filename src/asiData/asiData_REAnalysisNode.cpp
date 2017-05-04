//-----------------------------------------------------------------------------
// Created on: 02 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_REAnalysisNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REAnalysisNode::asiData_REAnalysisNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REAnalysisNode::Instance()
{
  return new asiData_REAnalysisNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_REAnalysisNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REAnalysisNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_REAnalysisNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------

//! \return underlying Gauss map Node.
Handle(asiData_REGaussMapNode) asiData_REAnalysisNode::GetGaussMap()
{
  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast( this->GetChildNode(Child_GaussMap) );
  //
  if ( !gauss_n.IsNull() && gauss_n->IsWellFormed() )
    return gauss_n;
  //
  return NULL;
}

//! \return function Node.
Handle(asiData_REFunctionsNode) asiData_REAnalysisNode::GetFunctions()
{
  Handle(asiData_REFunctionsNode)
    functions_n = Handle(asiData_REFunctionsNode)::DownCast( this->GetChildNode(Child_Functions) );
  //
  if ( !functions_n.IsNull() && functions_n->IsWellFormed() )
    return functions_n;
  //
  return NULL;
}
