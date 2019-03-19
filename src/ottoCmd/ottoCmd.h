//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoCmd_h
#define ottoCmd_h

#define ottoCmd_NotUsed(x) x

#ifdef ottoCmd_EXPORTS
  #define ottoCmd_EXPORT __declspec(dllexport)
#else
  #define ottoCmd_EXPORT __declspec(dllimport)
#endif

//-----------------------------------------------------------------------------

// asiTcl includes
#include <asiTcl_Interp.h>

// asiEngine includes
#include <asiEngine_Model.h>

// ottoExe includes
#include <ottoExe_CommonFacilities.h>

//-----------------------------------------------------------------------------

//! Custom commands for ottoExe.
class ottoCmd
{
public:

  ottoCmd_EXPORT static void
    Factory(const Handle(asiTcl_Interp)&      interp,
            const Handle(Standard_Transient)& data);

public:

  ottoCmd_EXPORT static void
    UpdateUI();

public:

  static Handle(ottoExe_CommonFacilities) cf; //!< Common facilities.

};

#endif
