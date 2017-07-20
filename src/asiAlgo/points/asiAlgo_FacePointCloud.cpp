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
