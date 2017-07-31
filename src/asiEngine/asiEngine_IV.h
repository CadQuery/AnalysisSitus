//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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

  asiEngine_EXPORT void
    Clean_All();

  asiEngine_EXPORT void
    Clean(const TCollection_AsciiString& name);

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVPointSet2dNode)
    Find_PointSet2d(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVPointSet2dNode)
    Create_PointSet2d(const Handle(TColStd_HArray1OfReal)& coords,
                      const TCollection_AsciiString&       name,
                      const bool                           useAutoNaming);

  asiEngine_EXPORT Handle(asiData_IVPointSet2dNode)
    Create_PointSet2d(const std::vector<gp_Pnt2d>&   points,
                      const TCollection_AsciiString& name,
                      const bool                     useAutoNaming);

  asiEngine_EXPORT void
    Clean_Points2d();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVPointSetNode)
    Find_PointSet(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVPointSetNode)
    Create_PointSet(const Handle(asiAlgo_BaseCloud<double>)& points,
                    const TCollection_AsciiString&           name,
                    const bool                               useAutoNaming);

  asiEngine_EXPORT void
    Clean_Points();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVCurveNode)
    Find_Curve(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVCurveNode)
    Create_Curve(const Handle(Geom_Curve)&      curve,
                 const double                   uLimit,
                 const TCollection_AsciiString& name,
                 const bool                     useAutoNaming);

  asiEngine_EXPORT void
    Update_Curve(const Handle(asiData_IVCurveNode)& node,
                 const Handle(Geom_Curve)&          curve,
                 const double                       uLimit);

  asiEngine_EXPORT void
    Clean_Curves();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVSurfaceNode)
    Find_Surface(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVSurfaceNode)
    Create_Surface(const Handle(Geom_Surface)&    surface,
                   const double                   uLimit,
                   const double                   vLimit,
                   const TCollection_AsciiString& name,
                   const bool                     useAutoNaming);

  asiEngine_EXPORT void
    Update_Surface(const Handle(asiData_IVSurfaceNode)& node,
                   const Handle(Geom_Surface)&          surface,
                   const double                         uLimit,
                   const double                         vLimit);

  asiEngine_EXPORT void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVTopoItemNode)
    Find_TopoItem(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVTopoItemNode)
    Create_TopoItem(const TopoDS_Shape&            shape,
                    const TCollection_AsciiString& name,
                    const bool                     useAutoNaming);

  asiEngine_EXPORT void
    Update_TopoItem(const Handle(asiData_IVTopoItemNode)& node,
                    const TopoDS_Shape&                   shape);

  asiEngine_EXPORT void
    Clean_Topo();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVTessItemNode)
    Find_TessItem(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVTessItemNode)
    Create_TessItem(const Handle(Poly_Triangulation)& tess,
                    const TCollection_AsciiString&    name,
                    const bool                        useAutoNaming);

  asiEngine_EXPORT void
    Update_TessItem(const Handle(asiData_IVTessItemNode)& node,
                    const Handle(Poly_Triangulation)&     tess);

  asiEngine_EXPORT void
    Clean_Tess();

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVTextItemNode)
    Find_TextItem(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVTextItemNode)
    Create_TextItem(const TCollection_AsciiString& text,
                    const TCollection_AsciiString& name,
                    const bool                     useAutoNaming);

  asiEngine_EXPORT void
    Update_TextItem(const Handle(asiData_IVTextItemNode)& node,
                    const TCollection_AsciiString&        text);

  asiEngine_EXPORT void
    Clean_Text();

protected:

  asiEngine_EXPORT void
    _cleanChildren(const Handle(ActAPI_INode)& parent);

protected:

  Handle(asiEngine_Model) m_model;  //!< Data Model instance.

};

#endif
