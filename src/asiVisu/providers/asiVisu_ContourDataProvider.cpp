//-----------------------------------------------------------------------------
// Created on: 19 September 2016
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
#include <asiVisu_ContourDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
//! \param contour [in] contour Node.
asiVisu_ContourDataProvider::asiVisu_ContourDataProvider(const Handle(asiData_ContourNode)& contour)
: asiVisu_ShapeDataProvider(), m_contour(contour)
{
  // Set Node ID
  m_nodeID = m_contour->GetId();

  Handle(ActAPI_HParameterList) params = new ActAPI_HParameterList;
  params->Append( m_contour->Parameter(asiData_ContourNode::PID_Coords) );
  params->Append( m_contour->Parameter(asiData_ContourNode::PID_IsClosed) );
  params->Append( m_contour->Parameter(asiData_ContourNode::PID_Geometry) );

  // Set tracked Parameters
  m_params = params;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape asiVisu_ContourDataProvider::GetShape() const
{
  return m_contour->AsShape();
}
