//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_intersect_AABBs.h>

// OCCT includes
#include <BRepBndLib.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param tolerance [in] tolerant thickness which is especially useful for
//!                       resolving intersections in case of nearly tangent
//!                       boundary boxes.
geom_intersect_AABBs::geom_intersect_AABBs(const double tolerance)
: m_fToler(tolerance)
{}

//! Checks AABBs intersection.
//! \param S1 [in] first shape to check.
//! \param S2 [in] second shape to check.
//! \return true if the bounding boxes intersect, false -- otherwise.
bool
  geom_intersect_AABBs::operator()(const TopoDS_Shape& S1,
                                   const TopoDS_Shape& S2)
{
  Bnd_Box B1, B2;
  BRepBndLib::Add(S1, B1);
  BRepBndLib::Add(S2, B2);

  B1.Enlarge(m_fToler);
  B2.Enlarge(m_fToler);

  return !B1.IsOut(B2);
}

//! Checks AABBs intersection.
//! \param S1 [in]     first shape to check.
//! \param S2 [in]     second shape to check.
//! \param B1 [in/out] bounding box of the first shape.
//! \param B2 [in/out] bounding box of the second shape.
//! \return true if the bounding boxes intersect, false -- otherwise.
bool
  geom_intersect_AABBs::operator()(const TopoDS_Shape& S1,
                                   const TopoDS_Shape& S2,
                                   Bnd_Box&            B1,
                                   Bnd_Box&            B2)
{
  if ( B1.IsVoid() )
  {
    BRepBndLib::Add(S1, B1);
    B1.Enlarge(m_fToler);
  }

  if ( B2.IsVoid() )
  {
    BRepBndLib::Add(S2, B2);
    B2.Enlarge(m_fToler);
  }

  return !B1.IsOut(B2);
}
