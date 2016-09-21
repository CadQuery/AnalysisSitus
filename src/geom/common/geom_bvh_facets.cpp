//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_bvh_facets.h>

// OCCT includes
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <TopExp_Explorer.hxx>

//-----------------------------------------------------------------------------

//! Creates the accelerating structure with immediate initialization.
//! \param[in] model    CAD model to create the accelerating structure for.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
geom_bvh_facets::geom_bvh_facets(const TopoDS_Shape&  model,
                                 ActAPI_ProgressEntry progress,
                                 ActAPI_PlotterEntry  plotter)
: BVH_PrimitiveSet<double, 4>(),
  Standard_Transient(),
  m_progress(progress),
  m_plotter(plotter)
{
  this->Init(model);
  this->MarkDirty();
}

//-----------------------------------------------------------------------------

//! Returns vertices for a facet with the given 0-based index.
void geom_bvh_facets::GetVertices(const int  index,
                                  BVH_Vec4d& vertex1,
                                  BVH_Vec4d& vertex2,
                                  BVH_Vec4d& vertex3) const
{
  const t_facet& facet = m_facets[index];

  vertex1 = facet.P0;
  vertex2 = facet.P1;
  vertex3 = facet.P2;
}

//-----------------------------------------------------------------------------

//! Adds face to the accelerating structure.
//! \param[in] face face to add.
//! \return true in case of success, false -- otherwise.
bool geom_bvh_facets::AddFace(const TopoDS_Face& face)
{
  TopLoc_Location loc;
  const Handle(Poly_Triangulation)& tris = BRep_Tool::Triangulation(face, loc);
  //
  if( tris.IsNull() )
    return false;

  for ( int t = 1; t <= tris->NbTriangles(); ++t )
  {
    const Poly_Triangle& tri = tris->Triangles().Value(t);

    int n1, n2, n3;
    tri.Get(n1, n2, n3);

    gp_Pnt P0 = tris->Nodes().Value(n1);
    P0.Transform(loc);
    //
    gp_Pnt P1 = tris->Nodes().Value(n2);
    P1.Transform(loc);
    //
    gp_Pnt P2 = tris->Nodes().Value(n3);
    P2.Transform(loc);

    // Create a new facet and store it in the internal collection
    t_facet facet;
    facet.P0 = BVH_Vec4d(P0.X(), P0.Y(), P0.Z(), 0.0);
    facet.P1 = BVH_Vec4d(P1.X(), P1.Y(), P1.Z(), 0.0);
    facet.P2 = BVH_Vec4d(P2.X(), P2.Y(), P2.Z(), 0.0);
    m_facets.push_back(facet);
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Initializes the accelerating structure with the given CAD model.
//! \param[in] model CAD model to prepare the accelerating structure for.
//! \return true in case of success, false -- otherwise.
bool geom_bvh_facets::Init(const TopoDS_Shape& model)
{
  // Prepare builder
  myBuilder = new BVH_BinnedBuilder<double, 4, 32> (5, 32);

  // Initialize with facets taken from faces
  for ( TopExp_Explorer fexp(model, TopAbs_FACE); fexp.More(); fexp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( fexp.Current() );
    //
    if ( !this->AddFace(face) )
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

//! \return number of stored facets.
int geom_bvh_facets::Size() const
{
  return (int) m_facets.size();
}

//-----------------------------------------------------------------------------

//! Builds an elementary box for a facet with the given index.
//! \param[in] index index of the facet of interest.
//! \return AABB for the facet of interest.
BVH_Box<double, 4> geom_bvh_facets::Box(const int index) const
{
  BVH_Box<double, 4> box;
  const t_facet& facet = m_facets[index];

  box.Add(facet.P0);
  box.Add(facet.P1);
  box.Add(facet.P2);

  return box;
}

//-----------------------------------------------------------------------------

//! Calculates center point of a facet with respect to the axis of interest.
//! \param[in] index index of the facet of interest.
//! \param[in] axis  axis of interest.
//! \return center parameter along the straight line.
double geom_bvh_facets::Center(const int index, const int axis) const
{
  const t_facet& facet = m_facets[index];

  if ( axis == 0 )
    return (1.0 / 3.0) * ( facet.P0.x() + facet.P1.x() + facet.P2.x() );
  else if ( axis == 1 )
    return (1.0 / 3.0) * ( facet.P0.y() + facet.P1.y() + facet.P2.y() );

  // The last possibility is "axis == 2"
  return (1.0 / 3.0) * ( facet.P0.z() + facet.P1.z() + facet.P2.z() );
}

//-----------------------------------------------------------------------------

//! Swaps two elements for BVH building.
//! \param[in] index1 first index.
//! \param[in] index2 second index.
void geom_bvh_facets::Swap(const int index1, const int index2)
{
  std::swap(m_facets[index1], m_facets[index2]);
}
