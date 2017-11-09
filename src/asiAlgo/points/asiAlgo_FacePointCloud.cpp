//-----------------------------------------------------------------------------
// Created on: 16 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_FacePointCloud.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//! Default constructor.
asiAlgo_FacePointCloud::asiAlgo_FacePointCloud()
{}

//! Initializes point cloud with vertices from the given face.
//! \param face [in] base face.
void asiAlgo_FacePointCloud::InitWithVertices(const TopoDS_Face& face)
{
  m_face = face;

  // Populate point cloud
  for ( TopExp_Explorer exp(m_face, TopAbs_VERTEX); exp.More(); exp.Next() )
  {
    const TopoDS_Vertex& V = TopoDS::Vertex( exp.Current() );
    gp_Pnt               P = BRep_Tool::Pnt(V);
    //
    m_points2d.push_back( BRep_Tool::Parameters(V, m_face).XY() );
    m_points3d.push_back( P.XYZ() );
  }
}

//! Returns coordinates by 1-based index.
//! \param oneBasedIndex [in] 1-based index of a point.
//! \return coordinates.
const gp_XYZ& asiAlgo_FacePointCloud::Value3d(const int oneBasedIndex) const
{
  return m_points3d[oneBasedIndex - 1];
}

//! \return base face.
const TopoDS_Face& asiAlgo_FacePointCloud::Face() const
{
  return m_face;
}

//! Sets base face.
//! \param face [in] face to set.
void asiAlgo_FacePointCloud::SetFace(const TopoDS_Face& face)
{
  m_face = face;
}

//! Creates empty copy of the point cloud.
//! \return empty copy.
Handle(asiAlgo_CommonCloud2d) asiAlgo_FacePointCloud::EmptyCopy() const
{
  Handle(asiAlgo_FacePointCloud) cloud = new asiAlgo_FacePointCloud;
  cloud->SetFace(m_face);
  return cloud;
}

//! Returns coordinates by 1-based index.
//! \param oneBasedIndex [in] 1-based index of a point.
//! \return coordinates.
const gp_XY& asiAlgo_FacePointCloud::Value(const int oneBasedIndex) const
{
  return m_points2d[oneBasedIndex - 1];
}

//! \return number of points.
const int asiAlgo_FacePointCloud::Length() const
{
  return (int) m_points2d.size();
}

//! \return coordinates of the managed points as a plain array.
Handle(TColStd_HArray1OfReal) asiAlgo_FacePointCloud::Coords2d() const
{
  const int numPts = this->Length();
  if ( !numPts )
    return NULL;

  Handle(TColStd_HArray1OfReal) result = new TColStd_HArray1OfReal(0, numPts - 1);
  //
  for ( int pidx = 0; pidx < numPts; ++pidx )
  {
    result->ChangeValue( pidx*2 )     = this->Value(pidx + 1).X();
    result->ChangeValue( pidx*2 + 1 ) = this->Value(pidx + 1).Y();
  }

  return result;
}

//! Initializes point cloud from the given array.
//! \param coords [in] coordinates of the points.
void asiAlgo_FacePointCloud::SetCoords2d(const Handle(TColStd_HArray1OfReal)& coords)
{
  m_points2d.clear();
  m_points3d.clear();
  //
  if ( coords.IsNull() )
    return;

  for ( int coordidx = 0; coordidx < coords->Length(); coordidx += 2 )
  {
    gp_XY pt( coords->Value(coordidx), coords->Value(coordidx + 1) );
    m_points2d.push_back(pt);
  }
}

//! Adds the passed point to the point cloud.
//! \param coord [in] point to add.
void asiAlgo_FacePointCloud::AddCoord(const gp_XY& coord)
{
  // Add 2D point
  m_points2d.push_back(coord);

  // Restore 3D
  Handle(Geom_Surface) S = BRep_Tool::Surface(m_face);
  gp_Pnt p3d = S->Value( coord.X(), coord.Y() );

  // Add 3D point
  m_points3d.push_back( p3d.XYZ() );
}
