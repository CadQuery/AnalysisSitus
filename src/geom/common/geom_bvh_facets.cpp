//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_bvh_facets.h>

// Geometry includes
#include <geom_bvh_iterator.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <BVH_LinearBuilder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

//-----------------------------------------------------------------------------

//! Creates the accelerating structure with immediate initialization.
//! \param[in] model       CAD model to create the accelerating structure for.
//! \param[in] builderType type of builder to use.
//! \param[in] progress    progress notifier.
//! \param[in] plotter     imperative plotter.
geom_bvh_facets::geom_bvh_facets(const TopoDS_Shape&  model,
                                 const BuilderType    builderType,
                                 ActAPI_ProgressEntry progress,
                                 ActAPI_PlotterEntry  plotter)
: BVH_PrimitiveSet<double, 4> (),
  Standard_Transient          (),
  m_progress                  (progress),
  m_plotter                   (plotter),
  m_fBoundingDiag             (0.0)
{
  this->init(model, builderType);
  this->MarkDirty();
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

//-----------------------------------------------------------------------------

//! Returns vertices for a facet with the given 0-based index.
inline void geom_bvh_facets::GetVertices(const int  index,
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

//! \return characteristic diagonal of the full model.
double geom_bvh_facets::GetBoundingDiag() const
{
  return m_fBoundingDiag;
}

//-----------------------------------------------------------------------------

//! Dumps the primitive set to the plotter.
//! \param[in] IV imperative plotter to dump to.
void geom_bvh_facets::Dump(ActAPI_PlotterEntry IV)
{
  // Access (build) hierarchy of boxes
  const NCollection_Handle< BVH_Tree<double, 4> >& bvh = this->BVH();
  if ( bvh.IsNull() )
  {
    std::cout << "Error: BVH construction failed" << std::endl;
    return;
  }

  // Prepare a topological structure to store BVH primitives in explicit form
  TopoDS_Compound comp_left, comp_right;
  BRep_Builder BB;
  BB.MakeCompound(comp_left);
  BB.MakeCompound(comp_right);

  // Loop over the BVH nodes
  for ( geom_bvh_iterator it(bvh); it.More(); it.Next() )
  {
    const BVH_Vec4i& nodeData = it.Current();

    if ( !it.IsLeaf() )
    {
      const BVH_Vec4d& minCorner_Left  = bvh->MinPoint( nodeData.y() );
      const BVH_Vec4d& maxCorner_Left  = bvh->MaxPoint( nodeData.y() );
      const BVH_Vec4d& minCorner_Right = bvh->MinPoint( nodeData.z() );
      const BVH_Vec4d& maxCorner_Right = bvh->MaxPoint( nodeData.z() );

      // Left box
      {
        gp_Pnt Pmin( minCorner_Left.x(), minCorner_Left.y(), minCorner_Left.z() );
        gp_Pnt Pmax( maxCorner_Left.x(), maxCorner_Left.y(), maxCorner_Left.z() );

        const gp_Pnt P2 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmin.Z() );
        const gp_Pnt P3 = gp_Pnt( Pmax.X(), Pmax.Y(), Pmin.Z() );
        const gp_Pnt P4 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmin.Z() );
        const gp_Pnt P5 = gp_Pnt( Pmin.X(), Pmin.Y(), Pmax.Z() );
        const gp_Pnt P6 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmax.Z() );
        const gp_Pnt P8 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmax.Z() );

        if ( Pmin.Distance(P2) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(Pmin, P2) );

        if ( P2.Distance(P3) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P2, P3) );

        if ( P3.Distance(P4) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P3, P4) );

        if ( P4.Distance(Pmin) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P4, Pmin) );

        if ( P5.Distance(P6) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P5, P6) );

        if ( P6.Distance(Pmax) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P6, Pmax) );

        if ( Pmax.Distance(P8) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(Pmax, P8) );

        if ( P8.Distance(P5) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P8, P5) );

        if ( P6.Distance(P2) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P6, P2) );

        if ( Pmax.Distance(P3) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(Pmax, P3) );

        if ( P8.Distance(P4) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P8, P4) );

        if ( P5.Distance(Pmin) > 1.0e-6 )
          BB.Add( comp_left, BRepBuilderAPI_MakeEdge(P5, Pmin) );
      }

      // Right box
      {
        gp_Pnt Pmin( minCorner_Right.x(), minCorner_Right.y(), minCorner_Right.z() );
        gp_Pnt Pmax( maxCorner_Right.x(), maxCorner_Right.y(), maxCorner_Right.z() );

        const gp_Pnt P2 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmin.Z() );
        const gp_Pnt P3 = gp_Pnt( Pmax.X(), Pmax.Y(), Pmin.Z() );
        const gp_Pnt P4 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmin.Z() );
        const gp_Pnt P5 = gp_Pnt( Pmin.X(), Pmin.Y(), Pmax.Z() );
        const gp_Pnt P6 = gp_Pnt( Pmax.X(), Pmin.Y(), Pmax.Z() );
        const gp_Pnt P8 = gp_Pnt( Pmin.X(), Pmax.Y(), Pmax.Z() );

        if ( Pmin.Distance(P2) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(Pmin, P2) );

        if ( P2.Distance(P3) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P2, P3) );

        if ( P3.Distance(P4) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P3, P4) );

        if ( P4.Distance(Pmin) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P4, Pmin) );

        if ( P5.Distance(P6) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P5, P6) );

        if ( P6.Distance(Pmax) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P6, Pmax) );

        if ( Pmax.Distance(P8) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(Pmax, P8) );

        if ( P8.Distance(P5) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P8, P5) );

        if ( P6.Distance(P2) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P6, P2) );

        if ( Pmax.Distance(P3) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(Pmax, P3) );

        if ( P8.Distance(P4) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P8, P4) );

        if ( P5.Distance(Pmin) > 1.0e-6 )
          BB.Add( comp_right, BRepBuilderAPI_MakeEdge(P5, Pmin) );
      }
    }
  }

  // Draw BVH
  IV.CLEAN();
  IV.DRAW_SHAPE(comp_left,  Color_Yellow, 1.0, true, "BVH Left");
  IV.DRAW_SHAPE(comp_right, Color_Green, 1.0, true, "BVH Right");
}

