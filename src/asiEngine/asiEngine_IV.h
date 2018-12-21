//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiEngine_IV_h
#define asiEngine_IV_h

// asiEngine includes
#include <asiEngine_Base.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

//! API for imperative viewer (IV).
class asiEngine_IV : public asiEngine_Base
{
public:

  //! Ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_IV(const Handle(asiEngine_Model)& model,
               ActAPI_ProgressEntry           progress = NULL,
               ActAPI_PlotterEntry            plotter  = NULL)
  : asiEngine_Base(model, progress, plotter) {}

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

  asiEngine_EXPORT void
    ActivateCurvesHandles(const bool          on,
                          asiVisu_PrsManager* pPrsMgr);

//---------------------------------------------------------------------------//

  asiEngine_EXPORT Handle(asiData_IVCurve2dNode)
    Find_Curve2d(const TCollection_AsciiString& name);

  asiEngine_EXPORT Handle(asiData_IVCurve2dNode)
    Create_Curve2d(const Handle(Geom2d_Curve)&    curve,
                   const Handle(Geom_Surface)&    surface,
                   const double                   uLimit,
                   const TCollection_AsciiString& name,
                   const bool                     useAutoNaming);

  asiEngine_EXPORT void
    Update_Curve2d(const Handle(asiData_IVCurve2dNode)& node,
                   const Handle(Geom2d_Curve)&          curve,
                   const Handle(Geom_Surface)&          surface,
                   const double                         uLimit);

  asiEngine_EXPORT void
    Clean_Curves2d();

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

};

#endif
