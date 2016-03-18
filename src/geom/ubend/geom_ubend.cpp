//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Geom includes
#include <geom_ubend.h>

// OCCT includes
#include <AppDef_Array1OfMultiPointConstraint.hxx>
#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiLine.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_FastSewing.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GeomAPI.hxx>
#include <gp_Pln.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

const double tang_modulus = 1000.0;
//
gp_Pnt2d P1_Original  (-4,  4);
gp_Pnt2d P2_Original  (-2,  4);
gp_Pnt2d P3_Original  ( 2,  4);
gp_Pnt2d P4_Original  ( 4,  4);
gp_Pnt2d P5_Original  ( 4,  2);
gp_Pnt2d P6_Original  ( 4, -2);
gp_Pnt2d P7_Original  ( 4, -4);
gp_Pnt2d P8_Original  ( 2, -4);
gp_Pnt2d P9_Original  (-2, -4);
gp_Pnt2d P10_Original (-4, -4);
gp_Pnt2d P11_Original (-4, -2);
gp_Pnt2d P12_Original (-4,  2);

//-----------------------------------------------------------------------------

//! Returns two-dimensional pole in a section corresponding to the passed
//! parameter along the path.
//! \param pole  [in] pole of interest.
//! \param xLaw  [in] evolution law for X coordinate.
//! \param yLaw  [in] evolution law for Y coordinate.
//! \param u     [in] parameter along the path.
//! \return coordinates of the pole of interest.
gp_Pnt2d geom_ubend::GetSectionPole(const PoleEnum                     pole,
                                    const Handle(calculus_design_law)& xLaw,
                                    const Handle(calculus_design_law)& yLaw,
                                    const double                       u)
{
  gp_Pnt2d coord;
  //
  switch ( pole )
  {
    case Pole_P1:  coord = P1_Original;  break;
    case Pole_P2:  coord = P2_Original;  break;
    case Pole_P3:  coord = P3_Original;  break;
    case Pole_P4:  coord = P4_Original;  break;
    case Pole_P5:  coord = P5_Original;  break;
    case Pole_P6:  coord = P6_Original;  break;
    case Pole_P7:  coord = P7_Original;  break;
    case Pole_P8:  coord = P8_Original;  break;
    case Pole_P9:  coord = P9_Original;  break;
    case Pole_P10: coord = P10_Original; break;
    case Pole_P11: coord = P11_Original; break;
    case Pole_P12: coord = P12_Original; break;
    default: break;
  }

  // Add evolution
  coord.SetX( coord.X() + xLaw->operator()(u) );
  coord.SetY( coord.Y() + yLaw->operator()(u) );

  // Return
  return coord;
}

//! Creates evolution law for the given control point.
//! \param pole  [in] pole of interest.
//! \param coord [in] coordinate of interest.
//! \param last  [in] last parameter value.
//! \return evolution law.
Handle(calculus_design_law)
  geom_ubend::CreateExplicitLaw(const PoleEnum  pole,
                                const CoordEnum coord,
                                const double    last)
{
  Handle(calculus_design_law) law;
  //
  switch ( pole )
  {
    case Pole_P1:
    {
      Handle(HRealArray) poles = new HRealArray(0, 2);
      poles->SetValue(0, coord == Coord_X ? 0.0 : 0.0 );
      poles->SetValue(1, coord == Coord_X ? 4.0 : 4.0);
      poles->SetValue(2, coord == Coord_X ? 0.0 : 0.0);
      //
      law = new calculus_design_law(poles, last);
      break;
    }
    case Pole_P2:
    case Pole_P3:
    case Pole_P4:
    case Pole_P5:
    case Pole_P6:
    {
      Handle(HRealArray) poles = new HRealArray(0, 2);
      poles->SetValue(0, coord == Coord_X ? 0.0 :  0.0);
      poles->SetValue(1, coord == Coord_X ? 4.0 :  4.0);
      poles->SetValue(2, coord == Coord_X ? 6.0 : -1.0);
      //
      law = new calculus_design_law(poles, last);
      break;
    }
    case Pole_P7:
    case Pole_P8:
    case Pole_P9:
    case Pole_P10:
    case Pole_P11:
    case Pole_P12:
    {
      law = NullExplicitLaw(last);
      break;
    }
    default: break;
  }
  //
  return law;
}

