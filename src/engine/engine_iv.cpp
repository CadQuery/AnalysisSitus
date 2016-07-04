//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_iv.h>

// Common includes
#include <common_facilities.h>
#include <common_model.h>

// Mesh includes
#include <tess_convert.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

// OCCT includes
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>

//! \return newly created IV Node.
Handle(visu_iv_node) engine_iv::Create_IV()
{
  // Access Model
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Add IV Node to Partition
  Handle(visu_iv_node) iv_n = Handle(visu_iv_node)::DownCast( visu_iv_node::Instance() );
  M->IVPartition()->AddNode(iv_n);

  // Initialize Node
  iv_n->Init();
  iv_n->SetName("Imperative Viewer");

  // Create underlying Points 2D
  {
    Handle(ActAPI_INode) iv_points_2d_base = visu_iv_points_2d_node::Instance();
    M->IVPoints2dPartition()->AddNode(iv_points_2d_base);

    // Initialize
    Handle(visu_iv_points_2d_node) iv_points_2d_n = Handle(visu_iv_points_2d_node)::DownCast(iv_points_2d_base);
    iv_points_2d_n->Init();
    iv_points_2d_n->SetName("Points 2D");

    // Add as child
    iv_n->AddChildNode(iv_points_2d_n);
  }

  // Create underlying Points
  {
    Handle(ActAPI_INode) iv_points_base = visu_iv_points_node::Instance();
    M->IVPointsPartition()->AddNode(iv_points_base);

    // Initialize
    Handle(visu_iv_points_node) iv_points_n = Handle(visu_iv_points_node)::DownCast(iv_points_base);
    iv_points_n->Init();
    iv_points_n->SetName("Points");

    // Add as child
    iv_n->AddChildNode(iv_points_n);
  }

  // Create underlying Curves
  {
    Handle(ActAPI_INode) iv_curves_base = visu_iv_curves_node::Instance();
    M->IVCurvesPartition()->AddNode(iv_curves_base);

    // Initialize
    Handle(visu_iv_curves_node) iv_curves_n = Handle(visu_iv_curves_node)::DownCast(iv_curves_base);
    iv_curves_n->Init();
    iv_curves_n->SetName("Curves");

    // Add as child
    iv_n->AddChildNode(iv_curves_n);
  }

  // Create underlying Surfaces
  {
    Handle(ActAPI_INode) iv_surfaces_base = visu_iv_surfaces_node::Instance();
    M->IVSurfacesPartition()->AddNode(iv_surfaces_base);

    // Initialize
    Handle(visu_iv_surfaces_node) iv_surfaces_n = Handle(visu_iv_surfaces_node)::DownCast(iv_surfaces_base);
    iv_surfaces_n->Init();
    iv_surfaces_n->SetName("Surfaces");

    // Add as child
    iv_n->AddChildNode(iv_surfaces_n);
  }

  // Create underlying Topology container
  {
    Handle(ActAPI_INode) iv_topo_base = visu_iv_topo_node::Instance();
    M->IVTopoPartition()->AddNode(iv_topo_base);

    // Initialize
    Handle(visu_iv_topo_node) iv_topo_n = Handle(visu_iv_topo_node)::DownCast(iv_topo_base);
    iv_topo_n->Init();
    iv_topo_n->SetName("Topology");

    // Add as child
    iv_n->AddChildNode(iv_topo_n);
  }

  // Create underlying Tessellation container
  {
    Handle(ActAPI_INode) iv_tess_base = visu_iv_tess_node::Instance();
    M->IVTessPartition()->AddNode(iv_tess_base);

    // Initialize
    Handle(visu_iv_tess_node) iv_tess_n = Handle(visu_iv_tess_node)::DownCast(iv_tess_base);
    iv_tess_n->Init();
    iv_tess_n->SetName("Tessellation");

    // Add as child
    iv_n->AddChildNode(iv_tess_n);
  }

  // Return the just created Node
  return iv_n;
}

//-----------------------------------------------------------------------------

