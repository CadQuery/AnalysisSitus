//-----------------------------------------------------------------------------
// Created on: 02 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_FuncAttr_h
#define asiData_FuncAttr_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_Function.h>

// OCCT includes
#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

//! OCAF Attribute representing Function object.
template <typename T_ARGUMENT>
class asiData_FuncAttr : public TDF_Attribute
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_FuncAttr<T_ARGUMENT>, TDF_Attribute)

// Construction & settling-down routines:
public:

  //! Default constructor.
  asiData_FuncAttr() : TDF_Attribute() {}

  //! Settles down new Attribute to the given OCAF Label.
  //! \param[in] Label TDF Label to settle down the new Attribute to.
  //! \return newly created Attribute settled down onto the target Label.
  static Handle(asiData_FuncAttr) Set(const TDF_Label& Label)
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

// GUID accessors:
public:

  //! Returns statically defined GUID for the Attribute.
  //! \return statically defined GUID.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID AttrGUID("A8DC530F-16E8-4591-8D7E-1D2C19E8B7EB");
    return AttrGUID;
  }

  //! Accessor for GUID associated with this kind of OCAF Attribute.
  //! \return GUID of the OCAF Attribute.
  virtual const Standard_GUID& ID() const
  {
    return GUID();
  }

// Attribute's kernel methods:
public:

  //! Creates new instance of the Attribute which is not initially populated
  //! with any data structures.
  //! \return new instance of the Attribute.
  virtual Handle(TDF_Attribute) NewEmpty() const
  {
    return new asiData_FuncAttr<T_ARGUMENT>();
  }

  //! Performs data transferring from the given OCAF Attribute to this one.
  //! This method is mainly used by OCAF Undo/Redo kernel as a part of
  //! backup functionality.
  //! \param[in] MainAttr OCAF Attribute to copy data from.
  virtual void Restore(const Handle(TDF_Attribute)& mainAttr)
  {
    // Nothing is here
  }

  //! Supporting method for Copy/Paste functionality. Performs full copying of
  //! the underlying data.
  //! \param[in] Into       where to paste.
  //! \param[in] RelocTable relocation table.
  virtual void Paste(const Handle(TDF_Attribute)&       into,
                     const Handle(TDF_RelocationTable)& relocTable) const
  {
    // Nothing is here
  }

// Accessors for domain-specific data:
public:

  //! Sets function object to store.
  //! \param[in] func function object to store.
  void SetFunction(const Handle(asiAlgo_Function<T_ARGUMENT>)& func)
  {
    m_func = func;
  }

  //! Returns the stored function object.
  //! \return stored function object.
  const Handle(asiAlgo_Function<T_ARGUMENT>)& GetFunction() const
  {
    return m_func;
  }

// Members:
private:

  //! Stored function.
  Handle(asiAlgo_Function<T_ARGUMENT>) m_func;

};

#endif
