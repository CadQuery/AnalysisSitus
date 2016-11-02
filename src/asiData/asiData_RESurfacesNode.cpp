//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiData_RESurfacesNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_RESurfacesNode::asiData_RESurfacesNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RESurfacesNode::Instance()
{
  return new asiData_RESurfacesNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_RESurfacesNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RESurfacesNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_RESurfacesNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Accessor for an individual persistent surface.
//! \param oneBased_idx [in] 1-based surface index.
//! \return Surface Node.
Handle(asiData_RESurfaceNode) asiData_RESurfacesNode::Surface(const int oneBased_idx)
{
  int idx = 1;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    if ( oneBased_idx == idx )
    {
      Handle(asiData_RESurfaceNode)
        surf_n = Handle(asiData_RESurfaceNode)::DownCast( cit->Value() );
      //
      return surf_n;
    }

    ++idx;
  }

  return NULL;
}
