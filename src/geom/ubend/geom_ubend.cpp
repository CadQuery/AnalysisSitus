//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Geom includes
#include <geom_ubend.h>

// OCCT includes
#include <AppDef_Array1OfMultiPointConstraint.hxx>
#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiLine.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GeomAPI.hxx>
#include <gp_Pln.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//! Creates u-bend path.
//! \return u-bend path.
Handle(Geom_BezierCurve) geom_ubend::CreatePath()
{
  TColgp_Array1OfPnt pathPoles(1, 4);
  pathPoles(1) = gp_Pnt(0.,   0.,   0.);
  pathPoles(2) = gp_Pnt(20.,  50.,  0.);
  pathPoles(3) = gp_Pnt(60.,  100., 0.);
  pathPoles(4) = gp_Pnt(150., 0.,   0.);
  return new Geom_BezierCurve(pathPoles);
}

//! Creates u-bend path as topological object.
//! \return u-bend path.
TopoDS_Edge geom_ubend::CreatePathTopo()
{
  return BRepBuilderAPI_MakeEdge( CreatePath() );
}

//! Builds as many sections as requested by user.
//! \param path          [in]  path curve.
//! \param nSections     [in]  requested number of sections.
//! \param side          [in]  side specifier.
//! \param crossSections [out] cross-sectional curves for the given u-bend side.
void geom_ubend::CreateSections(const Handle(Geom_Curve)&                        path,
                                const int                                        nSections,
                                const SideEnum                                   side,
                                NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections)
{
  // Prepare parameter values
  double* u          = new double[nSections];
  u[0]               = path->FirstParameter();
  u[nSections - 1]   = path->LastParameter();
  const double ustep = (u[nSections - 1] - u[0]) / (nSections - 1);
  //
  for ( int i = 1; i < nSections - 1; ++i )
  {
    u[i] = u[0] + i*ustep;
  }

  // Build sections for the given side specifier
  for ( int i = 0; i < nSections; ++i )
  {
    gp_Pnt P;
    gp_Vec V1, V2;
    //
    path->D2(u[i], P, V1, V2);
    //
    gp_Dir d1(V1);
    gp_Dir d2(V2);

    // Create base plane
    gp_Pln pln( gp_Ax3(P, d1, d2) );

    Handle(Geom_Curve) c3d = GeomAPI::To3d(sideCurve2d(side), pln);
    crossSections.Append( Handle(Geom_BSplineCurve)::DownCast(c3d) );
  }

  delete[] u;
}

//! CreateSections() variation returning topological result.
//! \param path          [in]  path curve.
//! \param nSections     [in]  requested number of sections.
//! \param side          [in]  side specifier.
//! \param crossSections [out] cross-sectional curves for the given u-bend side.
void geom_ubend::CreateSectionsTopo(const Handle(Geom_Curve)& path,
                                    const int                 nSections,
                                    const SideEnum            side,
                                    TopoDS_Compound&          crossSections)
{
  NCollection_Sequence<Handle(Geom_BSplineCurve)> geometries;
  CreateSections(path, nSections, side, geometries);

  // Build topo
  BRep_Builder().MakeCompound(crossSections);
  for ( int i = 1; i <= geometries.Length(); ++i )
  {
    BRep_Builder().Add( crossSections, BRepBuilderAPI_MakeEdge( geometries(i) ) );
  }
}

