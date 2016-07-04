//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_re.h>

// A-Situs (common) includes
#include <common_facilities.h>
#include <common_model.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

//-----------------------------------------------------------------------------

//! \return newly created Reverse Engineering Node.
Handle(geom_re_node) engine_re::Create_RE()
{
  // Access Model
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Add RE Node to Partition
  Handle(geom_re_node) re_n = Handle(geom_re_node)::DownCast( geom_re_node::Instance() );
  M->REPartition()->AddNode(re_n);

  // Initialize Node
  re_n->Init();
  re_n->SetName("Reverse Engineering");

  // Create underlying Surfaces Node
  {
    Handle(ActAPI_INode) geom_surfaces_base = geom_re_surfaces_node::Instance();
    M->RESurfacesPartition()->AddNode(geom_surfaces_base);

    // Initialize
    Handle(geom_re_surfaces_node) re_surfaces_n = Handle(geom_re_surfaces_node)::DownCast(geom_surfaces_base);
    re_surfaces_n->Init();
    re_surfaces_n->SetName("Surfaces");

    // Set as child
    re_n->AddChildNode(re_surfaces_n);
  }

  // Create underlying Contours Node
  {
    Handle(ActAPI_INode) geom_contours_base = geom_re_contours_node::Instance();
    M->REContoursPartition()->AddNode(geom_contours_base);

    // Initialize
    Handle(geom_re_contours_node) re_contours_n = Handle(geom_re_contours_node)::DownCast(geom_contours_base);
    re_contours_n->Init();
    re_contours_n->SetName("Contours");

    // Set as child
    re_n->AddChildNode(re_contours_n);
  }

  // Create underlying Points Node
  {
    Handle(ActAPI_INode) geom_points_base = geom_re_points_node::Instance();
    M->REPointsPartition()->AddNode(geom_points_base);

    // Initialize
    Handle(geom_re_points_node) re_points_n = Handle(geom_re_points_node)::DownCast(geom_points_base);
    re_points_n->Init();
    re_points_n->SetName("Points");

    // Set as child
    re_n->AddChildNode(re_points_n);
  }

  // Return the just created Node
  return re_n;
}

//-----------------------------------------------------------------------------

//! Creates Surface Node for the given parametric surface.
//! \param surface [in] target parametric surface.
//! \param uLimit  [in] bounding U value for infinite domains.
//! \param vLimit  [in] bounding V value for infinite domains.
//! \return Surface Node.
Handle(geom_re_surface_node)
  engine_re::Create_Surface(const Handle(Geom_Surface)& surface,
                            const double                uLimit,
                            const double                vLimit)
{
  // Access Model and RE Surfaces Node
  Handle(common_model)          M    = common_facilities::Instance()->Model;
  Handle(geom_re_surfaces_node) RESs = M->RENode()->Surfaces();

  // Add Surface Node to Partition
  Handle(geom_re_surface_node) surf_n = Handle(geom_re_surface_node)::DownCast( geom_re_surface_node::Instance() );
  M->RESurfacePartition()->AddNode(surf_n);

  // Generate unique name
  TCollection_ExtendedString surf_name("Surface");
  surf_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(surf_n, RESs), surf_name);

  // Initialize
  surf_n->Init(surface, uLimit, vLimit);
  surf_n->SetName(surf_name);

  // Set as child
  RESs->AddChildNode(surf_n);

  // Return the just created Node
  return surf_n;
}

//-----------------------------------------------------------------------------

//! Deletes all Surface Nodes.
void engine_re::Clean_Surfaces()
{
  Handle(common_model)          M             = common_facilities::Instance()->Model;
  Handle(geom_re_surfaces_node) RESs          = M->RENode()->Surfaces();
  Handle(ActAPI_HNodeList)      nodesToDelete = new ActAPI_HNodeList;

  // Loop over direct children of a Surfaces Node
  for ( Handle(ActAPI_IChildIterator) cit = RESs->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();

    // Check if the given Node is consistent
    if ( child_n.IsNull() || !child_n->IsWellFormed() )
      continue;

    // Set Node for deletion
    nodesToDelete->Append(child_n);
  }

  // Delete all Nodes queued for removal
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
    M->DeleteNode( nit.Value()->GetId() );
}

//-----------------------------------------------------------------------------

//! Creates a new Contour Node initializing it with the passed topological
//! wire.
//! \param contour [in] contour in form of topological wire.
//! \return just created Data Node.
Handle(geom_re_contour_node) engine_re::Create_Contour(const TopoDS_Wire& contour)
{
  // Access Model and RE Contours Node
  Handle(common_model)          M    = common_facilities::Instance()->Model;
  Handle(geom_re_contours_node) RECs = M->RENode()->Contours();

  // Add Contour Node to Partition
  Handle(geom_re_contour_node) cont_n = Handle(geom_re_contour_node)::DownCast( geom_re_contour_node::Instance() );
  M->REContourPartition()->AddNode(cont_n);

  // Generate unique name
  TCollection_ExtendedString cont_name("Contour");
  cont_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(cont_n, RECs), cont_name);

  // Initialize
  cont_n->Init(contour);
  cont_n->SetName(cont_name);

  // Set as child
  RECs->AddChildNode(cont_n);

  // Return the just created Node
  return cont_n;
}

//-----------------------------------------------------------------------------

//! Cleans up all contours stored in the collection of RE contours.
void engine_re::Clean_Contours()
{
  Handle(common_model)          M             = common_facilities::Instance()->Model;
  Handle(geom_re_contours_node) RECs          = M->RENode()->Contours();
  Handle(ActAPI_HNodeList)      nodesToDelete = new ActAPI_HNodeList;

  // Loop over direct children of a Contours Node
  for ( Handle(ActAPI_IChildIterator) cit = RECs->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();

    // Check if the given Node is consistent
    if ( child_n.IsNull() || !child_n->IsWellFormed() )
      continue;

    // Set Node for deletion
    nodesToDelete->Append(child_n);
  }

  // Delete all Nodes queued for removal
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
    M->DeleteNode( nit.Value()->GetId() );
}
