//-----------------------------------------------------------------------------
// Created on: 21 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoData_LimbNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

ottoData_LimbNode::ottoData_LimbNode() : ActData_BaseNode()
{
  // Register standard Active Data Parameters
  REGISTER_PARAMETER(Name,          PID_Name);
  REGISTER_PARAMETER(AsciiString,   PID_FilenameMesh);
  REGISTER_PARAMETER(Triangulation, PID_Mesh);
  REGISTER_PARAMETER(Shape,         PID_OBB);
  REGISTER_PARAMETER(RealArray,     PID_OBBProps);
  REGISTER_PARAMETER(Shape,         PID_Sections);
  REGISTER_PARAMETER(Int,           PID_NumSections);
  REGISTER_PARAMETER(Shape,         PID_Surface);
  REGISTER_PARAMETER(TreeFunction,  PID_LoadMeshFunc);
  REGISTER_PARAMETER(TreeFunction,  PID_BuildOBBFunc);
  REGISTER_PARAMETER(TreeFunction,  PID_BuildSectionsFunc);
  REGISTER_PARAMETER(TreeFunction,  PID_SkinSurfaceFunc);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_INode) ottoData_LimbNode::Instance()
{
  return new ottoData_LimbNode();
}

//-----------------------------------------------------------------------------

void ottoData_LimbNode::Init()
{
  // Initialize Parameters.
  this->InitParameter(PID_Name,         "Name",          "",             ParameterFlag_IsVisible);
  this->InitParameter(PID_FilenameMesh, "Source STL",    "STL_Filename", ParameterFlag_IsVisible);
  this->InitParameter(PID_NumSections,  "Num. sections", "Num_Sections", ParameterFlag_IsVisible);

  // Set default values.
  ActParamTool::AsAsciiString ( this->Parameter(PID_FilenameMesh) ) ->SetValue( "" );
  ActParamTool::AsShape       ( this->Parameter(PID_OBB) )          ->SetShape( TopoDS_Shape() );
  ActParamTool::AsRealArray   ( this->Parameter(PID_OBBProps) )     ->SetArray( NULL );
  ActParamTool::AsShape       ( this->Parameter(PID_Sections) )     ->SetShape( TopoDS_Shape() );
  ActParamTool::AsInt         ( this->Parameter(PID_NumSections) )  ->SetValue( 2 );
  ActParamTool::AsShape       ( this->Parameter(PID_Surface) )      ->SetShape( TopoDS_Shape() );
}

//-----------------------------------------------------------------------------

TCollection_ExtendedString ottoData_LimbNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//-----------------------------------------------------------------------------

void ottoData_LimbNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}
