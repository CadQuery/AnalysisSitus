//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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
