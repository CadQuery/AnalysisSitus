//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_Selection_h
#define asiVisu_Selection_h

// Visualization includes
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
#include <QtCore/QPoint>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Picker type.
enum asiVisu_PickType
{
  PickType_Cell = 0,
  PickType_Point,
  PickType_World
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
  SelectionMode_Workpiece = 0x0002, //!< Entire shape or mesh.
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
  QPoint Start;      //!< Picked X coordinate.
  QPoint Finish;     //!< Picked Y coordinate.
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
class asiVisu_PickResult
{
public:

  asiVisu_EXPORT
    asiVisu_PickResult(const int selModes = SelectionMode_None);

public:

  asiVisu_EXPORT void
    SetPickedPos(const double x, const double y, const double z);

  asiVisu_EXPORT void
    SetPickedActor(const vtkSmartPointer<vtkActor>& actor);

  asiVisu_EXPORT void
    SetPickedElementId(const vtkIdType elemId);

  asiVisu_EXPORT void
    AddPickedElementId(const vtkIdType elemId);

  asiVisu_EXPORT void
    SetPickedElementIds(const TColStd_PackedMapOfInteger& elemIds);

  asiVisu_EXPORT void
    SetPickedPointId(const vtkIdType pointId);

  asiVisu_EXPORT void
    AddPickedPointId(const vtkIdType pointId);

  asiVisu_EXPORT void
    SetPickedPointIds(const TColStd_PackedMapOfInteger& pointIds);

  asiVisu_EXPORT void
    SetPickedCellId(const vtkIdType cellId);

  asiVisu_EXPORT void
    AddPickedCellId(const vtkIdType cellId);

  asiVisu_EXPORT void
    SetPickedCellIds(const TColStd_PackedMapOfInteger& cellIds);

  asiVisu_EXPORT void
    SetSelectionModes(const int selModes);

public:

  asiVisu_EXPORT void
    GetPickedPos(double& x, double& y, double& z) const;

  asiVisu_EXPORT const vtkSmartPointer<vtkActor>&
    GetPickedActor() const;

  asiVisu_EXPORT const TColStd_PackedMapOfInteger&
    GetPickedElementIds() const;

  asiVisu_EXPORT const TColStd_PackedMapOfInteger&
    GetPickedPointIds() const;

  asiVisu_EXPORT const TColStd_PackedMapOfInteger&
    GetPickedCellIds() const;

  asiVisu_EXPORT void
    Clear();

  asiVisu_EXPORT bool
    IsEmpty() const;

  asiVisu_EXPORT bool
    DoesSelectionCover(const int mode) const;

  asiVisu_EXPORT bool
    IsSelectionEqual(const int mode) const;

  asiVisu_EXPORT bool
    IsSelectionNone() const;

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

private:

  //! Picked position in world coordinates.
  double m_pickPos[3];

  //! Picked actor.
  vtkSmartPointer<vtkActor> m_pickedActor;

  // Picked element IDs (application-specific).
  TColStd_PackedMapOfInteger m_pickedElementIds;

  // Picked point IDs.
  TColStd_PackedMapOfInteger m_pickedPointIds;

  // Picked cell IDs.
  TColStd_PackedMapOfInteger m_pickedCellIds;

  //! Selection modes the results were obtained for.
  int m_iSelModes;

};

#endif
