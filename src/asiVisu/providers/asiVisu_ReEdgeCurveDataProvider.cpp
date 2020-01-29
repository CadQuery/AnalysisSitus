//-----------------------------------------------------------------------------
// Created on: 01 July 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiVisu_ReEdgeCurveDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

asiVisu_ReEdgeCurveDataProvider::asiVisu_ReEdgeCurveDataProvider(const Handle(asiData_ReEdgeNode)& N)
: asiVisu_CurveDataProvider (),
  m_node                    (N)
{}

//-----------------------------------------------------------------------------

Handle(Standard_Type) asiVisu_ReEdgeCurveDataProvider::GetCurveType() const
{
  Handle(Geom_Curve) curve = m_node->GetCurve();
  //
  if ( curve.IsNull() )
    return nullptr;

  return curve->DynamicType();
}

//-----------------------------------------------------------------------------

Handle(Geom_Curve) asiVisu_ReEdgeCurveDataProvider::GetCurve(double& f, double& l) const
{
  return m_node->GetCurve(f, l);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList)
  asiVisu_ReEdgeCurveDataProvider::translationSources() const
{
  ActAPI_ParameterStream out;
  out << m_node->Parameter(asiData_ReEdgeNode::PID_Curve);

  return out.List;
}
