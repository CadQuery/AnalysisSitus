//-----------------------------------------------------------------------------
// Created on: 20 March 2017
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
