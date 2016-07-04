//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_contour_orientation.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Default constructor.
geom_contour_orientation::geom_contour_orientation()
{}

//! Calculates orientation vector by contour.
//! \param Contour       [in]     contour to calculate the orientation vector for.
//! \param precision     [in]     precision.
//! \param result_Center [out]    calculated center point.
//! \param result_Vector [out]    resulting orientation vector.
//! \param Journal       [in/out] journal instance.
//! \return true in case of success, false -- otherwise.
bool geom_contour_orientation::operator()(const TopoDS_Shape&  Contour,
                                          const double         precision,
                                          gp_Pnt&              result_Center,
                                          gp_Vec&              result_Vector,
                                          ActAPI_ProgressEntry Journal) const
{
  /* ==============================================
   *  Prepare discretization and find middle point
   * ============================================== */

  TColgp_SequenceOfPnt pts;
  gp_XYZ Pm;
  int nContourVerts = 0;
  for ( TopExp_Explorer exp(Contour, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    double f, l;
    BRep_Tool::Range(E, f, l);
    BRepAdaptor_Curve bac(E);
    GCPnts_TangentialDeflection Defl(bac, f, l, precision, precision);

    for ( int pt_idx = 1; pt_idx <= Defl.NbPoints(); ++pt_idx )
    {
      gp_Pnt P = Defl.Value(pt_idx);
      pts.Append(P);
      ++nContourVerts;
      Pm += P.XYZ();
    }
  }
  if ( nContourVerts )
    Pm /= nContourVerts;

  /* ========================
   *  Compute average normal
   * ======================== */

  gp_XYZ ori;
  const int nPts = pts.Length();
  int nTerms = 0;
  for ( int pt_idx = 1; pt_idx <= nPts; ++pt_idx )
  {
    const int pt_idx_next = ( (pt_idx == nPts) ? 1 : (pt_idx + 1) );
    gp_XYZ P1 = pts(pt_idx).XYZ();
    gp_XYZ P2 = pts(pt_idx_next).XYZ();

    gp_XYZ n = (P1 - Pm).Crossed(P2 - Pm);
    if ( n.SquareModulus() > Square(precision) )
    {
      ori += n.Normalized();
      ++nTerms;
    }
  }
  if ( nTerms )
    ori /= nTerms;

  result_Center = Pm;

  if ( !ori.IsEqual( gp::Origin().XYZ(), RealEpsilon() ) )
    ori.Normalize();
  else
    Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_orientation.FailedToCalcOrient" );

  result_Vector = ori;
  return true;
}
