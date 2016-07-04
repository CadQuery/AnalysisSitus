//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_split_shape_h
#define geom_split_shape_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_edge_edge_dist.h>
#include <geom_utils.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>

//! Base class for CC splitter tools.
class geom_split_shape
{
public:

  ASitus_EXPORT
    geom_split_shape(const double inaccuracy);

protected:

  struct t_interactions
  {
    NCollection_Sequence< NCollection_Sequence<TopoDS_Vertex> > List;

    void __add(const TopoDS_Vertex& V, const double prec)
    {
      bool added = false;
      for ( int i = 1; i <= this->List.Length(); ++i )
      {
        NCollection_Sequence<TopoDS_Vertex>& tuple = this->List(i);
        for ( int j = 1; j <= tuple.Length(); ++j )
        {
          const TopoDS_Vertex& V_probe = tuple(j);
          if ( __areCoincident(V, V_probe, prec) )
          {
            tuple.Append(V);
            added = true;
            break;
          }
        }

        if ( added )
          break;
      }

      // Not added to any existing tuple, so add a new tuple
      if ( !added )
      {
        NCollection_Sequence<TopoDS_Vertex> new_tuple;
        new_tuple.Append(V);
        this->List.Append(new_tuple);
      }
    }

    static bool __areCoincident(const TopoDS_Vertex& V1,
                                const TopoDS_Vertex& V2,
                                const double         prec)
    {
      if ( V1.IsPartner(V2) )
        return true;

      const gp_Pnt P1 = BRep_Tool::Pnt(V1);
      const gp_Pnt P2 = BRep_Tool::Pnt(V2);

      if ( P1.Distance(P2) <= prec )
        return true;

      return false;
    }

    static bool __areCoincident(const TopoDS_Edge& E1,
                                const TopoDS_Edge& E2,
                                const double       prec)
    {
      if ( E1.IsPartner(E2) )
        return true;

      // Geometric check is done not by vertices, but by extremities of a
      // curve. That is because a vertex is a tolerant entity, so it can
      // be significantly shifted with respect to a host curve. Since we
      // are interested in equality of elongated geometries, rather than
      // equality of points, the check by extremities is more robust

      double f1, l1, f2, l2;
      BRep_Tool::Range(E1, f1, l1);
      BRep_Tool::Range(E2, f2, l2);

      gp_Pnt P1f = BRepAdaptor_Curve(E1).Value(f1);
      gp_Pnt P1l = BRepAdaptor_Curve(E1).Value(l1);
      gp_Pnt P2f = BRepAdaptor_Curve(E2).Value(f2);
      gp_Pnt P2l = BRepAdaptor_Curve(E2).Value(l2);

      if ( P1f.Distance(P2f) < prec && P1l.Distance(P2l) < prec ||
           P1f.Distance(P2l) < prec && P1l.Distance(P2f) < prec )
        return true;

      return false;
    }
  };

  //! Structure representing interaction parameters.
  struct TInterPt
  {
    TInterPt() : W(0) {}
    TInterPt(const gp_Pnt& _P, const double _W, const TopoDS_Face& _F) : P(_P), W(_W), F(_F) {}

    gp_Pnt      P; //!< Spatial point.
    double      W; //!< Interaction parameter.
    TopoDS_Face F; //!< Host face.

    //! Comparator.
    //! \param pt [in] point to compare with.
    //! \return true/false.
    inline bool operator>(const TInterPt& pt) const
    {
      return this->W > pt.W;
    }

    //! Equality comparator.
    //! \param pt [in] point to compare with.
    //! \return true/false.
    inline bool operator==(const TInterPt& pt) const
    {
      return this->W == pt.W;
    }
  };

  //! Convenience alias for a collection of intersection points.
  typedef NCollection_Sequence<TInterPt> TInterPts;

  //! Convenience alias for map of splitting points associated with edges.
  typedef NCollection_IndexedDataMap<TopoDS_Edge, TInterPts, TopTools_ShapeMapHasher> TEdgeInteractions;

protected:

  ASitus_EXPORT TopoDS_Wire
    split(const TopoDS_Edge&                E,
          const TInterPts&                  pts,
          const Handle(ShapeBuild_ReShape)& ctx) const;

  ASitus_EXPORT void
    mergeCoincidentVertices(TopoDS_Shape&                     mosaic,
                            TopoDS_Face&                      face,
                            const double                      inaccuracy,
                            const Handle(ShapeBuild_ReShape)& ctx) const;

protected:

  double m_fInaccuracy; //!< Tolerance of the input geometry.

};

#endif
