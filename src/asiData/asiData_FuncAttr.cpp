//-----------------------------------------------------------------------------
// Created on: 02 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_FuncAttr.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------
// Construction & settling-down routines
//-----------------------------------------------------------------------------

//! Default constructor.
template <typename T_ARGUMENT>
asiData_FuncAttr<T_ARGUMENT>::asiData_FuncAttr() : TDF_Attribute()
{}

//! Settles down new Attribute to the given OCAF Label.
//! \param[in] Label TDF Label to settle down the new Attribute to.
//! \return newly created Attribute settled down onto the target Label.
template <typename T_ARGUMENT>
Handle(asiData_FuncAttr<T_ARGUMENT>)
  asiData_FuncAttr<T_ARGUMENT>::Set(const TDF_Label& Label)
{
  Handle(asiData_FuncAttr<T_ARGUMENT>) A;
  //
  if ( !Label.FindAttribute(GUID(), A) )
  {
    A = new asiData_FuncAttr<T_ARGUMENT>();
    Label.AddAttribute(A);
  }
  return A;
}

//-----------------------------------------------------------------------------
// Accessors for Attribute's GUID
//-----------------------------------------------------------------------------

//! Returns statically defined GUID for the Attribute.
//! \return statically defined GUID.
template <typename T_ARGUMENT>
const Standard_GUID& asiData_FuncAttr<T_ARGUMENT>::GUID()
{
  static Standard_GUID AttrGUID("A8DC530F-16E8-4591-8D7E-1D2C19E8B7EB");
  return AttrGUID;
}

//! Accessor for GUID associated with this kind of OCAF Attribute.
//! \return GUID of the OCAF Attribute.
template <typename T_ARGUMENT>
const Standard_GUID& asiData_FuncAttr<T_ARGUMENT>::ID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------
// Attribute's kernel methods:
//-----------------------------------------------------------------------------

//! Creates new instance of the Attribute which is not initially populated
//! with any data structures.
//! \return new instance of the Attribute.
template <typename T_ARGUMENT>
Handle(TDF_Attribute) asiData_FuncAttr<T_ARGUMENT>::NewEmpty() const
{
  return new asiData_FuncAttr<T_ARGUMENT>();
}

//! Performs data transferring from the given OCAF Attribute to this one.
//! This method is mainly used by OCAF Undo/Redo kernel as a part of
//! backup functionality.
//! \param[in] MainAttr OCAF Attribute to copy data from.
template <typename T_ARGUMENT>
void asiData_FuncAttr<T_ARGUMENT>::Restore(const Handle(TDF_Attribute)& asiData_NotUsed(mainAttr))
{
  // Nothing is here
}

//! Supporting method for Copy/Paste functionality. Performs full copying of
//! the underlying data.
//! \param[in] Into       where to paste.
//! \param[in] RelocTable relocation table.
template <typename T_ARGUMENT>
void asiData_FuncAttr<T_ARGUMENT>::Paste(const Handle(TDF_Attribute)&       asiData_NotUsed(into),
                                         const Handle(TDF_RelocationTable)& asiData_NotUsed(relocTable)) const
{
  // Nothing is here
}

//-----------------------------------------------------------------------------
// Accessors for domain-specific data
//-----------------------------------------------------------------------------

//! Sets function object to store.
//! \param[in] func function object to store.
template <typename T_ARGUMENT>
void asiData_FuncAttr<T_ARGUMENT>::SetFunction(const Handle(asiAlgo_Function<T_ARGUMENT>)& func)
{
  m_func = func;
}

//! Returns the stored function object.
//! \return stored function object.
template <typename T_ARGUMENT>
const Handle(asiAlgo_Function<T_ARGUMENT>)& asiData_FuncAttr<T_ARGUMENT>::GetFunction() const
{
  return m_func;
}