//! Creates four skin surfaces simultaneously from the passed collection of
//! cross-sectional curves.
//! \param crossSections_top    [in]  cross-sections for the top surface.
//! \param crossSections_right  [in]  cross-sections for the right surface.
//! \param crossSections_bottom [in]  cross-sections for the bottom surface.
//! \param crossSections_left   [in]  cross-sections for the left surface.
//! \param skins                [out] skinned surfaces.
void geom_ubend::CreateSkins(const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_top,
                             const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_right,
                             const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_bottom,
                             const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_left,
                             NCollection_Sequence<Handle(Geom_BSplineSurface)>&     skins)
{
  const int nSections = crossSections_top.Length();
  //
  const NCollection_Sequence<Handle(Geom_BSplineCurve)>*
    uIsos[4] = {&crossSections_top, &crossSections_right, &crossSections_bottom, &crossSections_left};

  // Interpolate poles of u-isos to build v-isos
  NCollection_Sequence<Handle(Geom_BSplineCurve)> vIsos;
  {
    // Initialize multiline constraints
    AppDef_Array1OfMultiPointConstraint arr(1, nSections);
    for ( int k = 1; k <= nSections; ++k )
    {
      TColgp_Array1OfPnt mpc_data( 1, crossSections_top(k)    ->NbPoles()
                                    + crossSections_right(k)  ->NbPoles() - 1
                                    + crossSections_bottom(k) ->NbPoles() - 1
                                    + crossSections_left(k)   ->NbPoles() - 2);

      // Curve on the top
      int mpc_idx = 1;
      for ( int p = 1; p <= crossSections_top(k)->NbPoles(); ++p, ++mpc_idx )
        mpc_data(mpc_idx) = crossSections_top(k)->Pole(p);

      // Curve on the right
      for ( int p = 2; p <= crossSections_right(k)->NbPoles(); ++p, ++mpc_idx )
        mpc_data(mpc_idx) = crossSections_right(k)->Pole(p);

      // Curve on the bottom
      for ( int p = 2; p <= crossSections_bottom(k)->NbPoles(); ++p, ++mpc_idx )
        mpc_data(mpc_idx) = crossSections_bottom(k)->Pole(p);

      // Curve on the left
      for ( int p = 2; p < crossSections_left(k)->NbPoles(); ++p, ++mpc_idx )
        mpc_data(mpc_idx) = crossSections_left(k)->Pole(p);

      AppDef_MultiPointConstraint MPC(mpc_data);
      arr(k) = MPC;
    }
    AppDef_MultiLine multiline(arr);

    // Run approximation
    double err3d, err2d;
    AppDef_BSplineCompute bcompute(multiline);
    bcompute.Error(err3d, err2d);

    // Build the resulting geometry
    const AppParCurves_MultiBSpCurve& TheCurve = bcompute.Value();

    // All curves are compatible at this stage: they have the same number
    // of knots, poles, and the same degree.
    Standard_Integer UDegree = TheCurve.Degree();
    const TColStd_Array1OfReal& UKnots = TheCurve.Knots();
    const TColStd_Array1OfInteger& UMults = TheCurve.Multiplicities();
    //
    for ( int k = 0; k < TheCurve.NbCurves(); ++k )
    {
      TColgp_Array1OfPnt Poles( 1, TheCurve.NbPoles() );
      TheCurve.Curve(k + 1, Poles);
      //
      Handle(Geom_BSplineCurve)
        bcurve = new Geom_BSplineCurve( Poles, UKnots, UMults, UDegree);
      vIsos.Append(bcurve);
    }
  }

  for ( int ss = 0; ss < 4; ++ss )
  {
    // STAGE 2: build a surface
    TColgp_Array2OfPnt SurfPoles( 1, vIsos(1)->NbPoles(), 1, uIsos[ss]->Value(1)->NbPoles() );
    {
      // Construct a grid of control points for a surface
      for ( int j = 1; j <= vIsos(1)->NbPoles(); j++ ) {
        for ( int i = 1; i <= uIsos[ss]->Value(1)->NbPoles(); i++ ) {

          int startIdx = 0;
          if ( ss == 0 )
            startIdx = 0;
          else if ( ss == 1 )
            startIdx = 3;
          else if ( ss == 2 )
            startIdx = 6;
          else if ( ss == 3 )
            startIdx = 9;

          if ( startIdx == 9 && i == uIsos[ss]->Value(1)->NbPoles() )
            SurfPoles(j, i) = vIsos(1)->Pole(j);
          else
            SurfPoles(j, i) = vIsos(startIdx + i)->Pole(j);
        }
      }
    }

    // We need to construct knot vectors for our surface: U and V
    Standard_Integer               UDegree = vIsos(1)->Degree();
    const TColStd_Array1OfReal&    UKnots  = vIsos(1)->Knots();
    const TColStd_Array1OfInteger& UMults  = vIsos(1)->Multiplicities();
    //
    Standard_Integer               VDegree = uIsos[ss]->Value(1)->Degree();
    const TColStd_Array1OfReal&    VKnots  = uIsos[ss]->Value(1)->Knots();
    const TColStd_Array1OfInteger& VMults  = uIsos[ss]->Value(1)->Multiplicities();

    //
    std::cout << "U degree = " << UDegree << std::endl;
    std::cout << "V degree = " << VDegree << std::endl;

    std::cout << "Number of poles in row: " << SurfPoles.RowLength() << std::endl;
    std::cout << "Number of poles in col: " << SurfPoles.ColLength() << std::endl;

    std::cout << "\nNum knots (U) = " << UKnots.Length() << std::endl;
    std::cout << "\nU knots: ";
    for ( int k = 1; k <= UKnots.Length(); ++k )
    {
      std::cout << UKnots(k) << " ";
    }
    std::cout << "\nMults of U knots: ";
    for ( int k = 1; k <= UMults.Length(); ++k )
    {
      std::cout << UMults(k) << " ";
    }

    std::cout << "\nNum knots (V) = " << VKnots.Length() << std::endl;
    std::cout << "\nV knots: ";
    for ( int k = 1; k <= VKnots.Length(); ++k )
    {
      std::cout << VKnots(k) << " ";
    }
    std::cout << "\nMults of V knots: ";
    for ( int k = 1; k <= VMults.Length(); ++k )
    {
      std::cout << VMults(k) << " ";
    }
    std::cout << std::endl;
    //

    Handle(Geom_BSplineSurface)
      S = new Geom_BSplineSurface(SurfPoles, UKnots, VKnots, UMults, VMults, UDegree, VDegree, 0, 0);

    // Append surface to the result
    skins.Append(S);
  }
}

