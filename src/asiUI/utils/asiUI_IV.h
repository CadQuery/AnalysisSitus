//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiUI_IV_h
#define asiUI_IV_h

// asiUI includes
#include <asiUI_ObjectBrowser.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// asiEngine includes
#include <asiEngine_Model.h>

// OCCT includes
#include <Poly_Triangulation.hxx>

// Active Data includes
#include <ActAPI_INode.h>
#include <ActAPI_IPlotter.h>

// STD includes
#include <vector>

//! Interface for Imperative Viewer. A particular algorithm may benefit
//! from immediate plotting of its geometric variables in a unified way
//! thanks to this tool.
class asiUI_IV : public ActAPI_IPlotter
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_IV, ActAPI_IPlotter)

public:

  //! Constructor.
  asiUI_IV(const Handle(asiEngine_Model)&             model,
           const vtkSmartPointer<asiVisu_PrsManager>& prsMgr3d,
           const vtkSmartPointer<asiVisu_PrsManager>& prsMgr2d,
           asiUI_ObjectBrowser*                       pBrowser)
  //
  : ActAPI_IPlotter (),
    m_bBrowserOn    (true),
    m_bVisuOn       (true),
    m_model         (model),
    m_prsMgr3d      (prsMgr3d),
    m_prsMgr2d      (prsMgr2d),
    m_pBrowser      (pBrowser)
  {}

  //! Destructor.
  virtual ~asiUI_IV() {}

// COMMON:
public:

  asiUI_EXPORT virtual void
    ERASE_ALL();

  asiUI_EXPORT virtual void
    ERASE(const TCollection_AsciiString&);

