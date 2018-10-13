//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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
#include <asiVisu_Selection.h>

// OCCT includes
#include <Standard_ProgramError.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Default constructor.
//! \param[in] selModes selection mode the picking results are related to.
asiVisu_PickerResult::asiVisu_PickerResult(const int selModes)
{
  m_iSelModes = selModes;
  m_pickPos[0] = m_pickPos[1] = m_pickPos[2] = 0.0;
}

//-----------------------------------------------------------------------------

//! Sets picked position.
//! \param[in] x x coordinate.
//! \param[in] y y coordinate.
//! \param[in] z z coordinate.
void asiVisu_PickerResult::SetPickedPos(const double x, const double y, const double z)
{
  m_pickPos[0] = x;
  m_pickPos[1] = y;
  m_pickPos[2] = z;
}

//-----------------------------------------------------------------------------

//! Sets picked actor.
//! \param[in] actor actor to set as picking result.
//! \return this for subsequent streaming.
void asiVisu_PickerResult::SetPickedActor(const vtkSmartPointer<vtkActor>& actor)
{
  m_pickedActor = actor;
}

//-----------------------------------------------------------------------------

//! Setter for selection modes.
//! \param[in] selModes selection modes to set.
void asiVisu_PickerResult::SetSelectionModes(const int selModes)
{
  m_iSelModes = selModes;
}

//-----------------------------------------------------------------------------

//! Returns picked position.
//! \param[out] x x coordinate.
//! \param[out] y y coordinate.
//! \param[out] z z coordinate.
void asiVisu_PickerResult::GetPickedPos(double& x, double& y, double& z) const
{
  x = m_pickPos[0];
  y = m_pickPos[1];
  z = m_pickPos[2];
}

//-----------------------------------------------------------------------------

//! \return picked actor.
const vtkSmartPointer<vtkActor>& asiVisu_PickerResult::GetPickedActor() const
{
  return m_pickedActor;
}

//-----------------------------------------------------------------------------

//! Cleans up the internally stored data structures.
void asiVisu_PickerResult::Clear()
{
  m_pickedActor = NULL;
  m_pickPos[0] = m_pickPos[1] = m_pickPos[2] = 0.0;
}

//-----------------------------------------------------------------------------

