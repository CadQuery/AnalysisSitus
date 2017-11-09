//-----------------------------------------------------------------------------
// Created on: 27 April 2017
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
#include <asiVisu_REGaussMapRefinedAxisDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param gaussNode [in] Gauss map Node.
asiVisu_REGaussMapRefinedAxisDataProvider::asiVisu_REGaussMapRefinedAxisDataProvider(const Handle(asiData_REGaussMapNode)& gaussNode)
: asiVisu_VectorsDataProvider(gaussNode)
{}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_BaseCloud<float>) asiVisu_REGaussMapRefinedAxisDataProvider::GetPointsf()
{
  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return NULL;

  Handle(asiAlgo_BaseCloud<float>) points = new asiAlgo_BaseCloud<float>();
  //
  points->AddElement(0, 0, 0);

  return points;
}

//-----------------------------------------------------------------------------

//! \return vectors to visualize.
Handle(asiAlgo_BaseCloud<float>) asiVisu_REGaussMapRefinedAxisDataProvider::GetVectorsf()
{
  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return NULL;

  double ax, ay, az;
  gauss_n->GetRefinedAxis(ax, ay, az);

  Handle(asiAlgo_BaseCloud<float>) vectors = new asiAlgo_BaseCloud<float>();
  //
  vectors->AddElement( (float) ax, (float) ay, (float) az );

  return vectors;
}

//-----------------------------------------------------------------------------

//! \return max modulus for a vector.
double asiVisu_REGaussMapRefinedAxisDataProvider::GetMaxVectorModulus() const
{
  return 10.0;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_REGaussMapRefinedAxisDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return out;

  // Register Parameters
  out << gauss_n->Parameter(asiData_REGaussMapNode::PID_RefinedAxisX)
      << gauss_n->Parameter(asiData_REGaussMapNode::PID_RefinedAxisY)
      << gauss_n->Parameter(asiData_REGaussMapNode::PID_RefinedAxisZ);

  return out;
}
