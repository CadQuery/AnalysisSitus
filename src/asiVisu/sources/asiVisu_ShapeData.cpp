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
