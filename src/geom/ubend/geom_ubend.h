//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Calculus includes
#include <calculus_design_law.h>

// OCCT includes
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>

//-----------------------------------------------------------------------------

#define UBEND_NUM_SECTIONS 50

//-----------------------------------------------------------------------------

//! U-bend modeling API. This tool employs the so called multipoint
//! approximation provided within AppDef package of OpenCascade. A good
//! reference to this functionality can be found in the user's guide. It is
//! also fruitful to have a look on "bsmooth" Draw command which is an
//! interactive approximation utility.
class geom_ubend
{
public:

  //! There are 4 surfaces in the u-bend.
  enum SideEnum
  {
    Side_Top = 0,
    Side_Right,
    Side_Bottom,
    Side_Left
  };

  //! There are 12 control points in each section.
  enum PoleEnum
  {
    Pole_P1 = 1,
    Pole_P2,
    Pole_P3,
    Pole_P4,
    Pole_P5,
    Pole_P6,
    Pole_P7,
    Pole_P8,
    Pole_P9,
    Pole_P10,
    Pole_P11,
    Pole_P12,
    //
    Pole_Last //!< Just for convenience in loops.
  };

  //! There are two coordinates in each pole evolving independently.
  enum CoordEnum
  {
    Coord_X = 1,
    Coord_Y
  };

  //! Collection of laws.
  typedef NCollection_Sequence<Handle(calculus_design_law)> t_coord_laws;

public:

  static gp_Pnt2d
    GetSectionPole(const PoleEnum                     pole,
                   const Handle(calculus_design_law)& xLaw,
                   const Handle(calculus_design_law)& yLaw,
                   const double                       u);

  static Handle(calculus_design_law)
    CreateExplicitLaw(const PoleEnum  pole,
                      const CoordEnum coord,
                      const double    last);

  static Handle(calculus_design_law)
    NullExplicitLaw(const double last);

  static Handle(Geom2d_BSplineCurve)
    CreateParametricLaw(const Handle(calculus_design_law)& xLaw,
                        const Handle(calculus_design_law)& yLaw);

  static Handle(Geom_BezierCurve)
    CreatePath();

  static TopoDS_Edge
    CreatePathTopo();

  static void
    CreateSections(const Handle(Geom_Curve)&                        path,
                   const int                                        nSections,
                   const SideEnum                                   side,
                   const t_coord_laws&                              xLaws,
                   const t_coord_laws&                              yLaws,
                   NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections);

  static void
    CreateSectionsTopo(const Handle(Geom_Curve)& path,
                       const int                 nSections,
                       const SideEnum            side,
                       const t_coord_laws&       xLaws,
                       const t_coord_laws&       yLaws,
                       TopoDS_Compound&          crossSections);

  static void
    CreateSkins(const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_top,
                const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_right,
                const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_bottom,
                const NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections_left,
                NCollection_Sequence<Handle(Geom_BSplineSurface)>&     skins);

  static void
    CreateSkinsTopo(const TopoDS_Compound& crossSections_top,
                    const TopoDS_Compound& crossSections_right,
                    const TopoDS_Compound& crossSections_bottom,
                    const TopoDS_Compound& crossSections_left,
                    TopoDS_Compound&       skins);

  static TopoDS_Shape
    AsShell(const TopoDS_Compound& skins);

private:

  static Handle(Geom2d_BSplineCurve)
    sideCurve2d(const SideEnum      side,
                const t_coord_laws& xLaw,
                const t_coord_laws& yLaw,
                const double        u);

};
