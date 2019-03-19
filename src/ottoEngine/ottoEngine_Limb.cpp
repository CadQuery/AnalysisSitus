//-----------------------------------------------------------------------------
// Created on: 08 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoEngine_Limb.h>

// ottoEngine includes
#include <ottoEngine_BuildOBBFunc.h>
#include <ottoEngine_BuildSectionsFunc.h>
#include <ottoEngine_LoadMeshFunc.h>
#include <ottoEngine_Model.h>
#include <ottoEngine_SkinSurfaceFunc.h>

//-----------------------------------------------------------------------------

Handle(ottoData_LimbNode) ottoEngine_Limb::CreateLimb()
{
  Handle(ottoEngine_Model) M = Handle(ottoEngine_Model)::DownCast(m_model);

  // Add Node to Partition.
  Handle(ottoData_LimbNode)
    limb_n = Handle(ottoData_LimbNode)::DownCast( ottoData_LimbNode::Instance() );
  //
  M->GetLimbPartition()->AddNode(limb_n);

  // Initialize.
  limb_n->Init();
  limb_n->SetUserFlags(NodeFlag_IsPresentedInPartView);
  limb_n->SetName("Limb");

  // Connect Tree Functions.
  limb_n->ConnectTreeFunction( ottoData_LimbNode::PID_LoadMeshFunc,
                               ottoEngine_LoadMeshFunc::GUID(),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_FilenameMesh),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_Mesh) );
  //
  limb_n->ConnectTreeFunction( ottoData_LimbNode::PID_BuildOBBFunc,
                               ottoEngine_BuildOBBFunc::GUID(),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_Mesh),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_OBB)
                                                        << limb_n->Parameter(ottoData_LimbNode::PID_OBBProps) );
  //
  limb_n->ConnectTreeFunction( ottoData_LimbNode::PID_BuildSectionsFunc,
                               ottoEngine_BuildSectionsFunc::GUID(),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_Mesh)
                                                        << limb_n->Parameter(ottoData_LimbNode::PID_OBBProps)
                                                        << limb_n->Parameter(ottoData_LimbNode::PID_NumSections),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_Sections) );
  //
  limb_n->ConnectTreeFunction( ottoData_LimbNode::PID_SkinSurfaceFunc,
                               ottoEngine_SkinSurfaceFunc::GUID(),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_Sections),
                               ActAPI_ParameterStream() << limb_n->Parameter(ottoData_LimbNode::PID_Surface) );

  // Return the just created Node
  return limb_n;
}
