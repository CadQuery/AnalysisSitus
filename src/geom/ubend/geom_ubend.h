//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// OCCT includes
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

#define UBEND_NUM_SECTIONS 25

//-----------------------------------------------------------------------------

//! U-bend modeling API.
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

public:

  static Handle(Geom_BezierCurve)
    CreatePath();

  static TopoDS_Edge
    CreatePathTopo();

  static void
    CreateSections(const Handle(Geom_Curve)&                        path,
                   const int                                        nSections,
                   const SideEnum                                   side,
                   NCollection_Sequence<Handle(Geom_BSplineCurve)>& crossSections);

  static void
    CreateSectionsTopo(const Handle(Geom_Curve)& path,
                       const int                 nSections,
                       const SideEnum            side,
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

private:

  static Handle(Geom2d_BSplineCurve)
    sideCurve2d(const SideEnum side);

};
