//-----------------------------------------------------------------------------
// Created on: 11 July 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiVisu_TriangulationDataProvider.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

asiVisu_TriangulationDataProvider::asiVisu_TriangulationDataProvider()
: asiVisu_DataProvider ()
{}

//-----------------------------------------------------------------------------

asiVisu_TriangulationDataProvider::asiVisu_TriangulationDataProvider(const Handle(ActData_TriangulationParameter)& triangulationParam,
                                                                     const Handle(ActData_BoolParameter)&          hasScalarsParam,
                                                                     const Handle(ActData_IntParameter)&           colorParam)
: asiVisu_DataProvider ( ),
  m_node               ( triangulationParam->GetNode() ),
  m_triangulationParam ( triangulationParam ),
  m_hasScalarsParam    ( hasScalarsParam ),
  m_colorParam         ( colorParam )
{}

//-----------------------------------------------------------------------------

ActAPI_DataObjectId
  asiVisu_TriangulationDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

Handle(Poly_Triangulation)
  asiVisu_TriangulationDataProvider::GetTriangulation() const
{
  return m_triangulationParam->GetTriangulation();
}

//-----------------------------------------------------------------------------

bool asiVisu_TriangulationDataProvider::HasScalars() const
{
  return m_hasScalarsParam.IsNull() ? false : m_hasScalarsParam->GetValue();
}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationDataProvider::GetColor(double& r, double& g, double& b) const
{
  const int icolor = m_colorParam->GetValue();

  QColor qcolor = asiVisu_Utils::IntToColor(icolor);

  r = qcolor.redF();
  g = qcolor.greenF();
  b = qcolor.blueF();
}

//-----------------------------------------------------------------------------

Handle(asiVisu_TriangulationDataProvider)
  asiVisu_TriangulationDataProvider::Clone() const
{
  return new asiVisu_TriangulationDataProvider(m_triangulationParam,
                                               m_hasScalarsParam,
                                               m_colorParam);
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList)
  asiVisu_TriangulationDataProvider::translationSources() const
{
  ActParamStream params;

  params << m_triangulationParam;

  if ( !m_hasScalarsParam.IsNull() )
    params << m_hasScalarsParam;

  if ( !m_colorParam.IsNull() )
    params << m_colorParam;

  return params.List;
}
