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

#ifndef asiVisu_Selection_h
#define asiVisu_Selection_h

// asiVisu includes
#include <asiVisu.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkSmartPointer.h>

// Qt includes
#pragma warning(push, 0)
#include <QPoint>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Picker type.
enum asiVisu_PickerType
{
  PickerType_Cell  = 0x0001,
  PickerType_Point = 0x0002,
  PickerType_World = 0x0004
};

//-----------------------------------------------------------------------------

//! Selection type.
enum asiVisu_SelectionNature
{
  SelectionNature_None,     //!< Nothing.
  SelectionNature_Pick = 1, //!< User makes final decision on selection.
  SelectionNature_Detection //!< User observes selection possibilities.
};

//-----------------------------------------------------------------------------

//! Selection mode.
enum asiVisu_SelectionMode
{
  SelectionMode_None      = 0x0001, //!< Selection is disabled.
  SelectionMode_Workpiece = 0x0002, //!< Any shape or mesh.
  SelectionMode_Face      = 0x0004, //!< Faces only.
  SelectionMode_Edge      = 0x0008, //!< Edges only.
  SelectionMode_Vertex    = 0x0010, //!< Vertices only.

  //! Group of NONE selection mode only.
  SelectionMode_DummySet  = SelectionMode_None,

  //! Group of NONE and WORKPIECE selection modes.
  SelectionMode_GlobalSet = SelectionMode_None |
                            SelectionMode_Workpiece,

  //! Group of local selection modes: FACE, EDGE, VERTEX.
  SelectionMode_LocalSet  = SelectionMode_Face |
                            SelectionMode_Edge |
                            SelectionMode_Vertex,

  //! Group of all possible selection modes.
  SelectionMode_Unverse   = SelectionMode_DummySet |
                            SelectionMode_GlobalSet |
                            SelectionMode_LocalSet
};

//-----------------------------------------------------------------------------

//! Class representing picking inputs. This structure is designed to be
//! used in pick callbacks in order to retrieve all data required to
//! shoot the actual picking request.
struct asiVisu_PickInput
{
  QPoint Start;      //!< Picked (X, Y) coordinates of start event.
  QPoint Finish;     //!< Picked (X, Y) coordinates of finish event.
  bool   IsMultiple; //!< Indicates whether the multiple picking is enabled.

  //! Default constructor
  asiVisu_PickInput()
  {
    this->IsMultiple = false;
  }

  //! Complete constructor.
  //! \param theStart   [in] start picking point.
  //! \param theFinish  [in] finish picking point.
  //! \param isMultiple [in] indicates whether the multiple picking is enabled.
  asiVisu_PickInput(const QPoint& theStart,
                    const QPoint& theFinish,
                    const bool    isMultiple)
  {
    this->Start      = theStart;
    this->Finish     = theFinish;
    this->IsMultiple = isMultiple;
  }
};

//-----------------------------------------------------------------------------

//! Class representing picking results for different kinds of selection.
class asiVisu_PickerResult : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PickerResult, Standard_Transient)

public:

  asiVisu_EXPORT
    asiVisu_PickerResult(const int selModes = SelectionMode_None);

public:

  asiVisu_EXPORT void
    SetPickedPos(const double x, const double y, const double z);

  asiVisu_EXPORT void
    SetPickedActor(const vtkSmartPointer<vtkActor>& actor);

  asiVisu_EXPORT void
    SetSelectionModes(const int selModes);

  asiVisu_EXPORT void
    GetPickedPos(double& x, double& y, double& z) const;

  asiVisu_EXPORT const vtkSmartPointer<vtkActor>&
    GetPickedActor() const;

  asiVisu_EXPORT virtual void
    Clear();

  asiVisu_EXPORT bool
    IsEmpty() const;

  asiVisu_EXPORT bool
    IsSelectionWorkpiece() const;

  asiVisu_EXPORT bool
    IsSelectionFace() const;

  asiVisu_EXPORT bool
    IsSelectionEdge() const;

  asiVisu_EXPORT bool
    IsSelectionVertex() const;

  asiVisu_EXPORT bool
    IsSelectionSubShape() const;

