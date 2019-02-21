//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiVisu_IVCurveDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

asiVisu_IVCurveDataProvider::asiVisu_IVCurveDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_CurveDataProvider (),
  m_node                    ( Handle(asiData_IVCurveNode)::DownCast(N) )
{}

//-----------------------------------------------------------------------------

Handle(Standard_Type)
  asiVisu_IVCurveDataProvider::GetCurveType() const
{
  double f, l;
  Handle(Geom_Curve) curve = this->GetCurve(f, l);

  if ( curve.IsNull() )
    return NULL;

  return curve->DynamicType();
}

//-----------------------------------------------------------------------------

Handle(Geom2d_Curve)
  asiVisu_IVCurveDataProvider::GetCurve2d(double&, double&) const
{
  return NULL;
}

//-----------------------------------------------------------------------------

Handle(Geom_Curve)
  asiVisu_IVCurveDataProvider::GetCurve(double& f, double& l) const
{
  if ( m_node.IsNull() )
    return NULL;

  return m_node->GetCurve(f, l);
}

//-----------------------------------------------------------------------------

Handle(HRealArray)
  asiVisu_IVCurveDataProvider::GetHandles() const
{
  if ( m_node.IsNull() )
    return NULL;

  return m_node->GetHandles();
}

//-----------------------------------------------------------------------------

int asiVisu_IVCurveDataProvider::GetActiveHandle() const
{
  if ( m_node.IsNull() )
    return -1;

  return m_node->GetActiveHandle();
}

//-----------------------------------------------------------------------------

void asiVisu_IVCurveDataProvider::GetReperPoints(std::vector<gp_XYZ>& pts) const
{
  if ( m_node.IsNull() )
    return;

  m_node->GetReperPoints(pts);
}

//-----------------------------------------------------------------------------

int asiVisu_IVCurveDataProvider::GetActiveReper() const
{
  if ( m_node.IsNull() )
    return -1;

  return m_node->GetActiveReper();
}

//-----------------------------------------------------------------------------

ActAPI_DataObjectId asiVisu_IVCurveDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

Handle(asiVisu_IVCurveDataProvider) asiVisu_IVCurveDataProvider::Clone() const
{
  return new asiVisu_IVCurveDataProvider(m_node);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList) asiVisu_IVCurveDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  if ( m_node.IsNull() )
    return out;

  // Register Parameter as sensitive
  out << m_node->Parameter(asiData_IVCurveNode::PID_Geometry)
      << m_node->Parameter(asiData_IVCurveNode::PID_Handles)
      << m_node->Parameter(asiData_IVCurveNode::PID_ActiveHandle)
      << m_node->Parameter(asiData_IVCurveNode::PID_ReperPoints);

  return out;
}
