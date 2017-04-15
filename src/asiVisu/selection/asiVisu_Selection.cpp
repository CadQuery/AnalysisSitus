//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_Selection.h>

// OCCT includes
#include <Standard_ProgramError.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Default constructor.
//! \param selModes [in] selection mode the picking results are related to.
asiVisu_PickResult::asiVisu_PickResult(const int selModes)
{
  m_iSelModes = selModes;
  m_pickPos[0] = m_pickPos[1] = m_pickPos[2] = 0.0;
}

//-----------------------------------------------------------------------------

//! Sets picked position.
//! \param x [in] x coordinate.
//! \param y [in] y coordinate.
//! \param z [in] z coordinate.
void asiVisu_PickResult::SetPickedPos(const double x, const double y, const double z)
{
  m_pickPos[0] = x;
  m_pickPos[1] = y;
  m_pickPos[2] = z;
}

//-----------------------------------------------------------------------------

//! Sets picked actor.
//! \param actor [in] actor to set as picking result.
//! \return this for subsequent streaming.
void asiVisu_PickResult::SetPickedActor(const vtkSmartPointer<vtkActor>& actor)
{
  m_pickedActor = actor;
}

//-----------------------------------------------------------------------------

//! Sets ID of the picked element. This ID is application-specific, e.g.
//! a sub-shape ID of a B-Rep part.
//! \param elemId [in] ID to set.
void asiVisu_PickResult::SetPickedElementId(const vtkIdType elemId)
{
  m_pickedElementIds.Clear();
  m_pickedElementIds.Add(elemId);
}

//-----------------------------------------------------------------------------

//! Adds ID of the picked element. This ID is application-specific, e.g.
//! a sub-shape ID of a B-Rep part.
//! \param elemId [in] ID to add.
void asiVisu_PickResult::AddPickedElementId(const vtkIdType elemId)
{
  m_pickedElementIds.Add(elemId);
}

//-----------------------------------------------------------------------------

//! Sets IDs of the picked elements. These IDs are application-specific, e.g.
//! sub-shape IDs of a B-Rep part.
//! \param elemIds [in] IDs to set.
void asiVisu_PickResult::SetPickedElementIds(const TColStd_PackedMapOfInteger& elemIds)
{
  m_pickedElementIds = elemIds;
}

//-----------------------------------------------------------------------------

//! Sets ID of the picked VTK point.
//! \param pointId [in] ID to set.
void asiVisu_PickResult::SetPickedPointId(const vtkIdType pointId)
{
  m_pickedPointIds.Clear();
  m_pickedPointIds.Add(pointId);
}

//-----------------------------------------------------------------------------

//! Adds ID of the picked point.
//! \param pointId [in] ID to add.
void asiVisu_PickResult::AddPickedPointId(const vtkIdType pointId)
{
  m_pickedPointIds.Add(pointId);
}

//-----------------------------------------------------------------------------

//! Sets IDs of the picked VTK points.
//! \param pointIds [in] ID to set.
void asiVisu_PickResult::SetPickedPointIds(const TColStd_PackedMapOfInteger& pointIds)
{
  m_pickedPointIds = pointIds;
}

//-----------------------------------------------------------------------------

//! Sets ID of the picked VTK cell.
//! \param cellId [in] ID to set.
void asiVisu_PickResult::SetPickedCellId(const vtkIdType cellId)
{
  m_pickedCellIds.Clear();
  m_pickedCellIds.Add(cellId);
}

//-----------------------------------------------------------------------------

//! Adds ID of the picked cell.
//! \param cellId [in] ID to add.
void asiVisu_PickResult::AddPickedCellId(const vtkIdType cellId)
{
  m_pickedCellIds.Add(cellId);
}

//-----------------------------------------------------------------------------

//! Sets IDs of the picked VTK cells.
//! \param cellIds [in] ID to set.
void asiVisu_PickResult::SetPickedCellIds(const TColStd_PackedMapOfInteger& cellIds)
{
  m_pickedCellIds = cellIds;
}

