//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_contour_node.h>

// Geometry includes
#include <geom_point_cloud.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// Active Data (auxiliary) includes
#include <ActAux_ArrayUtils.h>

// OCCT includes
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_contour_node::geom_contour_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Coords);
  REGISTER_PARAMETER(Bool,      PID_IsClosed);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_contour_node::Instance()
{
  return new geom_contour_node();
}

//! Performs initial actions required to make the Node WELL-FORMED.
void geom_contour_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set default values to primitive Parameters
  this->SetCoords(NULL);
  this->SetClosed(false);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_contour_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_contour_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets coordinates of polyline points.
//! \param[in] coords coordinates to store.
void geom_contour_node::SetCoords(const Handle(HRealArray)& coords)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Coords) )->SetArray(coords);
}

//! \return stored array.
Handle(HRealArray) geom_contour_node::GetCoords() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Coords) )->GetArray();
}

//! Adds another point to the contour.
//! \param[in] point point to add.
void geom_contour_node::AddPoint(const gp_XYZ& point)
{
  Handle(HRealArray)
    arr = ActParamTool::AsRealArray( this->Parameter(PID_Coords) )->GetArray();

  arr = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( arr, point.X() );
  arr = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( arr, point.Y() );
  arr = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( arr, point.Z() );

  this->SetCoords(arr);
}

//! Sets closeness property for the contour.
//! \param[in] isClosed indicates whether the contour is
//!            closed or not.
void geom_contour_node::SetClosed(const bool isClosed)
{
  ActParamTool::AsBool( this->Parameter(PID_IsClosed) )->SetValue(isClosed);
}

//! \return true if the contour is defined as closed,
//!         false -- otherwise.
bool geom_contour_node::IsClosed() const
{
  return ActParamTool::AsBool( this->Parameter(PID_IsClosed) )->GetValue();
}

//! \return contour converted to shape.
TopoDS_Wire geom_contour_node::AsShape() const
{
  Handle(geom_point_cloud) points = new geom_point_cloud( this->GetCoords() );
  //
  if ( !points->GetNumOfPoints() )
    return TopoDS_Wire();

  const int                    nPts    = points->GetNumOfPoints();
  TopoDS_Vertex                V_first = BRepBuilderAPI_MakeVertex( points->GetPoint(0) );
  TopoDS_Vertex                V_prev  = V_first, V_last;
  Handle(ShapeExtend_WireData) WD      = new ShapeExtend_WireData;
  //
  for ( int p = 1; p < nPts; ++p )
  {
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex( points->GetPoint(p) );
    TopoDS_Edge   E = BRepBuilderAPI_MakeEdge(V_prev, V);
    //
    WD->Add(E);
    //
    if ( p == nPts - 1 )
      V_last = V;

    V_prev = V;
  }
  //
  if ( this->IsClosed() )
  {
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(V_last, V_first);
    WD->Add(E);
  }

  return WD->WireAPIMake();
}

//! Re-packages all coordinates to a convenient sequence structure.
//! \param[out] point result of re-packaging.
void geom_contour_node::AsPoints(TColgp_SequenceOfPnt& points) const
{
  Handle(geom_point_cloud) pcloud = new geom_point_cloud( this->GetCoords() );
  const int                nPts   = pcloud->GetNumOfPoints();
  //
  for ( int p = 1; p < nPts; ++p )
  {
    points.Append( pcloud->GetPoint(p) );
  }
}
