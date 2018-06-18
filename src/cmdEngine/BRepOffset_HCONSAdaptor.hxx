// Created on: 2018-06-18
// Created by: Sergey SLYADNEV
// Copyright (c) 1999-2018 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef BRepOffset_HCONSAdaptor_HeaderFile
#define BRepOffset_HCONSAdaptor_HeaderFile

#include <BRepOffset_CONSAdaptor.hxx>

class BRepOffset_HCONSAdaptor : public Adaptor3d_HCurve
{
public:

  BRepOffset_HCONSAdaptor() : Adaptor3d_HCurve() {}
  
  BRepOffset_HCONSAdaptor(const BRepOffset_CONSAdaptor& C)
  {
    myCurve = C;
  }

public:
  
  void Set (const BRepOffset_CONSAdaptor& C)
  {
    myCurve = C;
  }
  
  const Adaptor3d_Curve& Curve() const Standard_OVERRIDE
  {
    return myCurve;
  }
  
  Adaptor3d_Curve& GetCurve() Standard_OVERRIDE
  {
    return myCurve;
  }
  
  //! Returns the curve used to create the GenHCurve.
  BRepOffset_CONSAdaptor& ChangeCurve()
  {
    return myCurve;
  }

  DEFINE_STANDARD_RTTI_INLINE(BRepOffset_HCONSAdaptor, Adaptor3d_HCurve)

protected:

  BRepOffset_CONSAdaptor myCurve;

};

#endif // BRepOffset_HCONSAdaptor_HeaderFile