//-----------------------------------------------------------------------------

//! Setter for selection modes.
//! \param selModes [in] selection modes to set.
void asiVisu_PickResult::SetSelectionModes(const int selModes)
{
  m_iSelModes = selModes;
}

//-----------------------------------------------------------------------------

//! Returns picked position.
//! \param x [out] x coordinate.
//! \param y [out] y coordinate.
//! \param z [out] z coordinate.
void asiVisu_PickResult::GetPickedPos(double& x, double& y, double& z) const
{
  x = m_pickPos[0];
  y = m_pickPos[1];
  z = m_pickPos[2];
}

//-----------------------------------------------------------------------------

//! \return picked actor.
const vtkSmartPointer<vtkActor>& asiVisu_PickResult::GetPickedActor() const
{
  return m_pickedActor;
}

//-----------------------------------------------------------------------------

//! \return picked element IDs.
const TColStd_PackedMapOfInteger& asiVisu_PickResult::GetPickedElementIds() const
{
  return m_pickedElementIds;
}

//-----------------------------------------------------------------------------

//! \return picked point IDs.
const TColStd_PackedMapOfInteger& asiVisu_PickResult::GetPickedPointIds() const
{
  return m_pickedPointIds;
}

//-----------------------------------------------------------------------------

//! \return picked cell IDs.
const TColStd_PackedMapOfInteger& asiVisu_PickResult::GetPickedCellIds() const
{
  return m_pickedCellIds;
}

//-----------------------------------------------------------------------------

//! Cleans up the internal collection.
void asiVisu_PickResult::Clear()
{
  m_pickedActor = NULL;
  m_pickedElementIds.Clear();
  m_pickedCellIds.Clear();
  m_pickedPointIds.Clear();
  m_pickPos[0] = m_pickPos[1] = m_pickPos[2] = 0.0;
}

//-----------------------------------------------------------------------------

//! Returns true if the result is empty, false -- otherwise.
//! \return true/false.
bool asiVisu_PickResult::IsEmpty() const
{
  return m_pickedActor.GetPointer() == NULL;
}

//-----------------------------------------------------------------------------

//! Checks whether the active selection mode covers the passed one.
//! \param mode [in] selection mode to check.
bool asiVisu_PickResult::DoesSelectionCover(const int mode) const
{
  return ( (m_iSelModes ^ mode) | m_iSelModes ) == m_iSelModes;
}

//-----------------------------------------------------------------------------

//! Checks whether the active selection mode is exclusive and equal to
//! the passed one.
//! \param mode [in] selection mode to compare the current one with.
bool asiVisu_PickResult::IsSelectionEqual(const int mode) const
{
  return m_iSelModes == mode;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! disabled selection.
//! \return true/false.
bool asiVisu_PickResult::IsSelectionNone() const
{
  return m_iSelModes & SelectionMode_None;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of workpiece (shape or mesh).
//! \return true/false.
bool asiVisu_PickResult::IsSelectionWorkpiece() const
{
  return (m_iSelModes & SelectionMode_Workpiece) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of faces.
//! \return true/false.
bool asiVisu_PickResult::IsSelectionFace() const
{
  return (m_iSelModes & SelectionMode_Face) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of edges.
//! \return true/false.
bool asiVisu_PickResult::IsSelectionEdge() const
{
  return (m_iSelModes & SelectionMode_Edge) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of vertices.
//! \return true/false.
bool asiVisu_PickResult::IsSelectionVertex() const
{
  return (m_iSelModes & SelectionMode_Vertex) > 0;
}

//-----------------------------------------------------------------------------

//! Returns true if the associated selection mode corresponds to
//! selection of any sub-shapes.
//! \return true/false.
bool asiVisu_PickResult::IsSelectionSubShape() const
{
  return this->IsSelectionFace() ||
         this->IsSelectionEdge() ||
         this->IsSelectionVertex();
}
