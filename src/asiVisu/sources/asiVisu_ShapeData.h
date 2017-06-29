//-----------------------------------------------------------------------------
// Created on: 20 March 2017
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

#ifndef asiVisu_ShapeData_h
#define asiVisu_ShapeData_h

// asiVisu includes
#include <asiVisu_ShapePrimitive.h>

// OCCT includes
#include <Standard_Type.hxx>

// VTK includes
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

//! Wrapper for vtkPolyData representing B-Rep shape.
class asiVisu_ShapeData : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeData, Standard_Transient)

public:

  asiVisu_EXPORT
    asiVisu_ShapeData();

public:

  asiVisu_EXPORT virtual vtkIdType
    InsertCoordinate(const double x,
                     const double y,
                     const double z);

  asiVisu_EXPORT virtual void
    InsertVertex(vtkIdType              shapeID,
                 vtkIdType              pid,
                 asiVisu_ShapePrimitive scType);

  asiVisu_EXPORT virtual void
    InsertLine(vtkIdType              shapeID,
               vtkIdType              pid1,
               vtkIdType              pid2,
               asiVisu_ShapePrimitive scType);

  asiVisu_EXPORT virtual void
    InsertPolyline(vtkIdType                         shapeID,
                   const vtkSmartPointer<vtkIdList>& pids,
                   asiVisu_ShapePrimitive            scType);

  asiVisu_EXPORT virtual void
    InsertTriangle(vtkIdType              shapeID,
                   vtkIdType              pid1,
                   vtkIdType              pid2,
                   vtkIdType              pid3,
                   asiVisu_ShapePrimitive scType);


public:

  //! \return instance of vtkPolyData representing the shape.
  const vtkSmartPointer<vtkPolyData>& GetPolyData() const
  {
    return m_polyData;
  }

private:

  vtkSmartPointer<vtkPolyData>    m_polyData;       //!< Polygonal data.
  vtkSmartPointer<vtkIdTypeArray> m_subShapeIDs;    //!< Array of sub-shapes IDs.
  vtkSmartPointer<vtkIdTypeArray> m_shapeCellTypes; //!< Array of cell types.

};

#endif
