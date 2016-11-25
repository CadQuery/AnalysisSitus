//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiEngine_IV_h
#define asiEngine_IV_h

// A-Situs includes
#include <asiEngine_Model.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>

//! API for imperative viewer (IV).
class asiEngine_IV
{
public:

  //! ctor.
  //! \param model [in] Data Model instance.
  asiEngine_IV(const Handle(asiEngine_Model)& model)
  //
  : m_model(model) {}

public:

  asiEngine_EXPORT Handle(asiData_IVNode)
    Create_IV();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVPointSet2dNode)
    Create_PointSet2d(const Handle(TColStd_HArray1OfReal)& coords,
                      const TCollection_AsciiString&       name);

  asiEngine_EXPORT void
    Clean_Points2d();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVPointSetNode)
    Create_PointSet(const Handle(asiAlgo_PointCloud<double>)& points,
                    const TCollection_AsciiString&            name);

  asiEngine_EXPORT void
    Clean_Points();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVCurveNode)
    Create_Curve(const Handle(Geom_Curve)&      curve,
                 const double                   uLimit,
                 const TCollection_AsciiString& name);

  asiEngine_EXPORT void
    Clean_Curves();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVSurfaceNode)
    Create_Surface(const Handle(Geom_Surface)&    surface,
                   const double                   uLimit,
                   const double                   vLimit,
                   const TCollection_AsciiString& name);

  asiEngine_EXPORT void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVTopoItemNode)
    Create_TopoItem(const TopoDS_Shape&            shape,
                    const TCollection_AsciiString& name);

  asiEngine_EXPORT void
    Clean_Topo();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVTessItemNode)
    Create_TessItem(const Handle(Poly_Triangulation)& tess,
                    const TCollection_AsciiString&    name);

  asiEngine_EXPORT void
    Clean_Tess();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVTextItemNode)
    Create_TextItem(const TCollection_AsciiString& text);

  asiEngine_EXPORT void
    Clean_Text();

protected:

  asiEngine_EXPORT void
    _cleanChildren(const Handle(ActAPI_INode)& parent);

protected:

  Handle(asiEngine_Model) m_model;  //!< Data Model instance.

};

#endif
