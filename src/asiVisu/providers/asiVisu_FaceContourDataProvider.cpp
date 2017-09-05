//-----------------------------------------------------------------------------
// Created on: 05 September 2017
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
#include <asiVisu_FaceContourDataProvider.h>

// OCCT includes
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param faceNode [in] Face Contour Node.
asiVisu_FaceContourDataProvider::asiVisu_FaceContourDataProvider(const Handle(asiData_FaceContourNode)& faceNode)
: asiVisu_DataProvider()
{
  // Initialize handle to the data source Node
  m_source = faceNode;

  // Access owning geometry
  m_partNode = Handle(asiData_PartNode)::DownCast( m_source->GetParentNode() );
}

//-----------------------------------------------------------------------------

//! \return working face.
TopoDS_Face asiVisu_FaceContourDataProvider::GetFace() const
{
  const int globalId = m_source->GetSelectedFace();
  if ( !globalId )
    return TopoDS_Face();

  const TopTools_IndexedMapOfShape&
    subShapes = m_partNode->GetAAG()->GetMapOfSubShapes();

  const TopoDS_Shape& shape = subShapes.FindKey(globalId);
  //
  if ( shape.ShapeType() != TopAbs_FACE )
    return TopoDS_Face();

  // Access face by the stored index
  const TopoDS_Face& F = TopoDS::Face(shape);
  return F;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList)
  asiVisu_FaceContourDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  if ( m_source.IsNull() || !m_source->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << m_source->Parameter(asiData_FaceContourNode::PID_SelectedFace)
      << m_partNode->Parameter(asiData_PartNode::PID_Geometry)
      << m_partNode->Parameter(asiData_PartNode::PID_AAG);

  return out;
}
