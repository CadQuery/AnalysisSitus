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

#ifndef BRepOffset_CONSAdaptor_HeaderFile
#define BRepOffset_CONSAdaptor_HeaderFile

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>

class BRepOffset_CONSAdaptor : public Adaptor3d_CurveOnSurface
{
public:

  BRepOffset_CONSAdaptor() : Adaptor3d_CurveOnSurface() {}
  
  BRepOffset_CONSAdaptor(const Handle(Adaptor3d_HSurface)& S) : Adaptor3d_CurveOnSurface(S) {}
  
  //! Creates a CurveOnSurface from the 2d curve <C> and
  //! the surface <S>.
  BRepOffset_CONSAdaptor(const Handle(Adaptor2d_HCurve2d)& C,
                         const Handle(Adaptor3d_HSurface)& S)
  : Adaptor3d_CurveOnSurface(C, S)
  {
    myCurve = C;
    mySurface = S;

    myUmin = mySurface->FirstUParameter();
    myUmax = mySurface->LastUParameter();
    myVmin = mySurface->FirstVParameter();
    myVmax = mySurface->LastVParameter();
  }

  void Load(const Handle(Adaptor2d_HCurve2d)& C, const Handle(Adaptor3d_HSurface)& S)
  {
    Adaptor3d_CurveOnSurface::Load(C, S);
    myCurve = C;
    mySurface = S;

    myUmin = mySurface->FirstUParameter();
    myUmax = mySurface->LastUParameter();
    myVmin = mySurface->FirstVParameter();
    myVmax = mySurface->LastVParameter();
  }

  Standard_EXPORT Handle(Adaptor3d_HCurve)
    Trim (const Standard_Real First,
          const Standard_Real Last,
          const Standard_Real Tol) const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve.
  void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE
  {
    gp_Pnt2d Puv;
  
    myCurve->D0(U,Puv);
    mySurface->D0(Puv.X() > myUmax ? myUmax : Puv.X(),
                  Puv.Y() > myVmax ? myVmax : Puv.Y(),
                  P);
  }

  //! Computes the point of parameter U on the curve.
  gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE
  {
    gp_Pnt2d Puv;
  
    myCurve->D0(U,Puv);
    return mySurface->Value( Puv.X() > myUmax ? myUmax : Puv.X(),
                             Puv.Y() > myVmax ? myVmax : Puv.Y() );
  }

protected:

  Handle(Adaptor3d_HSurface) mySurface;
  Handle(Adaptor2d_HCurve2d) myCurve;

  double myUmin, myUmax, myVmin, myVmax;

};

#endif // BRepOffset_CONSAdaptor_HeaderFile
