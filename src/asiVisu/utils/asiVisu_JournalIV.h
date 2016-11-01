//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_JournalIV_h
#define asiVisu_JournalIV_h

// A-Situs includes
#include <asiAlgo.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

// OCCT includes
#include <Poly_Triangulation.hxx>

// Active Data includes
#include <ActAPI_INode.h>
#include <ActAPI_IPlotter.h>

DEFINE_STANDARD_HANDLE(asiVisu_JournalIV, ActAPI_IPlotter)

//! Interface for Imperative Viewer. A particular algorithm may benefit
//! from immediate plotting of its geometric variables in a unified way
//! thanks to this abstract class.
class asiVisu_JournalIV : public ActAPI_IPlotter
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_JournalIV, ActAPI_IPlotter)

public:

  //! \return IV instance.
  static Handle(asiVisu_JournalIV) Instance()
  {
    return new asiVisu_JournalIV();
  }

  //! Constructor.
  asiVisu_JournalIV() : ActAPI_IPlotter() {}

  //! Destructor.
  virtual ~asiVisu_JournalIV() {}

// COMMON:
public:

  virtual void
    CLEAN();

// GEOMETRY:
public:

  virtual void
    DRAW_POINT(const gp_XY&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_POINT(const gp_Pnt2d&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_POINT(const gp_XYZ&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_POINT(const gp_Pnt&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_POINTS(const Handle(HRealArray)&,
                const Quantity_Color&,
                const TCollection_AsciiString&);

  virtual void
    DRAW_LINK(const gp_Pnt&,
              const gp_Pnt&,
              const Quantity_Color&,
              const TCollection_AsciiString&);

  virtual void
    DRAW_VECTOR_AT(const gp_Pnt&,
                   const gp_Vec&,
                   const Quantity_Color&,
                   const TCollection_AsciiString&);

  virtual void
    DRAW_CURVE(const Handle(Geom_Curve)&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const Quantity_Color&,
                 const TCollection_AsciiString&);

  virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const Quantity_Color&,
                 const double, // opacity
                 const TCollection_AsciiString&);

  virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const double, // U limit
                 const double, // V limit
                 const Quantity_Color&,
                 const TCollection_AsciiString&);

  virtual void
    DRAW_SURFACE(const Handle(Geom_Surface)&,
                 const double, // U limit
                 const double, // V limit
                 const Quantity_Color&,
                 const double, // opacity
                 const TCollection_AsciiString&);

// TOPOLOGY:
public:

  virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const Quantity_Color&,
               const TCollection_AsciiString&);

  virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const double, // opacity
               const TCollection_AsciiString&);

  virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const Quantity_Color&,
               const double, // opacity
               const TCollection_AsciiString&);

  virtual void
    DRAW_SHAPE(const TopoDS_Shape&,
               const Quantity_Color&,
               const double, // opacity
               const bool, // is wireframe
               const TCollection_AsciiString&);

// TESSELLATION:
public:

  virtual void
    DRAW_TRIANGULATION(const Handle(Poly_Triangulation)&,
                       const Quantity_Color&,
                       const double, // opacity
                       const TCollection_AsciiString&);

// TEXT
public:

  virtual void
    DRAW_TEXT(const TCollection_AsciiString& text);

protected:

  void
    visualize(const bool                  is2d,
              const Handle(ActAPI_INode)& node,
              const bool                  hasColor,
              const Quantity_Color&       color,
              const double                opacity,
              const bool                  isWireframe) const;

  const vtkSmartPointer<asiVisu_PrsManager>&
    prsManager(const bool is2d) const;

  const vtkSmartPointer<asiVisu_PrsManager>&
    prsManager3d() const;

  const vtkSmartPointer<asiVisu_PrsManager>&
    prsManager2d() const;

};

#endif