// GEOMETRY:
public:

  asiUI_EXPORT virtual void
    DRAW_POINT(const gp_XY&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_POINT(const gp_Pnt2d&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_POINT(const gp_XYZ&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_POINT(const gp_Pnt&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_POINT(const TCollection_AsciiString&,
                 const gp_XY&,
                 const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_POINT(const TCollection_AsciiString&,
                 const gp_Pnt2d&,
                 const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_POINT(const TCollection_AsciiString&,
                 const gp_XYZ&,
                 const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_POINT(const TCollection_AsciiString&,
                 const gp_Pnt&,
                 const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_POINTS(const Handle(HRealArray)&,
                const Quantity_Color&,
                const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_POINTS(const TCollection_AsciiString&,
                  const Handle(HRealArray)&,
                  const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_VECTOR_AT(const gp_Pnt&,
                   const gp_Vec&,
                   const Quantity_Color&,
                   const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_VECTOR_AT(const TCollection_AsciiString&,
                     const gp_Pnt&,
                     const gp_Vec&,
                     const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_CURVE(const Handle(Geom_Curve)&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_CURVE(const TCollection_AsciiString&,
                 const Handle(Geom_Curve)&,
                 const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_CURVE2D(const Handle(Geom2d_Curve)&,
                 const Quantity_Color&,
                 const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_CURVE2D(const TCollection_AsciiString&,
                   const Handle(Geom2d_Curve)&,
                   const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const Quantity_Color&,
                 const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const Quantity_Color&,
                 const double, // opacity
                 const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const double, // U limit
                 const double, // V limit
                 const Quantity_Color&,
                 const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const double, // U limit
                 const double, // V limit
                 const Quantity_Color&,
                 const double, // opacity
                 const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_SURFACE(const TCollection_AsciiString&,
                   const Handle(Geom_Surface)&,
                   const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_SURFACE(const TCollection_AsciiString&,
                   const Handle(Geom_Surface)&,
                   const Quantity_Color&,
                   const double); // opacity

  asiUI_EXPORT virtual void
    REDRAW_SURFACE(const TCollection_AsciiString&,
                   const Handle(Geom_Surface)&,
                   const double, // U limit
                   const double, // V limit
                   const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_SURFACE(const TCollection_AsciiString&,
                   const Handle(Geom_Surface)&,
                   const double, // U limit
                   const double, // V limit
                   const Quantity_Color&,
                   const double); // opacity

// TOPOLOGY:
public:

  asiUI_EXPORT virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const double, // opacity
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const Quantity_Color&,
               const double, // opacity
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const Quantity_Color&,
               const double, // opacity
               const bool, // is wireframe
               const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_SHAPE(const TCollection_AsciiString&,
                 const TopoDS_Shape&);

  asiUI_EXPORT virtual void
    REDRAW_SHAPE(const TCollection_AsciiString&,
                 const TopoDS_Shape&,
                 const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_SHAPE(const TCollection_AsciiString&,
                 const TopoDS_Shape&,
                 const double); // opacity

  asiUI_EXPORT virtual void
    REDRAW_SHAPE(const TCollection_AsciiString&,
                 const TopoDS_Shape&,
                 const Quantity_Color&,
                 const double); // opacity

  asiUI_EXPORT virtual void
    REDRAW_SHAPE(const TCollection_AsciiString&,
                 const TopoDS_Shape&,
                 const Quantity_Color&,
                 const double, // opacity
                 const bool); // is wireframe

// TESSELLATION:
public:

  asiUI_EXPORT virtual void
    DRAW_LINK(const gp_Pnt&,
              const gp_Pnt&,
              const Quantity_Color&,
              const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_LINK(const gp_XYZ&,
              const gp_XYZ&,
              const Quantity_Color&,
              const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_LINK(const gp_Pnt2d&,
              const gp_Pnt2d&,
              const Quantity_Color&,
              const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_LINK(const gp_XY&,
              const gp_XY&,
              const Quantity_Color&,
              const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_LINK(const TCollection_AsciiString&,
                const gp_Pnt&,
                const gp_Pnt&,
                const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_LINK(const TCollection_AsciiString&,
                const gp_XYZ&,
                const gp_XYZ&,
                const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_LINK(const TCollection_AsciiString&,
                const gp_Pnt2d&,
                const gp_Pnt2d&,
                const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_LINK(const TCollection_AsciiString&,
                const gp_XY&,
                const gp_XY&,
                const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_POLYLINE(const std::vector<gp_XYZ>&,
                  const Quantity_Color&,
                  const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_POLYLINE(const std::vector<gp_XY>&,
                  const Quantity_Color&,
                  const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_POLYLINE(const TCollection_AsciiString&,
                    const std::vector<gp_XYZ>&,
                    const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_POLYLINE(const TCollection_AsciiString&,
                    const std::vector<gp_XY>&,
                    const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_TRIANGLE(const gp_Pnt&,
                  const gp_Pnt&,
                  const gp_Pnt&,
                  const Quantity_Color&,
                  const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_TRIANGLE(const gp_XYZ&,
                  const gp_XYZ&,
                  const gp_XYZ&,
                  const Quantity_Color&,
                  const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_TRIANGLE(const gp_Pnt2d&,
                  const gp_Pnt2d&,
                  const gp_Pnt2d&,
                  const Quantity_Color&,
                  const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_TRIANGLE(const gp_XY&,
                  const gp_XY&,
                  const gp_XY&,
                  const Quantity_Color&,
                  const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_TRIANGLE(const TCollection_AsciiString&,
                    const gp_Pnt&,
                    const gp_Pnt&,
                    const gp_Pnt&,
                    const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_TRIANGLE(const TCollection_AsciiString&,
                    const gp_XYZ&,
                    const gp_XYZ&,
                    const gp_XYZ&,
                    const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_TRIANGLE(const TCollection_AsciiString&,
                    const gp_Pnt2d&,
                    const gp_Pnt2d&,
                    const gp_Pnt2d&,
                    const Quantity_Color&);

  asiUI_EXPORT virtual void
    REDRAW_TRIANGLE(const TCollection_AsciiString&,
                    const gp_XY&,
                    const gp_XY&,
                    const gp_XY&,
                    const Quantity_Color&);

  asiUI_EXPORT virtual void
    DRAW_TRIANGULATION(const Handle(Poly_Triangulation)&,
                       const Quantity_Color&,
                       const double, // opacity
                       const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    REDRAW_TRIANGULATION(const TCollection_AsciiString&,
                         const Handle(Poly_Triangulation)&,
                         const Quantity_Color&,
                         const double); // opacity

// TEXT
public:

  asiUI_EXPORT virtual void
    DRAW_TEXT(const TCollection_AsciiString& text,
              const TCollection_AsciiString& name);

  asiUI_EXPORT virtual void
    REDRAW_TEXT(const TCollection_AsciiString& name,
                const TCollection_AsciiString& text);

// A-SITUS SPECIFIC:
public:

  asiUI_EXPORT void
    DRAW_ASI_POINTSF(const Handle(asiAlgo_BaseCloud<float>)&,
                     const Quantity_Color&,
                     const TCollection_AsciiString&);

  asiUI_EXPORT void
    DRAW_ASI_POINTS(const std::vector<gp_Pnt2d>&,
                    const Quantity_Color&,
                    const TCollection_AsciiString&);

  asiUI_EXPORT void
    BROWSER_OFF();

  asiUI_EXPORT void
    BROWSER_ON();

  asiUI_EXPORT void
    VISUALIZATION_OFF();

  asiUI_EXPORT void
    VISUALIZATION_ON();

public:

  //! Initializes imperative plotter.
  //! \param model    [in] Data Model instance.
  //! \param prsMgr3d [in] 3D presentation manager.
  //! \param prsMgr2d [in] 2D presentation manager.
  //! \param pBrowser [in] Object browser instance.
  void Init(const Handle(asiEngine_Model)&             model,
            const vtkSmartPointer<asiVisu_PrsManager>& prsMgr3d,
            const vtkSmartPointer<asiVisu_PrsManager>& prsMgr2d,
            asiUI_ObjectBrowser*                       pBrowser)
  {
    m_model    = model;
    m_prsMgr3d = prsMgr3d;
    m_prsMgr2d = prsMgr2d;
    m_pBrowser = pBrowser;
  }

  //! Sets presentation manager for 3D visualization.
  //! \param prsMgr [in] presentation manager to set.
  void SetPrsMgr3d(const vtkSmartPointer<asiVisu_PrsManager>& prsMgr)
  {
    m_prsMgr3d = prsMgr;
  }

  //! Sets presentation manager for 3D visualization.
  //! \param prsMgr [in] presentation manager to set.
  void SetPrsMgr2d(const vtkSmartPointer<asiVisu_PrsManager>& prsMgr)
  {
    m_prsMgr2d = prsMgr;
  }

  //! \return presentation manager for 3D visualization.
  const vtkSmartPointer<asiVisu_PrsManager>& GetPrsMgr3d() const
  {
    return m_prsMgr3d;
  }

  //! \return presentation manager for 2D visualization.
  const vtkSmartPointer<asiVisu_PrsManager>& GetPrsMgr2d() const
  {
    return m_prsMgr2d;
  }

protected:

  asiUI_EXPORT void
    visualize(const bool                  is2d,
              const Handle(ActAPI_INode)& node,
              const bool                  hasColor,
              const Quantity_Color&       color,
              const double                opacity,
              const bool                  isWireframe) const;

  asiUI_EXPORT const vtkSmartPointer<asiVisu_PrsManager>&
    prsManager(const bool is2d) const;

protected:

  asiUI_EXPORT void
    draw_point(const gp_XY&                   coord,
               const Quantity_Color&          color,
               const TCollection_AsciiString& name,
               const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_points(const Handle(HRealArray)&      coords,
                const Quantity_Color&          color,
                const TCollection_AsciiString& name,
                const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_curve(const Handle(Geom_Curve)&      curve,
               const Quantity_Color&          color,
               const TCollection_AsciiString& name,
               const bool                     is2dViewer,
               const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_curve2d(const Handle(Geom2d_Curve)&    curve,
                 const Quantity_Color&          color,
                 const TCollection_AsciiString& name,
                 const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_surface(const Handle(Geom_Surface)&    surface,
                 const double                   uLimit,
                 const double                   vLimit,
                 const Quantity_Color&          color,
                 const double                   opacity,
                 const TCollection_AsciiString& name,
                 const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_shape(const TopoDS_Shape&            shape,
               const bool                     hasColor,
               const Quantity_Color&          color,
               const double                   opacity,
               const bool                     isWireframe,
               const TCollection_AsciiString& name,
               const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_link(const gp_XYZ&                  p1,
              const gp_XYZ&                  p2,
              const Quantity_Color&          color,
              const TCollection_AsciiString& name,
              const bool                     is2dViewer,
              const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_link(const gp_XYZ&                  p1,
              const gp_XYZ&                  p2,
              const Quantity_Color&          color,
              const TCollection_AsciiString& name,
              const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_link(const gp_XY&                   p1,
              const gp_XY&                   p2,
              const Quantity_Color&          color,
              const TCollection_AsciiString& name,
              const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_polyline(const std::vector<gp_XYZ>&     poles,
                  const Quantity_Color&          color,
                  const TCollection_AsciiString& name,
                  const bool                     is2dViewer,
                  const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_polyline(const std::vector<gp_XYZ>&     poles,
                  const Quantity_Color&          color,
                  const TCollection_AsciiString& name,
                  const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_polyline(const std::vector<gp_XY>&      poles,
                  const Quantity_Color&          color,
                  const TCollection_AsciiString& name,
                  const bool                     newPrimitive);

  asiUI_EXPORT void
    draw_triangle(const gp_XYZ&                  p1,
                  const gp_XYZ&                  p2,
                  const gp_XYZ&                  p3,
                  const Quantity_Color&          color,
                  const TCollection_AsciiString& name,
                  const bool                     newPrimitive,
                  const bool                     isWireframe);

  asiUI_EXPORT void
    draw_triangle(const gp_XY&                   p1,
                  const gp_XY&                   p2,
                  const gp_XY&                   p3,
                  const Quantity_Color&          color,
                  const TCollection_AsciiString& name,
                  const bool                     newPrimitive,
                  const bool                     isWireframe);

  asiUI_EXPORT void
    draw_triangulation(const Handle(Poly_Triangulation)& tris,
                       const Quantity_Color&             color,
                       const double                      opacity,
                       const TCollection_AsciiString&    name,
                       const bool                        newPrimitive);

  asiUI_EXPORT void
    draw_text(const TCollection_AsciiString& text,
              const TCollection_AsciiString& name,
              const bool                     newPrimitive);

protected:

  bool                                m_bBrowserOn; //!< Flag to enable/disable plotting.
  bool                                m_bVisuOn;    //!< Flag to enable/disable plotting.
  Handle(asiEngine_Model)             m_model;      //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr3d;   //!< Presentation manager 3D.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr2d;   //!< Presentation manager 2D.
  asiUI_ObjectBrowser*                m_pBrowser;   //!< Object browser.

};

#endif
