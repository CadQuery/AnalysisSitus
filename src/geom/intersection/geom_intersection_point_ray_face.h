//-----------------------------------------------------------------------------
// Created on: 19 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_intersection_point_ray_face_h
#define geom_intersection_point_ray_face_h

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <TopoDS_Face.hxx>

//! Point representing intersection between a ray and a face.
struct geom_intersection_point_ray_face
{
public:

  //! Default constructor.
  inline geom_intersection_point_ray_face() {}

  //! Complete constructor.
  //! \param _P [in] intersection point.
  //! \param _F [in] host face for the intersection point.
  inline geom_intersection_point_ray_face(const gp_Pnt&      _P,
                                          const TopoDS_Face& _F)
  : P(_P), F(_F)
  {}

public:

  //! Calculates surface normal at the intersection point using the host
  //! surface of the associated face.
  //! \return normal vector (null vector in case of failure).
  inline gp_Vec Norm() const
  {
    if ( F.IsNull() )
      return gp_Vec();

    Handle(Geom_Surface) S = BRep_Tool::Surface(F);
    ShapeAnalysis_Surface sas(S);
    gp_Pnt2d UV = sas.ValueOfUV( P, Precision::Confusion() );

    gp_Pnt S_P;
    gp_Vec S_Du, S_Dv;
    S->D1(UV.X(), UV.Y(), S_P, S_Du, S_Dv);
    return S_Du.Crossed(S_Dv);
  }

public:

  gp_Pnt      P; //!< Spatial intersection point.
  TopoDS_Face F; //!< Host face for the intersection point.

};

//! Collection of intersection points.
typedef NCollection_Sequence<geom_intersection_point_ray_face> geom_intersection_points_ray_face;

#endif