//-----------------------------------------------------------------------------

//! Initializes the accelerating structure with the given CAD model.
//! \param[in] model       CAD model to prepare the accelerating structure for.
//! \param[in] builderType type of builder to use.
//! \return true in case of success, false -- otherwise.
bool geom_bvh_facets::init(const TopoDS_Shape& model,
                           const BuilderType   builderType)
{
  m_model = model;

  // Prepare builder
  if ( builderType == Builder_Binned )
    myBuilder = new BVH_BinnedBuilder<double, 4, 32>(5, 32);
  else
    myBuilder = new BVH_LinearBuilder<double, 4>(5, 32);

  // Explode shape on faces to get face indices
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(m_model, TopAbs_FACE, faces);

  // Initialize with facets taken from faces
  for ( int fidx = 1; fidx <= faces.Extent(); ++fidx )
  {
    const TopoDS_Face& face = TopoDS::Face( faces(fidx) );
    //
    if ( !this->addFace(face, fidx) )
      return false;
  }

  // Calculate bounding diagonal
  Bnd_Box aabb;
  BRepBndLib::Add(m_model, aabb);
  m_fBoundingDiag = ( aabb.CornerMax().XYZ() - aabb.CornerMin().XYZ() ).Modulus();

  return true;
}

//-----------------------------------------------------------------------------

//! Adds face to the accelerating structure.
//! \param[in] face     face to add.
//! \param[in] face_idx index of the face being added.
//! \return true in case of success, false -- otherwise.
bool geom_bvh_facets::addFace(const TopoDS_Face& face,
                              const int          face_idx)
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
    t_facet facet(face_idx);
    //
    facet.P0 = BVH_Vec4d(P0.X(), P0.Y(), P0.Z(), 0.0);
    facet.P1 = BVH_Vec4d(P1.X(), P1.Y(), P1.Z(), 0.0);
    facet.P2 = BVH_Vec4d(P2.X(), P2.Y(), P2.Z(), 0.0);
    //
    m_facets.push_back(facet);
  }

  return true;
}
