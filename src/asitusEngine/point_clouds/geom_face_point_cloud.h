//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_face_point_cloud_h
#define geom_face_point_cloud_h

// Geometry includes
#include <geom_common_point_cloud_2d.h>

// OCCT includes
#include <TopoDS_Face.hxx>

DEFINE_STANDARD_HANDLE(geom_face_point_cloud, geom_common_point_cloud_2d)

//! Data adapter for two-dimensional point cloud of face vertices.
class geom_face_point_cloud : public geom_common_point_cloud_2d
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_face_point_cloud, geom_common_point_cloud_2d)

public:

  geom_face_point_cloud();

public:

  void
    InitWithVertices(const TopoDS_Face& face);

  const gp_XYZ&
    Value3d(const int oneBasedIndex) const;

  const TopoDS_Face&
    Face() const;

  void
    SetFace(const TopoDS_Face& face);

public:

  virtual Handle(geom_common_point_cloud_2d)
    EmptyCopy() const;

  virtual const gp_XY&
    Value(const int oneBasedIndex) const;

  virtual const int
    Length() const;

  virtual Handle(TColStd_HArray1OfReal)
    Coords2d() const;

  virtual void
    SetCoords2d(const Handle(TColStd_HArray1OfReal)& coords);

  virtual void
    AddCoord(const gp_XY& coord);

protected:

  TopoDS_Face         m_face;     //!< Base face.
  std::vector<gp_XY>  m_points2d; //!< Points 2D.
  std::vector<gp_XYZ> m_points3d; //!< Points 3D.

};

#endif
