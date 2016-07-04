//-----------------------------------------------------------------------------
// Created on: 26 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_build_stock.h>

// Geometry includes
#include <feature_aag.h>
#include <geom_build_contours.h>
#include <geom_find_same_hosts.h>
#include <geom_intersect_curves.h>
#include <geom_intersect_surfaces.h>
#include <geom_utils.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <TColGeom_SequenceOfSurface.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>

//! Constructor.
//! \param progress [in] Progress Entry.
//! \param plotter  [in] Imperative Plotter.
geom_build_stock::geom_build_stock(ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//! Builds a stock volume from the given part.
//! \param part [in] workpiece to build a stock for.
//! \return true in case of success, false -- otherwise.
bool geom_build_stock::operator()(const TopoDS_Shape& part)
{
  /* ==================================
   *  STAGE 01: Find convex-only faces
   * ================================== */

  // Build AAG
  Handle(feature_aag) aag = new feature_aag(part);

  // Find convex-only faces
  TopTools_IndexedMapOfShape convex;
  if ( !aag->FindConvexOnly(convex) )
  {
    std::cout << "There are no convex-only faces in AAG" << std::endl;
    return false;
  }

  /* =====================================
   *  STAGE 02: Find unique host surfaces
   * ===================================== */

  // Keep only planes
  TopTools_IndexedMapOfShape planar;
  for ( int f = 1; f <= convex.Extent(); ++f )
  {
    const TopoDS_Face&   F = TopoDS::Face( convex(f) );
    Handle(Geom_Surface) S = BRep_Tool::Surface(F);

    if ( !S->IsKind( STANDARD_TYPE(Geom_Plane) ) )
      continue;

    planar.Add(F);
  }

  // Find same-host faces
  geom_find_same_hosts::t_groups groups;
  geom_find_same_hosts finder(planar);
  finder(groups);
  //
  std::cout << "\tFound " << groups.Length() << " groups of hosts" << std::endl;

  // Collect seed surfaces
  TColGeom_SequenceOfSurface hosts;
  //
  for ( int g = 1; g <= groups.Length(); ++g )
  {
    const TopoDS_Face& F = TopoDS::Face( planar( groups(g).GetMinimalMapped() ) );

    // Get UV bounds
    double uMin, uMax, vMin, vMax;
    BRepTools::UVBounds(F, uMin, uMax, vMin, vMax);
    //
    const double uLimit = Max( Abs(uMin), Abs(uMax) )*1.5;
    const double vLimit = Max( Abs(vMin), Abs(vMax) )*1.5;

    // Add host surface to the result
    Handle(Geom_Surface) host = BRep_Tool::Surface(F);
    hosts.Append(host);

    // Draw surface
    this->Plotter().DRAW_SURFACE( host, uLimit, vLimit, Quantity_NOC_SPRINGGREEN, 0.5 );
  }

  /* ===================================
   *  STAGE 03: Intersect host surfaces
   * =================================== */

  // Intersect surfaces
  geom_intersection_curves_ss icurves;
  geom_intersect_surfaces intersector( Precision::Confusion() );
  intersector(hosts, icurves);

  // Build wires from the intersection results
  for ( int c = 1; c <= icurves.Length(); ++c )
  {
    const Handle(geom_intersection_curve_ss)& icurve = icurves(c);

    // Draw contour curve
    this->Plotter().DRAW_CURVE( icurve->C, Quantity_NOC_SKYBLUE );
  }

  /* ============================
   *  STAGE 04: Intersect curves
   * ============================ */

  // Use bounding box for limiting curves
  Bnd_Box PartBox;
  BRepBndLib::Add(part, PartBox);
  const double delta = ( PartBox.CornerMax().XYZ() - PartBox.CornerMin().XYZ() ).Modulus();

  // Intersect curves
  geom_intersection_points_cc ipoints;
  geom_intersect_curves cintersector( Precision::Confusion(), delta,  this->Progress(), this->Plotter() );
  cintersector(icurves, ipoints);

  /* ====================================
   *  STAGE 05: Build contours and faces
   * ==================================== */

  TopTools_IndexedMapOfShape contours, faces;
  //
  geom_build_contours contourer( this->Progress(), this->Plotter() );
  if ( !contourer(ipoints, contours, faces) )
  {
    std::cout << "Error: cannot build contours" << std::endl;
    return false;
  }

  // Draw contours
  for ( int c = 1; c <= contours.Extent(); ++c )
  {
    const TopoDS_Wire& W = TopoDS::Wire( contours(c) );
    //
    this->Plotter().DRAW_SHAPE(W, 0.5, "Contour");
  }

  // Build a shell from the reconstructed faces
  TopoDS_Shell stockShell;
  //
  BRep_Builder BB;
  BB.MakeShell(stockShell);
  //
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    const TopoDS_Face& F = TopoDS::Face( faces(f) );
    BB.Add(stockShell, F);
  }


  this->Plotter().CLEAN();
  this->Plotter().DRAW_SHAPE(stockShell, "Stock_shell");

  /* =================================
   *  STAGE 06: Sew and build a solid
   * ================================= */

  if ( !geom_utils::Sew( stockShell, Precision::Confusion() ) )
  {
    std::cout << "Error: cannot sew the stock shell" << std::endl;
    return false;
  }

  // Build solid
  BB.MakeSolid(m_result);
  BB.Add(m_result, stockShell);

  /// TODO
  m_result.Reverse();

  return true;
}
