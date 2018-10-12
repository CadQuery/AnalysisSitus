//-----------------------------------------------------------------------------
// Created on: 10 october 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

// Own include
#include <asiVisu_ContourPolesDataProvider.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

//-----------------------------------------------------------------------------

//! Constructor.
asiVisu_ContourPolesDataProvider::asiVisu_ContourPolesDataProvider(const Handle(asiData_ContourNode)& contour)
: asiVisu_PointsDataProvider(contour)
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_ContourPolesDataProvider::GetPoints() const
{
  Handle(asiData_ContourNode)
    contour_n = Handle(asiData_ContourNode)::DownCast(m_node);
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    return NULL;

  // Get indices of poles.
  Handle(TColStd_HPackedMapOfInteger) hPoleIds = contour_n->GetPoleIndices();

  // Prepare result.
  Handle(asiAlgo_BaseCloud<double>) res = new asiAlgo_BaseCloud<double>;

  // Get coordinates.
  Handle(HRealArray) hCoords = contour_n->GetCoords();
  const int numPts = contour_n->GetNumPoints();
  //
  for ( int idx = 0; idx < numPts; ++idx )
  {
    if ( !hPoleIds->Map().Contains(idx) )
      continue;

    const double x = hCoords->Value(3*idx + 0);
    const double y = hCoords->Value(3*idx + 1);
    const double z = hCoords->Value(3*idx + 2);
    //
    res->AddElement(x, y, z);
  }

  return res;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_ContourPolesDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_ContourNode)
    points_n = Handle(asiData_ContourNode)::DownCast(m_node);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << points_n->Parameter(asiData_ContourNode::PID_Coords)
      << points_n->Parameter(asiData_ContourNode::PID_Poles);

  return out;
}