public:

  virtual asiVisu_PickerType GetPickerType() const = 0;

private:

  //! Picked position in world coordinates.
  double m_pickPos[3];

  //! Picked actor.
  vtkSmartPointer<vtkActor> m_pickedActor;

  //! Selection modes the results were obtained for.
  int m_iSelModes;

};

//-----------------------------------------------------------------------------

//! Result of cell picker.
class asiVisu_CellPickerResult : public asiVisu_PickerResult
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_CellPickerResult, asiVisu_PickerResult)

public:

  //! Ctor.
  //! \param[in] selModes selection modes.
  asiVisu_CellPickerResult(const int selModes = SelectionMode_None)
  : asiVisu_PickerResult(selModes) {}

  //! \return picker type.
  virtual asiVisu_PickerType GetPickerType() const { return PickerType_Cell; }

public:

  asiVisu_EXPORT void
    SetPickedElementId(const vtkIdType elemId);

  asiVisu_EXPORT void
    AddPickedElementId(const vtkIdType elemId);

  asiVisu_EXPORT void
    RemovePickedElementId(const vtkIdType elemId);

  asiVisu_EXPORT void
    SetPickedElementIds(const TColStd_PackedMapOfInteger& elemIds);

  asiVisu_EXPORT const TColStd_PackedMapOfInteger&
    GetPickedElementIds() const;

  asiVisu_EXPORT void
    SetPickedCellId(const vtkIdType cellId);

  asiVisu_EXPORT void
    AddPickedCellId(const vtkIdType cellId);

  asiVisu_EXPORT void
    RemovePickedCellId(const vtkIdType cellId);

  asiVisu_EXPORT void
    SetPickedCellIds(const TColStd_PackedMapOfInteger& cellIds);

  asiVisu_EXPORT const TColStd_PackedMapOfInteger&
    GetPickedCellIds() const;

  asiVisu_EXPORT virtual void
    Clear();

protected:

  //! Picked element IDs.
  TColStd_PackedMapOfInteger m_pickedElementIds;

  //! Picked cell IDs.
  TColStd_PackedMapOfInteger m_pickedCellIds;

};

//-----------------------------------------------------------------------------

//! Result of point picker.
class asiVisu_PointPickerResult : public asiVisu_PickerResult
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PointPickerResult, asiVisu_PickerResult)

public:

  //! Ctor.
  //! \param[in] selModes selection modes.
  asiVisu_PointPickerResult(const int selModes = SelectionMode_None)
  : asiVisu_PickerResult(selModes) {}

  //! \return picker type.
  virtual asiVisu_PickerType GetPickerType() const { return PickerType_Point; }

public:

  asiVisu_EXPORT void
    SetPickedPointId(const vtkIdType elemId);

  asiVisu_EXPORT void
    AddPickedPointId(const vtkIdType elemId);

  asiVisu_EXPORT void
    RemovePickedPointId(const vtkIdType elemId);

  asiVisu_EXPORT void
    SetPickedPointIds(const TColStd_PackedMapOfInteger& elemIds);

  asiVisu_EXPORT const TColStd_PackedMapOfInteger&
    GetPickedPointIds() const;

  asiVisu_EXPORT virtual void
    Clear();

protected:

  //! Picked point IDs.
  TColStd_PackedMapOfInteger m_pickedPointIds;

};

//-----------------------------------------------------------------------------

//! Result of world picker.
class asiVisu_WorldPickerResult : public asiVisu_PickerResult
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_WorldPickerResult, asiVisu_PickerResult)

public:

  //! Ctor.
  //! \param[in] selModes selection modes.
  asiVisu_WorldPickerResult(const int selModes = SelectionMode_None)
  : asiVisu_PickerResult(selModes) {}

  //! \return picker type.
  virtual asiVisu_PickerType GetPickerType() const { return PickerType_World; }

};

#endif