//! Creates explicit law which is a constant zero.
//! \param last [in] last parameter value.
//! \return evolution law (no evolution).
Handle(calculus_design_law) geom_ubend::NullExplicitLaw(const double last)
{
  Handle(HRealArray) poles = new HRealArray(0, 1);
  poles->SetValue(0, 0.0);
  poles->SetValue(1, 0.0);
  //
  Handle(calculus_design_law) law = new calculus_design_law(poles, last);
  //
  return law;
}

//! This function allows to switch from a pair of explicit functions to
//! a single parametric function, which is actually a curve.
//! \param xLaw [in] explicit evolution law for X coordinate.
//! \param yLaw [in] explicit evolution law for Y coordinate.
Handle(Geom2d_BSplineCurve)
  geom_ubend::CreateParametricLaw(const Handle(calculus_design_law)& xLaw,
                                  const Handle(calculus_design_law)& yLaw)
{
  // Access poles
  Handle(HRealArray) poles_X = xLaw->ChangePoles();
  Handle(HRealArray) poles_Y = yLaw->ChangePoles();
  //
  if ( poles_X.IsNull() || poles_Y.IsNull() )
    return NULL;

  // Populate array of two-dimensional control points
  TColgp_Array1OfPnt2d poles( 1, poles_X->Length() );
  for ( int i = 1; i <= poles_X->Length(); ++i )
  {
    poles(i) = gp_Pnt2d( poles_X->Value(i - 1), poles_Y->Value(i - 1) );
  }

  // Access knots
  Handle(HRealArray) knots_X = xLaw->ChangeKnots();
  Handle(HRealArray) knots_Y = yLaw->ChangeKnots();
  //
  if ( knots_X.IsNull() || knots_Y.IsNull() )
    return NULL;

  // Populate array of knots
  TColStd_Array1OfReal knots( 1, knots_Y->Length() );
  for ( int i = 1; i <= knots_X->Length(); ++i )
  {
    knots(i) = knots_X->Value(i - 1); // NOTICE that X and Y knots should be identical
  }

  // Access multiplicities
  Handle(HIntArray) mults_X = xLaw->ChangeMults();
  Handle(HIntArray) mults_Y = yLaw->ChangeMults();
  //
  if ( mults_X.IsNull() || mults_Y.IsNull() )
    return NULL;

  // Populate array of multiplicities
  TColStd_Array1OfInteger mults( 1, mults_Y->Length() );
  for ( int i = 1; i <= mults_X->Length(); ++i )
  {
    mults(i) = mults_X->Value(i - 1); // NOTICE that X and Y multiplicities should be identical
  }

  // Choose degree
  const int degree = poles_X->Length() - 1;

  // Build parametric representation of evolution laws (evolution curve)
  return new Geom2d_BSplineCurve(poles, knots, mults, degree, 0);
}

