//-----------------------------------------------------------------------------
// Created on: 21 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_AAGAttr_h
#define asiData_AAGAttr_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// OCCT includes
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

DEFINE_STANDARD_HANDLE(asiData_AAGAttr, TDF_Attribute)

//! OCAF Attribute representing Attributed Adjacency Graph (AAG).
class asiData_AAGAttr : public TDF_Attribute
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_AAGAttr, TDF_Attribute)

// Construction & settling-down routines:
public:

  asiData_EXPORT
    asiData_AAGAttr();

  asiData_EXPORT static Handle(asiData_AAGAttr)
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
    SetAAG(const Handle(asiAlgo_AAG)& aag);

  asiData_EXPORT const Handle(asiAlgo_AAG)&
    GetAAG() const;

// Members:
private:

  //! Stored AAG.
  Handle(asiAlgo_AAG) m_aag;

};

#endif
