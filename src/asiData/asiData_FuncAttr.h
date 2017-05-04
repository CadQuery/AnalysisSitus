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

  asiData_EXPORT
    asiData_FuncAttr();

  asiData_EXPORT static Handle(asiData_FuncAttr)
    Set(const TDF_Label& Label);

// GUID accessors:
public:

  asiData_EXPORT static const Standard_GUID&
    GUID();

  asiData_EXPORT virtual const Standard_GUID&
    ID() const;

// Attribute's kernel methods:
public:

  asiData_EXPORT virtual Handle(TDF_Attribute)
    NewEmpty() const;

  asiData_EXPORT virtual void
    Restore(const Handle(TDF_Attribute)& mainAttr);

  asiData_EXPORT virtual void
    Paste(const Handle(TDF_Attribute)&       into,
          const Handle(TDF_RelocationTable)& relocTable) const;

// Accessors for domain-specific data:
public:

  asiData_EXPORT void
    SetFunction(const Handle(asiAlgo_Function<T_ARGUMENT>)& func);

  asiData_EXPORT const Handle(asiAlgo_Function<T_ARGUMENT>)&
    GetFunction() const;

// Members:
private:

  //! Stored function.
  Handle(asiAlgo_Function<T_ARGUMENT>) m_func;

};

#endif
