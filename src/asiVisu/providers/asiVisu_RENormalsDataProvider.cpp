//-----------------------------------------------------------------------------
// Created on: 09 December 2016
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
#include <asiVisu_RENormalsDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param pointsNode  [in] points Node.
//! \param normalsNode [in] normals Node.
asiVisu_RENormalsDataProvider::asiVisu_RENormalsDataProvider(const Handle(asiData_REPointsNode)&  pointsNode,
                                                             const Handle(asiData_RENormalsNode)& normalsNode)
: asiVisu_VectorsDataProvider(pointsNode)
{
  m_vectors = normalsNode;
}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_BaseCloud<float>) asiVisu_RENormalsDataProvider::GetPointsf()
{
  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast(m_source);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  return points_n->GetPointsf();
}

//-----------------------------------------------------------------------------

//! \return normals to visualize.
Handle(asiAlgo_BaseCloud<float>) asiVisu_RENormalsDataProvider::GetVectorsf()
{
  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_vectors);
  //
  if ( normals_n.IsNull() || !normals_n->IsWellFormed() )
    return NULL;

  return normals_n->GetNormalsf();
}

//-----------------------------------------------------------------------------

//! \return max modulus for a normal.
double asiVisu_RENormalsDataProvider::GetMaxVectorModulus() const
{
  return 10.0;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RENormalsDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_vectors);
  //
  if ( normals_n.IsNull() || !normals_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << normals_n->Parameter(asiData_RENormalsNode::PID_Normals);

  return out;
}
