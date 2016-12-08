//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_RE.h>

// A-Situs (common) includes
#include <asiEngine_Model.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

//-----------------------------------------------------------------------------

//! \return newly created Reverse Engineering Node.
Handle(asiData_RENode) exeAsBuilt_RE::Create_RE()
{
  // Add RE Node to Partition
  Handle(asiData_RENode) re_n = Handle(asiData_RENode)::DownCast( asiData_RENode::Instance() );
  m_model->GetREPartition()->AddNode(re_n);

  // Initialize Node
  re_n->Init();
  re_n->SetName("Reverse Engineering");

  // Create underlying Surfaces Node
  {
    Handle(ActAPI_INode) geom_surfaces_base = asiData_RESurfacesNode::Instance();
    m_model->GetRESurfacesPartition()->AddNode(geom_surfaces_base);

    // Initialize
    Handle(asiData_RESurfacesNode) re_surfaces_n = Handle(asiData_RESurfacesNode)::DownCast(geom_surfaces_base);
    re_surfaces_n->Init();
    re_surfaces_n->SetName("Surfaces");

    // Set as child
    re_n->AddChildNode(re_surfaces_n);
  }

  // Create underlying Contours Node
  {
    Handle(ActAPI_INode) geom_contours_base = asiData_REContoursNode::Instance();
    m_model->GetREContoursPartition()->AddNode(geom_contours_base);

    // Initialize
    Handle(asiData_REContoursNode) re_contours_n = Handle(asiData_REContoursNode)::DownCast(geom_contours_base);
    re_contours_n->Init();
    re_contours_n->SetName("Contours");

    // Set as child
    re_n->AddChildNode(re_contours_n);
  }

  // Create underlying Points Node
  {
    Handle(ActAPI_INode) geom_points_base = asiData_REPointsNode::Instance();
    m_model->GetREPointsPartition()->AddNode(geom_points_base);

    // Initialize
    Handle(asiData_REPointsNode) re_points_n = Handle(asiData_REPointsNode)::DownCast(geom_points_base);
    re_points_n->Init();
    re_points_n->SetName("Points");

    // Set as child
    re_n->AddChildNode(re_points_n);
  }

  // Create underlying Normals Node
  {
    Handle(ActAPI_INode) geom_normals_base = asiData_RENormalsNode::Instance();
    m_model->GetRENormalsPartition()->AddNode(geom_normals_base);

    // Initialize
    Handle(asiData_RENormalsNode) re_normals_n = Handle(asiData_RENormalsNode)::DownCast(geom_normals_base);
    re_normals_n->Init();
    re_normals_n->SetName("Normals");

    // Set as child
    re_n->AddChildNode(re_normals_n);
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
Handle(asiData_RESurfaceNode)
  exeAsBuilt_RE::Create_Surface(const Handle(Geom_Surface)& surface,
                                const double                uLimit,
                                const double                vLimit)
{
  // Access Model and RE Surfaces Node
  Handle(asiData_RESurfacesNode) RESs = m_model->GetRENode()->Surfaces();

  // Add Surface Node to Partition
  Handle(asiData_RESurfaceNode) surf_n = Handle(asiData_RESurfaceNode)::DownCast( asiData_RESurfaceNode::Instance() );
  m_model->GetRESurfacePartition()->AddNode(surf_n);

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
void exeAsBuilt_RE::Clean_Surfaces()
{
  Handle(asiData_RESurfacesNode) RESs          = m_model->GetRENode()->Surfaces();
  Handle(ActAPI_HNodeList)       nodesToDelete = new ActAPI_HNodeList;

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
    m_model->DeleteNode( nit.Value()->GetId() );
}

//-----------------------------------------------------------------------------

//! Creates a new Contour Node initializing it with the passed topological wire.
//! \param contour [in] contour in form of topological wire.
//! \return just created Data Node.
Handle(asiData_REContourNode) exeAsBuilt_RE::Create_Contour(const TopoDS_Wire& contour)
{
  // Access Model and RE Contours Node
  Handle(asiData_REContoursNode) RECs = m_model->GetRENode()->Contours();

  // Add Contour Node to Partition
  Handle(asiData_REContourNode) cont_n = Handle(asiData_REContourNode)::DownCast( asiData_REContourNode::Instance() );
  m_model->GetREContourPartition()->AddNode(cont_n);

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
void exeAsBuilt_RE::Clean_Contours()
{
  Handle(asiData_REContoursNode) RECs          = m_model->GetRENode()->Contours();
  Handle(ActAPI_HNodeList)       nodesToDelete = new ActAPI_HNodeList;

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
    m_model->DeleteNode( nit.Value()->GetId() );
}
