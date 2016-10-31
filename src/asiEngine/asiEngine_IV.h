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
#include <asiData_IVNode.h>

//! API for imperative viewer (IV).
namespace engine_iv
{
  Handle(asiData_IVNode)
    Create_IV();

//---------------------------------------------------------------------------//

  Handle(asiData_IVPointSet2dNode)
    Create_PointSet2d(const Handle(TColStd_HArray1OfReal)& coords,
                      const TCollection_AsciiString&       name);

  void
    Clean_Points2d();

//---------------------------------------------------------------------------//

  Handle(asiData_IVPointSetNode)
    Create_PointSet(const Handle(asiAlgo_PointCloud)& points,
                    const TCollection_AsciiString&  name);

  void
    Clean_Points();

//---------------------------------------------------------------------------//

  Handle(asiData_IVCurveNode)
    Create_Curve(const Handle(Geom_Curve)&      curve,
                 const double                   uLimit,
                 const TCollection_AsciiString& name);

  void
    Clean_Curves();

//---------------------------------------------------------------------------//

  Handle(asiData_IVSurfaceNode)
    Create_Surface(const Handle(Geom_Surface)&    surface,
                   const double                   uLimit,
                   const double                   vLimit,
                   const TCollection_AsciiString& name);

  void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  Handle(asiData_IVTopoItemNode)
    Create_TopoItem(const TopoDS_Shape&            shape,
                    const TCollection_AsciiString& name);

  void
    Clean_Topo();

//---------------------------------------------------------------------------//

  Handle(asiData_IVTessItemNode)
    Create_TessItem(const Handle(Poly_Triangulation)& tess,
                    const TCollection_AsciiString&    name);

  void
    Clean_Tess();

//---------------------------------------------------------------------------//

  Handle(asiData_IVTextItemNode)
    Create_TextItem(const TCollection_AsciiString& text);

  void
    Clean_Text();

//---------------------------------------------------------------------------//

  void
    _cleanChildren(const Handle(ActAPI_INode)& parent);

};

#endif