//! Creates Point Set 2D Node.
//! \param coords [in] target point cloud.
//! \param name   [in] name to set (auto-generated if empty).
//! \return Point Set 2D Node.
Handle(visu_iv_point_set_2d_node)
  engine_iv::Create_PointSet2d(const Handle(TColStd_HArray1OfReal)& coords,
                               const TCollection_AsciiString&       name)
{
  // Access Model and parent Node
  Handle(common_model)           M         = common_facilities::Instance()->Model;
  Handle(visu_iv_points_2d_node) IV_Parent = M->IVNode()->Points2d();

  // Add Point Set Node to Partition
  Handle(visu_iv_point_set_2d_node) item_n = Handle(visu_iv_point_set_2d_node)::DownCast( visu_iv_point_set_2d_node::Instance() );
  M->IVPointSet2dPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Point Set 2D" : name );
  item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Initialize
  item_n->Init();
  item_n->SetName(item_name);
  item_n->SetPoints(coords);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Deletes all Point Set 2D Nodes.
void engine_iv::Clean_Points2d()
{
  Handle(visu_iv_points_2d_node)
    IV_Parent = common_facilities::Instance()->Model->IVNode()->Points2d();
  //
  _cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Creates Point Set Node.
//! \param points [in] target point cloud.
//! \param name   [in] name to set (auto-generated if empty).
//! \return Point Set Node.
Handle(visu_iv_point_set_node)
  engine_iv::Create_PointSet(const Handle(geom_point_cloud)& points,
                             const TCollection_AsciiString&  name)
{
  // Access Model and parent Node
  Handle(common_model)        M         = common_facilities::Instance()->Model;
  Handle(visu_iv_points_node) IV_Parent = M->IVNode()->Points();

  // Add Point Set Node to Partition
  Handle(visu_iv_point_set_node) item_n = Handle(visu_iv_point_set_node)::DownCast( visu_iv_point_set_node::Instance() );
  M->IVPointSetPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Point Set" : name );
  item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Initialize
  item_n->Init();
  item_n->SetName(item_name);
  item_n->SetPoints(points);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Deletes all Point Set Nodes.
void engine_iv::Clean_Points()
{
  Handle(visu_iv_points_node)
    IV_Parent = common_facilities::Instance()->Model->IVNode()->Points();
  //
  _cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Creates Curve Node.
//! \param curve  [in] parametric curve to store.
//! \param uLimit [in] absolute value to bound infinite parametric domain.
//! \param name   [in] name to set (auto-generated if empty).
//! \return newly created Node.
Handle(visu_iv_curve_node)
  engine_iv::Create_Curve(const Handle(Geom_Curve)&      curve,
                          const double                   uLimit,
                          const TCollection_AsciiString& name)
{
  if ( curve.IsNull() )
    return NULL;

  // Access Model and parent Node
  Handle(common_model)        M         = common_facilities::Instance()->Model;
  Handle(visu_iv_curves_node) IV_Parent = M->IVNode()->Curves();

  // Add Curve Node to Partition
  Handle(visu_iv_curve_node) item_n = Handle(visu_iv_curve_node)::DownCast( visu_iv_curve_node::Instance() );
  M->IVCurvePartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Curve" : name );
  item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Handle infinite domains
  double             f      = curve->FirstParameter();
  double             l      = curve->LastParameter();
  bool               toTrim = false;
  Handle(Geom_Curve) geometry;
  //
  if ( Precision::IsInfinite(f) )
  {
    f      = -uLimit;
    toTrim = true;
  }
  if ( Precision::IsInfinite(l) )
  {
    l = uLimit;
    if ( !toTrim ) toTrim = true;
  }
  //
  if ( toTrim )
    geometry = new Geom_TrimmedCurve(curve, f, l);
  else
    geometry = curve;

  // Initialize
  item_n->Init();
  item_n->SetName(item_name);
  item_n->SetCurve(geometry, f, l);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//! Deletes all Curve Nodes.
void engine_iv::Clean_Curves()
{
  Handle(visu_iv_curves_node)
    IV_Parent = common_facilities::Instance()->Model->IVNode()->Curves();
  //
  _cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Creates Surface Node.
//! \param surface [in] parametric surface to store.
//! \param uLimit  [in] absolute value to bound infinite parametric domain in U.
//! \param vLimit  [in] absolute value to bound infinite parametric domain in V.
//! \param name    [in] name to set (auto-generated if empty).
//! \return newly created Node.
Handle(visu_iv_surface_node)
  engine_iv::Create_Surface(const Handle(Geom_Surface)&    surface,
                            const double                   uLimit,
                            const double                   vLimit,
                            const TCollection_AsciiString& name)
{
  if ( surface.IsNull() )
    return NULL;

  // Access Model and parent Node
  Handle(common_model)          M         = common_facilities::Instance()->Model;
  Handle(visu_iv_surfaces_node) IV_Parent = M->IVNode()->Surfaces();

  // Add Surface Node to Partition
  Handle(visu_iv_surface_node) item_n = Handle(visu_iv_surface_node)::DownCast( visu_iv_surface_node::Instance() );
  M->IVSurfacePartition()->AddNode(item_n);

  // Generate unique name if a good name is not passed
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Surface" : name );
  item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Initialize
  item_n->Init();
  item_n->SetName(item_name);
  item_n->SetSurface(surface);
  item_n->SetLimits(uLimit, vLimit);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//! Deletes all Surface Nodes.
void engine_iv::Clean_Surfaces()
{
  Handle(visu_iv_surfaces_node)
    IV_Parent = common_facilities::Instance()->Model->IVNode()->Surfaces();
  //
  _cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Creates Topological Item Node.
//! \param shape [in] shape to store.
//! \param name  [in] name to set (auto-generated if empty).
//! \return newly created Node.
Handle(visu_iv_topo_item_node)
  engine_iv::Create_TopoItem(const TopoDS_Shape&            shape,
                             const TCollection_AsciiString& name)
{
  // Access Model and parent Node
  Handle(common_model)      M         = common_facilities::Instance()->Model;
  Handle(visu_iv_topo_node) IV_Parent = M->IVNode()->Topology();

  // Add Topological Item Node to Partition
  Handle(visu_iv_topo_item_node) item_n = Handle(visu_iv_topo_item_node)::DownCast( visu_iv_topo_item_node::Instance() );
  M->IVTopoItemPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Shape" : name );
  item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Initialize
  item_n->Init();
  item_n->SetName(item_name);
  item_n->SetShape(shape);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//! Deletes all topological items.
void engine_iv::Clean_Topo()
{
  Handle(visu_iv_topo_node)
    IV_Parent = common_facilities::Instance()->Model->IVNode()->Topology();
  //
  _cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Creates tessellation item.
//! \param tess [in] surface triangulation to create a tessellation item from.
//! \param name [in] name of the tessellation item.
//! \return newly created tessellation item.
Handle(visu_iv_tess_item_node)
  engine_iv::Create_TessItem(const Handle(Poly_Triangulation)& tess,
                             const TCollection_AsciiString&    name)
{
  // Access Model and parent Node
  Handle(common_model)      M         = common_facilities::Instance()->Model;
  Handle(visu_iv_tess_node) IV_Parent = M->IVNode()->Tessellation();

  // Add Tessellation Item Node to Partition
  Handle(visu_iv_tess_item_node) item_n = Handle(visu_iv_tess_item_node)::DownCast( visu_iv_tess_item_node::Instance() );
  M->IVTessItemPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Mesh" : name );
  item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Convert to AD mesh
  Handle(Mesh) mesh;
  if ( !tess_convert::ToPersistent(tess, mesh) )
    return NULL;

  // Initialize
  item_n->Init();
  item_n->SetName(item_name);
  item_n->SetMesh(mesh);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//! Deletes all tessellation items.
void engine_iv::Clean_Tess()
{
  Handle(visu_iv_tess_node)
    IV_Parent = common_facilities::Instance()->Model->IVNode()->Tessellation();
  //
  _cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Removes all child Nodes for the given parent.
//! \param parent [in] parent Node to clean up children for.
void engine_iv::_cleanChildren(const Handle(ActAPI_INode)& parent)
{
  Handle(common_model)     M             = common_facilities::Instance()->Model;
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // Loop over direct children of a Surfaces Node
  for ( Handle(ActAPI_IChildIterator) cit = parent->GetChildIterator(); cit->More(); cit->Next() )
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
