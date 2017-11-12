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
#include <asiVisu_TriangulationDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_TriangulationDataProvider::asiVisu_TriangulationDataProvider()
: asiVisu_DataProvider()
{}

//-----------------------------------------------------------------------------

//! Constructor accepting the data source Node.
//! \param N [in] Triangulation Node.
asiVisu_TriangulationDataProvider::asiVisu_TriangulationDataProvider(const Handle(asiData_TriangulationNode)& N)
: asiVisu_DataProvider(), m_node(N)
{}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId
  asiVisu_TriangulationDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! \return Triangulation Node.
Handle(asiData_TriangulationNode)
  asiVisu_TriangulationDataProvider::GetTriangulationNode() const
{
  return m_node;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT triangulation to be visualized.
//! \return OCCT facet model.
Handle(Poly_Triangulation)
  asiVisu_TriangulationDataProvider::GetTriangulation() const
{
  return m_node->GetTriangulation();
}

//-----------------------------------------------------------------------------

//! \return copy.
Handle(asiVisu_TriangulationDataProvider)
  asiVisu_TriangulationDataProvider::Clone() const
{
  return new asiVisu_TriangulationDataProvider(m_node);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList)
  asiVisu_TriangulationDataProvider::translationSources() const
{
  ActParamStream params;

  params << m_node->Parameter(asiData_TriangulationNode::PID_Triangulation);

  return params.List;
}
