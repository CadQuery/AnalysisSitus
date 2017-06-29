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

#ifndef asiUI_JournalIV_h
#define asiUI_JournalIV_h

// A-Situs includes
#include <asiUI.h>

// UI includes
#include <asiUI_ObjectBrowser.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

// Engine includes
#include <asiEngine_Model.h>

// OCCT includes
#include <Poly_Triangulation.hxx>

// Active Data includes
#include <ActAPI_INode.h>
#include <ActAPI_IPlotter.h>

// STD includes
#include <vector>

DEFINE_STANDARD_HANDLE(asiUI_JournalIV, ActAPI_IPlotter)

//! Interface for Imperative Viewer. A particular algorithm may benefit
//! from immediate plotting of its geometric variables in a unified way
//! thanks to this abstract class.
class asiUI_JournalIV : public ActAPI_IPlotter
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_JournalIV, ActAPI_IPlotter)

public:

  //! Constructor.
  asiUI_JournalIV(const Handle(asiEngine_Model)&             model,
                  const vtkSmartPointer<asiVisu_PrsManager>& prsMgr3d,
                  const vtkSmartPointer<asiVisu_PrsManager>& prsMgr2d,
                  asiUI_ObjectBrowser*                       pBrowser)
  //
  : ActAPI_IPlotter (),
    m_model         (model),
    m_prsMgr3d      (prsMgr3d),
    m_prsMgr2d      (prsMgr2d),
    m_pBrowser      (pBrowser)
  {}

  //! Destructor.
  virtual ~asiUI_JournalIV() {}

// COMMON:
public:

  asiUI_EXPORT virtual void
    CLEAN();

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
    DRAW_POINTS(const Handle(HRealArray)&,
                const Quantity_Color&,
                const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_LINK(const gp_Pnt&,
              const gp_Pnt&,
              const Quantity_Color&,
              const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_VECTOR_AT(const gp_Pnt&,
                   const gp_Vec&,
                   const Quantity_Color&,
                   const TCollection_AsciiString&);

  asiUI_EXPORT virtual void
    DRAW_CURVE(const Handle(Geom_Curve)&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

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

// TESSELLATION:
public:

  asiUI_EXPORT virtual void
    DRAW_TRIANGULATION(const Handle(Poly_Triangulation)&,
                       const Quantity_Color&,
                       const double, // opacity
                       const TCollection_AsciiString&);

// TEXT
public:

  asiUI_EXPORT virtual void
    DRAW_TEXT(const TCollection_AsciiString& text);

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

  asiUI_EXPORT const vtkSmartPointer<asiVisu_PrsManager>&
    prsManager3d() const;

  asiUI_EXPORT const vtkSmartPointer<asiVisu_PrsManager>&
    prsManager2d() const;

protected:

  Handle(asiEngine_Model)             m_model;    //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr3d; //!< Presentation manager 3D.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr2d; //!< Presentation manager 2D.
  asiUI_ObjectBrowser*                m_pBrowser; //!< Object browser.

};

#endif
