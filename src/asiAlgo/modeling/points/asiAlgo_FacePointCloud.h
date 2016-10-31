//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FacePointCloud_h
#define asiAlgo_FacePointCloud_h

// Geometry includes
#include <asiAlgo_CommonCloud.h>

// OCCT includes
#include <TopoDS_Face.hxx>

DEFINE_STANDARD_HANDLE(asiAlgo_FacePointCloud, asiAlgo_CommonCloud2d)

//! Data adapter for two-dimensional point cloud of face vertices.
class asiAlgo_FacePointCloud : public asiAlgo_CommonCloud2d
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FacePointCloud, asiAlgo_CommonCloud2d)

public:

  asiAlgo_EXPORT
    asiAlgo_FacePointCloud();

public:

  asiAlgo_EXPORT void
    InitWithVertices(const TopoDS_Face& face);

  asiAlgo_EXPORT const gp_XYZ&
    Value3d(const int oneBasedIndex) const;

  asiAlgo_EXPORT const TopoDS_Face&
    Face() const;

  asiAlgo_EXPORT void
    SetFace(const TopoDS_Face& face);

public:

  asiAlgo_EXPORT virtual Handle(asiAlgo_CommonCloud2d)
    EmptyCopy() const;

  asiAlgo_EXPORT virtual const gp_XY&
    Value(const int oneBasedIndex) const;

  asiAlgo_EXPORT virtual const int
    Length() const;

  asiAlgo_EXPORT virtual Handle(TColStd_HArray1OfReal)
    Coords2d() const;

  asiAlgo_EXPORT virtual void
    SetCoords2d(const Handle(TColStd_HArray1OfReal)& coords);

  asiAlgo_EXPORT virtual void
    AddCoord(const gp_XY& coord);

protected:

  TopoDS_Face         m_face;     //!< Base face.
  std::vector<gp_XY>  m_points2d; //!< Points 2D.
  std::vector<gp_XYZ> m_points3d; //!< Points 3D.

};

#endif
