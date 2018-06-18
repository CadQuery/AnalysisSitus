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

// Include self.
#include <BRepOffset_CONSAdaptor.hxx>

#include <BRepOffset_HCONSAdaptor.hxx>

Handle(Adaptor3d_HCurve)
  BRepOffset_CONSAdaptor::Trim(const Standard_Real First,
                               const Standard_Real Last,
                               const Standard_Real Tol) const
{
  Handle(BRepOffset_HCONSAdaptor) HCS = new BRepOffset_HCONSAdaptor();
  HCS->ChangeCurve().Load(myCurve->Trim(First,Last,Tol), mySurface);
  return HCS;
}
