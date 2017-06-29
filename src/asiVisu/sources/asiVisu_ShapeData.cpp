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

// Own include
#include <asiVisu_ShapeData.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------

asiVisu_ShapeData::asiVisu_ShapeData()
{
  // Polygonal data for complete geometry
  m_polyData = vtkSmartPointer<vtkPolyData>::New();
  m_polyData->Allocate();
  m_polyData->SetPoints( vtkSmartPointer<vtkPoints>::New() );

  // Array to store sub-shape IDs. Pinned to cell data
  m_subShapeIDs = vtkSmartPointer<vtkIdTypeArray>::New();
  m_subShapeIDs->SetName(ARRNAME_PART_SUBSHAPE_IDS);
  m_subShapeIDs->SetNumberOfComponents(1);
  m_polyData->GetCellData()->SetPedigreeIds(m_subShapeIDs);

  // Array to store cell types with B-Rep perspective. Pinned to cell data
  m_shapeCellTypes = vtkSmartPointer<vtkIdTypeArray>::New();
  m_shapeCellTypes->SetName(ARRNAME_PART_CELL_TYPES);
  m_shapeCellTypes->SetNumberOfComponents(1);
  m_polyData->GetCellData()->AddArray(m_shapeCellTypes);
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_ShapeData::InsertCoordinate(const double x,
                                              const double y,
                                              const double z)
{
  return m_polyData->GetPoints()->InsertNextPoint(x, y, z);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeData::InsertVertex(vtkIdType              shapeID,
                                     vtkIdType              pid,
                                     asiVisu_ShapePrimitive scType)
{
  // Insert new cell
  m_polyData->InsertNextCell(VTK_VERTEX, 1, &pid);

  // Insert sub-shape ID
  m_subShapeIDs->InsertNextTypedTuple(&shapeID);

  // Insert type of the cell wrt its role in a B-Rep model
  vtkIdType scTypeId = (vtkIdType) scType;
  m_shapeCellTypes->InsertNextTypedTuple(&scTypeId);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeData::InsertLine(vtkIdType              shapeID,
                                   vtkIdType              pid1,
                                   vtkIdType              pid2,
                                   asiVisu_ShapePrimitive scType)
{
  // Insert new cell
  vtkIdType pids[2] = {pid1, pid2};
  m_polyData->InsertNextCell(VTK_LINE, 2, pids);

  // Insert sub-shape type
  m_subShapeIDs->InsertNextTypedTuple(&shapeID);

  // Insert type of the cell wrt its role in a B-Rep model
  vtkIdType scTypeId = (vtkIdType) scType;
  m_shapeCellTypes->InsertNextTypedTuple(&scTypeId);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeData::InsertPolyline(vtkIdType                         shapeID,
                                       const vtkSmartPointer<vtkIdList>& pids,
                                       asiVisu_ShapePrimitive            scType)
{
  if ( !pids.GetPointer() )
    return;

  // Insert new cell
  m_polyData->InsertNextCell(VTK_POLY_LINE, pids);

  // Insert sub-shape type
  m_subShapeIDs->InsertNextTypedTuple(&shapeID);

  // Insert type of the cell wrt its role in a B-Rep model
  vtkIdType scTypeId = (vtkIdType) scType;
  m_shapeCellTypes->InsertNextTypedTuple(&scTypeId);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeData::InsertTriangle(vtkIdType              shapeID,
                                       vtkIdType              pid1,
                                       vtkIdType              pid2,
                                       vtkIdType              pid3,
                                       asiVisu_ShapePrimitive scType)
{
  // Insert new cell
  vtkIdType pids[3] = {pid1, pid2, pid3};
  m_polyData->InsertNextCell(VTK_TRIANGLE, 3, pids);

  // Insert sub-shape type
  m_subShapeIDs->InsertNextTypedTuple(&shapeID);

  // Insert type of the cell wrt its role in a B-Rep model
  vtkIdType scTypeId = (vtkIdType) scType;
  m_shapeCellTypes->InsertNextTypedTuple(&scTypeId);
}
