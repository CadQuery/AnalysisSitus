//-----------------------------------------------------------------------------
// Created on: 16 April 2016
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
