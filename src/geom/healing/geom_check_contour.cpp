//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_check_contour.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
geom_check_contour::geom_check_contour()
{
}

//! Returns true if the passed contour is a single wire. Returns false
//! otherwise.
//! \param Contour [in]     contour to check.
//! \param Journal [in/out] journal instance.
//! \return true/false.
bool geom_check_contour::Check_isSingleWire(const TopoDS_Shape&  Contour,
                                            ActAPI_ProgressEntry Journal) const
{
  int nWires = 0;
  for ( TopExp_Explorer exp(Contour, TopAbs_WIRE); exp.More(); exp.Next() )
  {
    nWires++;
  }
  if ( nWires > 1 )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_check_contour.NotSingleWire" << Contour );
    return false;
  }
  return true;
}

//! Checks whether the passed contour is connected or not.
//! \param Contour        [in]     contour to check.
//! \param doCheckSharing [in]     indicates whether to check topological
//!                                sharing. If not, the passed precision
//!                                is used to check geometric coincidence.
//! \param prec           [in]     precision to check geometric coincidence.
//! \param Journal        [in/out] journal instance.
//! \return true/false.
bool geom_check_contour::Check_connectedWire(const TopoDS_Shape&  Contour,
                                             const bool           doCheckSharing,
                                             const double         prec,
                                             ActAPI_ProgressEntry Journal) const
{
  // Extract wire
  TopExp_Explorer exp(Contour, TopAbs_WIRE);
  TopoDS_Wire W = TopoDS::Wire( exp.Current() );

  // Prepare wire data
  Handle(ShapeExtend_WireData) WD = new ShapeExtend_WireData(W);
  const int nEdges = WD->NbEdges();
  if ( nEdges == 1 )
  {
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve( WD->Edge(1), f, l );

    if ( C.IsNull() )
      return false;

    if ( C->IsPeriodic() )
      return true;

    gp_Pnt Pf = C->Value( C->FirstParameter() );
    gp_Pnt Pl = C->Value( C->LastParameter() );

    return Pf.Distance(Pl) < gp::Resolution();
  }
  else if ( !nEdges )
    return false;

  // Check connectivity
  for ( int e = 1; e <= nEdges; ++e )
  {
    const int e_next = ( e == nEdges ? 1 : e + 1 );

    TopoDS_Edge E1 = WD->Edge(e);
    TopoDS_Edge E2 = WD->Edge(e_next);

    TopoDS_Vertex V1_f = ShapeAnalysis_Edge().FirstVertex (E1);
    TopoDS_Vertex V1_l = ShapeAnalysis_Edge().LastVertex  (E1);
    TopoDS_Vertex V2_f = ShapeAnalysis_Edge().FirstVertex (E2);
    TopoDS_Vertex V2_l = ShapeAnalysis_Edge().LastVertex  (E2);

    if ( doCheckSharing )
    {
      if ( !V1_f.IsSame(V2_l) && !V1_f.IsSame(V2_f) && !V1_l.IsSame(V2_f) && !V1_l.IsSame(V2_l) )
        return false;
    }
    else
    {
      gp_Pnt P1_f = BRep_Tool::Pnt(V1_f);
      gp_Pnt P1_l = BRep_Tool::Pnt(V1_l);
      gp_Pnt P2_f = BRep_Tool::Pnt(V2_f);
      gp_Pnt P2_l = BRep_Tool::Pnt(V2_l);

      bool isConnected = false;

      if ( P1_f.Distance(P2_f) < prec )
        isConnected = true;
      else if ( P1_f.Distance(P2_l) < prec )
        isConnected = true;
      else if ( P1_l.Distance(P2_f) < prec )
        isConnected = true;
      else if ( P1_l.Distance(P2_l) < prec )
        isConnected = true;

      if ( !isConnected )
        return false;
    }
  }

  return true;
}
