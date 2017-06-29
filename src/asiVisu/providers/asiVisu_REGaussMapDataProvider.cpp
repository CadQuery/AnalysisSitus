//-----------------------------------------------------------------------------
// Created on: 25 April 2017
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
#include <asiVisu_REGaussMapDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param gaussNode [in] Gauss map Node.
asiVisu_REGaussMapDataProvider::asiVisu_REGaussMapDataProvider(const Handle(asiData_REGaussMapNode)& gaussNode)
: asiVisu_VectorsDataProvider(gaussNode)
{}

//-----------------------------------------------------------------------------

//! \todo see inline comment: the method deserves reworking.
//! \return points where normals are located.
Handle(asiAlgo_BaseCloud<float>) asiVisu_REGaussMapDataProvider::GetPointsf()
{
  // WARNING: the following is a bullshit. We should not allocate
  //          an array of points just to initialize it with zeroes.
  //          This should be reworked once we have a VTK source for
  //          vectors WITHOUT points.

  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return NULL;

  Handle(asiAlgo_BaseCloud<float>) vectors = gauss_n->GetVectorsf();
  Handle(asiAlgo_BaseCloud<float>) points  = new asiAlgo_BaseCloud<float>();
  //
  for ( int i = 0; i < vectors->GetNumberOfElements(); ++i )
    points->AddElement(0, 0, 0); // crappy. Crappy! CRAPPY!!!

  return points;
}

//-----------------------------------------------------------------------------

//! \return vectors to visualize.
Handle(asiAlgo_BaseCloud<float>) asiVisu_REGaussMapDataProvider::GetVectorsf()
{
  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return NULL;

  return gauss_n->GetVectorsf();
}

//-----------------------------------------------------------------------------

//! \return max modulus for a vector.
double asiVisu_REGaussMapDataProvider::GetMaxVectorModulus() const
{
  return 10.0;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_REGaussMapDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << gauss_n->Parameter(asiData_REGaussMapNode::PID_Vectors);

  return out;
}