//! Creates u-bend path.
//! \return u-bend path.
Handle(Geom_BezierCurve) geom_ubend::CreatePath()
{
  TColgp_Array1OfPnt pathPoles(1, 4);
  pathPoles(1) = gp_Pnt(0.,  0.,  0.);
  pathPoles(2) = gp_Pnt(0.,  50., 0.);
  pathPoles(3) = gp_Pnt(50., 50., 0.);
  pathPoles(4) = gp_Pnt(52., 0.,  0.);
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
//! \param xLaws         [in]  evolution laws for X coordinate.
//! \param yLaws         [in]  evolution laws for Y coordinate.
//! \param crossSections [out] cross-sectional curves for the given u-bend side.
void geom_ubend::CreateSections(const Handle(Geom_Curve)&                        path,
                                const int                                        nSections,
                                const SideEnum                                   side,
                                const t_coord_laws&                              xLaws,
                                const t_coord_laws&                              yLaws,
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
    // TODO: don't use 2-nd derivative, because in case of inflection
    //       point it can give bad result

    gp_Pnt P;
    gp_Vec V1, V2;
    //
    path->D2(u[i], P, V1, V2);
    //
    gp_Dir d1(V1);
    gp_Dir d2(V2);

    // Create base plane
    gp_Pln pln( gp_Ax3(P, d1, d2) );

    // Create side curve
    Handle(Geom_Curve) c3d = GeomAPI::To3d(sideCurve2d(side, xLaws, yLaws, u[i]), pln);
    crossSections.Append( Handle(Geom_BSplineCurve)::DownCast(c3d) );
  }

  delete[] u;
}

