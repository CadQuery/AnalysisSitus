//-----------------------------------------------------------------------------
// Created on: 21 July 2019
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
#include <asiVisu_ReCoedgeDataProvider.h>

// asiData includes
#include <asiData_RePatchNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

asiVisu_ReCoedgeDataProvider::asiVisu_ReCoedgeDataProvider(const Handle(asiData_ReCoedgeNode)& N)
: asiVisu_DataProvider (),
  m_node               (N)
{}

//-----------------------------------------------------------------------------

Handle(Geom_Curve) asiVisu_ReCoedgeDataProvider::GetCurve() const
{
  double f, l;
  return m_node->GetEdge()->GetCurve(f, l);
}

//-----------------------------------------------------------------------------

Handle(Geom_Surface) asiVisu_ReCoedgeDataProvider::GetSurface() const
{
  Handle(asiData_RePatchNode)
    patch = Handle(asiData_RePatchNode)::DownCast( m_node->GetParentNode() );

  return patch->GetSurface();
}

//-----------------------------------------------------------------------------

bool asiVisu_ReCoedgeDataProvider::GetSameSense() const
{
  return m_node->IsSameSense();
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList)
  asiVisu_ReCoedgeDataProvider::translationSources() const
{
  ActAPI_ParameterStream out;
  out << m_node->Parameter(asiData_ReCoedgeNode::PID_EdgeRef)
      << m_node->Parameter(asiData_ReCoedgeNode::PID_SameSense)
      << m_node->GetParentNode()->Parameter(asiData_RePatchNode::PID_Surface);

  return out.List;
}
