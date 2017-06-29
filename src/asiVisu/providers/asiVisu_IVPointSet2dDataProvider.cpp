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
#include <asiVisu_IVPointSet2dDataProvider.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param pointSet [in] point set to source data from.
asiVisu_IVPointSet2dDataProvider::asiVisu_IVPointSet2dDataProvider(const Handle(ActAPI_INode)& pointSet)
: asiVisu_PointsDataProvider(pointSet)
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_IVPointSet2dDataProvider::GetPoints() const
{
  Handle(asiData_IVPointSet2dNode)
    points_n = Handle(asiData_IVPointSet2dNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  Handle(TColStd_HArray1OfReal) coords2d = points_n->GetPoints();
  if ( coords2d.IsNull() )
    return NULL;

  // Re-pack to 3d
  const int nPts = coords2d->Length() / 2;
  Handle(TColStd_HArray1OfReal) coords3d = new TColStd_HArray1OfReal(0, nPts*3 - 1);
  //
  for ( int coordidx3d = 0, coordidx2d = 0; coordidx3d < coords3d->Length(); coordidx3d += 3, coordidx2d += 2 )
  {
    coords3d->ChangeValue(coordidx3d + 0) = coords2d->Value(coordidx2d);
    coords3d->ChangeValue(coordidx3d + 1) = coords2d->Value(coordidx2d + 1);
    coords3d->ChangeValue(coordidx3d + 2) = 0.0;
  }

  return asiAlgo_PointCloudUtils::AsPointCloud(coords3d);
}

//-----------------------------------------------------------------------------

//! \return NULL filter.
Handle(TColStd_HPackedMapOfInteger) asiVisu_IVPointSet2dDataProvider::GetIndices() const
{
  return NULL;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_IVPointSet2dDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_IVPointSet2dNode)
    points_n = Handle(asiData_IVPointSet2dNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(asiData_IVPointSet2dNode::PID_Geometry);

  return out;
}