//! CreateSections() variation returning topological result.
//! \param path          [in]  path curve.
//! \param nSections     [in]  requested number of sections.
//! \param side          [in]  side specifier.
//! \param xLaws         [in]  evolution laws for X coordinate.
//! \param yLaws         [in]  evolution laws for Y coordinate.
//! \param crossSections [out] cross-sectional curves for the given u-bend side.
void geom_ubend::CreateSectionsTopo(const Handle(Geom_Curve)& path,
                                    const int                 nSections,
                                    const SideEnum            side,
                                    const t_coord_laws&       xLaws,
                                    const t_coord_laws&       yLaws,
                                    TopoDS_Compound&          crossSections)
{
  NCollection_Sequence<Handle(Geom_BSplineCurve)> geometries;
  CreateSections(path, nSections, side, xLaws, yLaws, geometries);

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
  if ( crossSections_top.Length() != crossSections_right.Length() ||
       crossSections_top.Length() != crossSections_bottom.Length() ||
       crossSections_top.Length() != crossSections_left.Length() )
    Standard_ProgramError::Raise("Number of side sections is not the same");

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
                                    + crossSections_left(k)   ->NbPoles() - 2 );

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

      // Put tangency constraints for the first and last sections
      if ( k == 1 || k == nSections )
      {
        TColgp_Array1OfVec tang_data( 1, crossSections_top(k)    ->NbPoles()
                                       + crossSections_right(k)  ->NbPoles() - 1
                                       + crossSections_bottom(k) ->NbPoles() - 1
                                       + crossSections_left(k)   ->NbPoles() - 2 );

        for ( int p = 1; p <= tang_data.Length(); ++p )
          tang_data(p) = gp_Vec(0, (k == 1) ? tang_modulus : -tang_modulus, 0);

        // Create constraint with tangency
        AppDef_MultiPointConstraint MPC(mpc_data, tang_data);
        arr(k) = MPC;
      }
      else
      {
        AppDef_MultiPointConstraint MPC(mpc_data);
        arr(k) = MPC;
      }
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
    int CurveDegree = TheCurve.Degree();
    const TColStd_Array1OfReal& CurveKnots = TheCurve.Knots();
    const TColStd_Array1OfInteger& CurveMults = TheCurve.Multiplicities();
    int nbPoles = TheCurve.NbPoles();
    //
    for ( int k = 0; k < TheCurve.NbCurves(); ++k )
    {
      TColgp_Array1OfPnt Poles(1, nbPoles);
      TheCurve.Curve(k + 1, Poles);
      //
      Handle(Geom_BSplineCurve)
        bcurve = new Geom_BSplineCurve(Poles, CurveKnots, CurveMults, CurveDegree);
      vIsos.Append(bcurve);
    }
  }

  for ( int ss = Side_Top; ss <= Side_Left; ++ss )
  {
    // STAGE 2: build a surface
    TColgp_Array2OfPnt SurfPoles( 1, vIsos(1)->NbPoles(), 1, uIsos[ss]->Value(1)->NbPoles() );
    {
      // Construct a grid of control points for a surface
      for ( int j = 1; j <= vIsos(1)->NbPoles(); j++ )
      {
        for ( int i = 1; i <= uIsos[ss]->Value(1)->NbPoles(); i++ )
        {

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
    int                            UDegree = vIsos(1)->Degree();
    const TColStd_Array1OfReal&    UKnots  = vIsos(1)->Knots();
    const TColStd_Array1OfInteger& UMults  = vIsos(1)->Multiplicities();
    //
    int                            VDegree = uIsos[ss]->Value(1)->Degree();
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

//! Builds a shell topological data structure from the passed unconnected
//! skin faces.
//! \param skins [in] skin faces.
//! \return topological shell.
TopoDS_Shape geom_ubend::AsShell(const TopoDS_Compound& skins)
{
  BRepBuilderAPI_FastSewing sewing;
  //
  for ( TopExp_Explorer exp(skins, TopAbs_FACE); exp.More(); exp.Next() )
  {
    sewing.Add( exp.Current() );
  }
  sewing.Perform();
  return sewing.GetResult();
}

//-----------------------------------------------------------------------------

//! Builds side section curve for one of the 4 u-bend surfaces.
//! \param side  [in] side specifier.
//! \param xLaws [in] evolution laws for X coordinate.
//! \param yLaws [in] evolution laws for Y coordinate.
//! \param u     [in] parameter along the path.
//! \return side curve in sketch space following the Frenet frame.
Handle(Geom2d_BSplineCurve) geom_ubend::sideCurve2d(const SideEnum      side,
                                                    const t_coord_laws& xLaws,
                                                    const t_coord_laws& yLaws,
                                                    const double        u)
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

  switch ( side )
  {
    case Side_Top:
    {
      poles(1) = GetSectionPole(Pole_P1, xLaws(Pole_P1), yLaws(Pole_P1), u);
      poles(2) = GetSectionPole(Pole_P2, xLaws(Pole_P2), yLaws(Pole_P2), u);
      poles(3) = GetSectionPole(Pole_P3, xLaws(Pole_P3), yLaws(Pole_P3), u);
      poles(4) = GetSectionPole(Pole_P4, xLaws(Pole_P4), yLaws(Pole_P4), u);
      break;
    }
    case Side_Right:
    {
      poles(1) = GetSectionPole(Pole_P4, xLaws(Pole_P4), yLaws(Pole_P4), u);
      poles(2) = GetSectionPole(Pole_P5, xLaws(Pole_P5), yLaws(Pole_P5), u);
      poles(3) = GetSectionPole(Pole_P6, xLaws(Pole_P6), yLaws(Pole_P6), u);
      poles(4) = GetSectionPole(Pole_P7, xLaws(Pole_P7), yLaws(Pole_P7), u);
      break;
    }
    case Side_Bottom:
    {
      poles(1) = GetSectionPole(Pole_P7,  xLaws(Pole_P7),  yLaws(Pole_P7),  u);
      poles(2) = GetSectionPole(Pole_P8,  xLaws(Pole_P8),  yLaws(Pole_P8),  u);
      poles(3) = GetSectionPole(Pole_P9,  xLaws(Pole_P9),  yLaws(Pole_P9),  u);
      poles(4) = GetSectionPole(Pole_P10, xLaws(Pole_P10), yLaws(Pole_P10), u);
      break;
    }
    case Side_Left:
    {
      poles(1) = GetSectionPole(Pole_P10, xLaws(Pole_P10), yLaws(Pole_P10), u);
      poles(2) = GetSectionPole(Pole_P11, xLaws(Pole_P11), yLaws(Pole_P11), u);
      poles(3) = GetSectionPole(Pole_P12, xLaws(Pole_P12), yLaws(Pole_P12), u);
      poles(4) = GetSectionPole(Pole_P1,  xLaws(Pole_P1),  yLaws(Pole_P1),  u);
      break;
    }
    default: break;
  }

  return new Geom2d_BSplineCurve(poles, knots, mults, degree, 0);
}
