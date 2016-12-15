//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FeatureAttr_h
#define asiAlgo_FeatureAttr_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_FeatureAttr, Standard_Transient)

//! Base class for all feature attributes.
class asiAlgo_FeatureAttr : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttr, Standard_Transient)

public:

  virtual ~asiAlgo_FeatureAttr() {}

public:

  virtual const Standard_GUID&
    GetGUID() const = 0;

public:

  virtual void Dump(Standard_OStream&) const {}

public:

  //! Hasher for sets.
  struct t_hasher
  {
    static int HashCode(const Handle(asiAlgo_FeatureAttr)& attr, const int upper)
    {
      return Standard_GUID::HashCode(attr->GetGUID(), upper);
    }

    static bool IsEqual(const Handle(asiAlgo_FeatureAttr)& attr, const Handle(asiAlgo_FeatureAttr)& other)
    {
      return Standard_GUID::IsEqual( attr->GetGUID(), other->GetGUID() );
    }
  };

};

#endif
