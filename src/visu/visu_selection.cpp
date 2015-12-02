//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_selection.h>

// OCCT includes
#include <Standard_ProgramError.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Default constructor.
//! \param theSelModes [in] selection mode the picking results are related to.
visu_pick_result::visu_pick_result(const int theSelModes)
{
  m_prevActor = NULL;
  m_iSelModes = theSelModes;
}

//! Destructor.
visu_pick_result::~visu_pick_result()
{
}

//-----------------------------------------------------------------------------

//! Streaming operator for VTK actors. Allows you to record new picking
//! result for the new actor.
//! \param theActor [in] actor to register in the collection of results.
//! \return this for subsequent streaming.
visu_pick_result&
  visu_pick_result::operator<<(const vtkSmartPointer<vtkActor>& theActor)
{
  m_prevActor = theActor;
  return *this;
}

//! Streaming operator for actor's element IDs. You have to register the parent
//! actor before invocation of this method.
//! \param theElemID [in] actor's element ID.
//! \return this for subsequent streaming.
visu_pick_result& visu_pick_result::operator<<(const vtkIdType theElemID)
{
  if ( m_prevActor == NULL )
    Standard_ProgramError::Raise("Invalid streaming order");

  if ( !m_pickMap.IsBound(m_prevActor) )
    m_pickMap.Bind( m_prevActor, TColStd_PackedMapOfInteger() );

  TColStd_PackedMapOfInteger& aCellMask = m_pickMap.ChangeFind(m_prevActor);
  int aComingID = (int) theElemID;

  // If such ID is already there, we exclude it. This behavior express
  // XOR principle of multiple selection
  if ( aCellMask.Contains(aComingID) )
    aCellMask.Remove(aComingID);
  else
    aCellMask.Add(aComingID);

  return *this;
}

//! Streaming operator for actor's element IDs. You have to register the parent
//! actor before invocation of this method.
//! \param theElemMask [in] actor's element mask.
//! \return this for subsequent streaming.
visu_pick_result& visu_pick_result::operator<<(const TColStd_PackedMapOfInteger& theElemMask)
{
  if ( m_prevActor == NULL )
    Standard_ProgramError::Raise("Invalid streaming order");

  if ( !m_pickMap.IsBound(m_prevActor) )
    m_pickMap.UnBind(m_prevActor);

  m_pickMap.Bind(m_prevActor, theElemMask);

  return *this;
}

//-----------------------------------------------------------------------------

//! Setter for selection modes.
//! \param theSelModes [in] selection modes to set.
void visu_pick_result::SetSelectionModes(const int theSelModes)
{
  m_iSelModes = theSelModes;
}

//! Accessor for the elements map.
//! \return elements map.
const visu_actor_elem_map& visu_pick_result::GetPickMap() const
{
  return m_pickMap;
}

//! Returns the overall number of selected elements regardless of their
//! belonging to one or another actor.
//! \return overall number of picked elements.
int visu_pick_result::NbElements() const
{
  int aResult = 0;
  for ( visu_actor_elem_map::Iterator it(m_pickMap); it.More(); it.Next() )
  {
    const TColStd_PackedMapOfInteger& aMask = it.Value();
    for ( TColStd_MapIteratorOfPackedMapOfInteger maskIt(aMask); maskIt.More(); maskIt.Next() )
      ++aResult;
  }
  return aResult;
}

//! Cleans up the internal collection.
void visu_pick_result::Clear()
{
  m_prevActor = NULL;
  m_pickMap.Clear();
}

//! Returns true if the result is empty, false -- otherwise.
//! \return true/false.
bool visu_pick_result::IsEmpty() const
{
  return m_pickMap.IsEmpty() > 0;
}

//! Checks whether the active selection mode covers the passed one.
//! \param theMode [in] selection mode to check.
bool visu_pick_result::DoesSelectionCover(const int theMode) const
{
  return ( (m_iSelModes ^ theMode) | m_iSelModes ) == m_iSelModes;
}

//! Checks whether the active selection mode is exclusive and equal to
//! the passed one.
//! \param theMode [in] selection mode to compare the current one with.
bool visu_pick_result::IsSelectionEqual(const int theMode) const
{
  return m_iSelModes == theMode;
}

//! Returns true if the associated selection mode corresponds to
//! disabled selection.
//! \return true/false.
bool visu_pick_result::IsSelectionNone() const
{
  return m_iSelModes & SelectionMode_None;
}

//! Returns true if the associated selection mode corresponds to
//! selection of workpiece (shape or mesh).
//! \return true/false.
bool visu_pick_result::IsSelectionWorkpiece() const
{
  return (m_iSelModes & SelectionMode_Workpiece) > 0;
}

//! Returns true if the associated selection mode corresponds to
//! selection of faces.
//! \return true/false.
bool visu_pick_result::IsSelectionFace() const
{
  return (m_iSelModes & SelectionMode_Face) > 0;
}

//! Returns true if the associated selection mode corresponds to
//! selection of edges.
//! \return true/false.
bool visu_pick_result::IsSelectionEdge() const
{
  return (m_iSelModes & SelectionMode_Edge) > 0;
}

//! Returns true if the associated selection mode corresponds to
//! selection of vertices.
//! \return true/false.
bool visu_pick_result::IsSelectionVertex() const
{
  return (m_iSelModes & SelectionMode_Vertex) > 0;
}

//! Returns true if the associated selection mode corresponds to
//! selection of any sub-shapes.
//! \return true/false.
bool visu_pick_result::IsSelectionSubShape() const
{
  return this->IsSelectionFace() ||
         this->IsSelectionEdge() ||
         this->IsSelectionVertex();
}