//! Topological variant of CreateSkinsTopo().
void geom_ubend::CreateSkinsTopo(const TopoDS_Compound& crossSections_top,
                                 const TopoDS_Compound& crossSections_right,
                                 const TopoDS_Compound& crossSections_bottom,
                                 const TopoDS_Compound& crossSections_left,
                                 TopoDS_Compound&       skins)
{
  NCollection_Sequence<Handle(Geom_BSplineCurve)> geometries_top,
                                                  geometries_right,
                                                  geometries_bottom,
                                                  geometries_left;

  // Collect geometries on the top
  for ( TopExp_Explorer exp(crossSections_top, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& e = TopoDS::Edge( exp.Current() );
    double f, l;
    geometries_top.Append( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(e, f, l) ) );
  }
  // Collect geometries on the right
  for ( TopExp_Explorer exp(crossSections_right, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& e = TopoDS::Edge( exp.Current() );
    double f, l;
    geometries_right.Append( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(e, f, l) ) );
  }
  // Collect geometries on the bottom
  for ( TopExp_Explorer exp(crossSections_bottom, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& e = TopoDS::Edge( exp.Current() );
    double f, l;
    geometries_bottom.Append( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(e, f, l) ) );
  }
  // Collect geometries on the left
  for ( TopExp_Explorer exp(crossSections_left, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& e = TopoDS::Edge( exp.Current() );
    double f, l;
    geometries_left.Append( Handle(Geom_BSplineCurve)::DownCast( BRep_Tool::Curve(e, f, l) ) );
  }

  // Run skinning
  NCollection_Sequence<Handle(Geom_BSplineSurface)> surfaces;
  CreateSkins(geometries_top, geometries_right, geometries_bottom, geometries_left, surfaces);

  // Build faces
  BRep_Builder().MakeCompound(skins);
  for ( int s = 1; s <= surfaces.Length(); ++s )
  {
    TopoDS_Face f = BRepBuilderAPI_MakeFace( surfaces(s), Precision::Confusion() ).Face();
    BRep_Builder().Add(skins, f);
  }
}

//-----------------------------------------------------------------------------

//! Builds side section curve for one of the 4 u-bend surfaces.
//! \param side [in] side specifier.
//! \return side curve in sketch space following the Frenet frame.
Handle(Geom2d_BSplineCurve) geom_ubend::sideCurve2d(const SideEnum side)
{
  TColgp_Array1OfPnt2d    poles(1, 4);
  TColStd_Array1OfReal    knots(1, 2);
  TColStd_Array1OfInteger mults(1, 2);
  //
  const int degree = 3;
  //
  knots(1) = 0.0;
  knots(2) = 1.0;
  //
  mults(1) = degree + 1;
  mults(2) = degree + 1;

  // Curve 1
  switch ( side )
  {
    case Side_Top:
    {
      poles(1) = gp_Pnt2d(-4, 4);
      poles(2) = gp_Pnt2d(-2, 4);
      poles(3) = gp_Pnt2d( 2, 4);
      poles(4) = gp_Pnt2d( 4, 4);
      break;
    }
    case Side_Right:
    {
      poles(1) = gp_Pnt2d(4,  4);
      poles(2) = gp_Pnt2d(4,  2);
      poles(3) = gp_Pnt2d(4, -2);
      poles(4) = gp_Pnt2d(4, -4);
      break;
    }
    case Side_Bottom:
    {
      poles(1) = gp_Pnt2d( 4, -4);
      poles(2) = gp_Pnt2d( 2, -4);
      poles(3) = gp_Pnt2d(-2, -4);
      poles(4) = gp_Pnt2d(-4, -4);
      break;
    }
    case Side_Left:
    {
      poles(1) = gp_Pnt2d(-4, -4);
      poles(2) = gp_Pnt2d(-4, -2);
      poles(3) = gp_Pnt2d(-4,  2);
      poles(4) = gp_Pnt2d(-4,  4);
      break;
    }
    default: break;
  }

  return new Geom2d_BSplineCurve(poles, knots, mults, degree, 0);
}
