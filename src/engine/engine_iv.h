//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_iv_h
#define engine_iv_h

// A-Situs includes
#include <analysis_situs.h>

// Visualization includes
#include <visu_iv_node.h>

//! API for imperative viewer (IV).
namespace engine_iv
{
  Handle(visu_iv_node)
    Create_IV();

//---------------------------------------------------------------------------//

  Handle(visu_iv_point_set_2d_node)
    Create_PointSet2d(const Handle(TColStd_HArray1OfReal)& coords,
                      const TCollection_AsciiString&       name);

  void
    Clean_Points2d();

//---------------------------------------------------------------------------//

  Handle(visu_iv_point_set_node)
    Create_PointSet(const Handle(geom_point_cloud)& points,
                    const TCollection_AsciiString&  name);

  void
    Clean_Points();

//---------------------------------------------------------------------------//

  Handle(visu_iv_curve_node)
    Create_Curve(const Handle(Geom_Curve)&      curve,
                 const double                   uLimit,
                 const TCollection_AsciiString& name);

  void
    Clean_Curves();

//---------------------------------------------------------------------------//

  Handle(visu_iv_surface_node)
    Create_Surface(const Handle(Geom_Surface)&    surface,
                   const double                   uLimit,
                   const double                   vLimit,
                   const TCollection_AsciiString& name);

  void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  Handle(visu_iv_topo_item_node)
    Create_TopoItem(const TopoDS_Shape&            shape,
                    const TCollection_AsciiString& name);

  void
    Clean_Topo();

//---------------------------------------------------------------------------//

  Handle(visu_iv_tess_item_node)
    Create_TessItem(const Handle(Poly_Triangulation)& tess,
                    const TCollection_AsciiString&    name);

  void
    Clean_Tess();

//---------------------------------------------------------------------------//

  void
    _cleanChildren(const Handle(ActAPI_INode)& parent);

};

#endif
