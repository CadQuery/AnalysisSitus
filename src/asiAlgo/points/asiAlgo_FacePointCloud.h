//-----------------------------------------------------------------------------
// Created on: 16 April 2016
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

#ifndef asiAlgo_FacePointCloud_h
#define asiAlgo_FacePointCloud_h

// Geometry includes
#include <asiAlgo_CommonCloud.h>

// OCCT includes
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

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