//! Returns true if the result is empty, false -- otherwise.
//! \return true/false.
bool asiVisu_PickerResult::IsEmpty() const
{
  return m_pickedActor.GetPointer() == NULL;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of workpiece (shape or mesh).
//! \return true/false.
bool asiVisu_PickerResult::IsSelectionWorkpiece() const
{
  return (m_iSelModes & SelectionMode_Workpiece) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of faces.
//! \return true/false.
bool asiVisu_PickerResult::IsSelectionFace() const
{
  return (m_iSelModes & SelectionMode_Face) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of edges.
//! \return true/false.
bool asiVisu_PickerResult::IsSelectionEdge() const
{
  return (m_iSelModes & SelectionMode_Edge) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of vertices.
//! \return true/false.
bool asiVisu_PickerResult::IsSelectionVertex() const
{
  return (m_iSelModes & SelectionMode_Vertex) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of any sub-shapes.
//! \return true/false.
bool asiVisu_PickerResult::IsSelectionSubShape() const
{
  return this->IsSelectionFace() ||
         this->IsSelectionEdge() ||
         this->IsSelectionVertex();
}

//-----------------------------------------------------------------------------

//! Sets ID of the picked element. This ID is application-specific, e.g.
//! a sub-shape ID of a B-Rep part.
//! \param[in] elemId ID to set.
void asiVisu_CellPickerResult::SetPickedElementId(const vtkIdType elemId)
{
  m_pickedElementIds.Clear();
  m_pickedElementIds.Add(elemId);
}

//-----------------------------------------------------------------------------

//! Adds ID of the picked element. This ID is application-specific, e.g.
//! a sub-shape ID of a B-Rep part.
//! \param[in] elemId ID to add.
void asiVisu_CellPickerResult::AddPickedElementId(const vtkIdType elemId)
{
  m_pickedElementIds.Add(elemId);
}

//-----------------------------------------------------------------------------

void asiVisu_CellPickerResult::RemovePickedElementId(const vtkIdType elemId)
{
  m_pickedElementIds.Remove(elemId);
}

//-----------------------------------------------------------------------------

//! Sets IDs of the picked elements. These IDs are application-specific, e.g.
//! sub-shape IDs of a B-Rep part.
//! \param[in] elemIds IDs to set.
void asiVisu_CellPickerResult::SetPickedElementIds(const TColStd_PackedMapOfInteger& elemIds)
{
  m_pickedElementIds = elemIds;
}

//-----------------------------------------------------------------------------

//! \return picked element IDs.
const TColStd_PackedMapOfInteger&
  asiVisu_CellPickerResult::GetPickedElementIds() const
{
  return m_pickedElementIds;
}

//-----------------------------------------------------------------------------

//! Sets ID of the picked cell.
//! \param[in] cellId ID to set.
void asiVisu_CellPickerResult::SetPickedCellId(const vtkIdType cellId)
{
  m_pickedCellIds.Clear();
  m_pickedCellIds.Add(cellId);
}

//-----------------------------------------------------------------------------

//! Adds ID of the picked cell.
//! \param[in] cellId ID to add.
void asiVisu_CellPickerResult::AddPickedCellId(const vtkIdType cellId)
{
  m_pickedCellIds.Add(cellId);
}

//-----------------------------------------------------------------------------

void asiVisu_CellPickerResult::RemovePickedCellId(const vtkIdType cellId)
{
  m_pickedCellIds.Remove(cellId);
}

//-----------------------------------------------------------------------------

//! Sets IDs of the picked cells.
//! \param[in] cellIds IDs to set.
void asiVisu_CellPickerResult::SetPickedCellIds(const TColStd_PackedMapOfInteger& cellIds)
{
  m_pickedCellIds = cellIds;
}

//-----------------------------------------------------------------------------

//! \return picked cell IDs.
const TColStd_PackedMapOfInteger&
  asiVisu_CellPickerResult::GetPickedCellIds() const
{
  return m_pickedCellIds;
}

//-----------------------------------------------------------------------------

void asiVisu_CellPickerResult::Clear()
{
  asiVisu_PickerResult::Clear();

  m_pickedCellIds.Clear();
  m_pickedElementIds.Clear();
}

//-----------------------------------------------------------------------------

//! Sets ID of the picked point.
//! \param[in] pointId ID to set.
void asiVisu_PointPickerResult::SetPickedPointId(const vtkIdType pointId)
{
  m_pickedPointIds.Clear();
  m_pickedPointIds.Add(pointId);
}

//-----------------------------------------------------------------------------

//! Adds ID of the picked point.
//! \param[in] pointId ID to add.
void asiVisu_PointPickerResult::AddPickedPointId(const vtkIdType pointId)
{
  m_pickedPointIds.Add(pointId);
}

//-----------------------------------------------------------------------------

void asiVisu_PointPickerResult::RemovePickedPointId(const vtkIdType pointId)
{
  m_pickedPointIds.Remove(pointId);
}

//-----------------------------------------------------------------------------

//! Sets IDs of the picked points.
//! \param[in] pointIds IDs to set.
void asiVisu_PointPickerResult::SetPickedPointIds(const TColStd_PackedMapOfInteger& pointIds)
{
  m_pickedPointIds = pointIds;
}

//-----------------------------------------------------------------------------

//! \return picked point IDs.
const TColStd_PackedMapOfInteger&
  asiVisu_PointPickerResult::GetPickedPointIds() const
{
  return m_pickedPointIds;
}

//-----------------------------------------------------------------------------

void asiVisu_PointPickerResult::Clear()
{
  asiVisu_PickerResult::Clear();

  m_pickedPointIds.Clear();
}
