//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Membership_h
#define asiAlgo_Membership_h

// Analysis Situs includes
#include <asiAlgo.h>

//! Set membership relations.
enum asiAlgo_Membership
{
//---------------------------------------------------------------------------//
  Membership_Unknown   = 0x01,
  Membership_In        = 0x02,
  Membership_On        = 0x04,
  Membership_Out       = 0x08,
  Membership_Composite = 0x10,
//---------------------------------------------------------------------------//
// Handy items to use in conditional statements
//---------------------------------------------------------------------------//
  Membership_InOn      = Membership_In | Membership_On,
  Membership_InOnComp  = Membership_In | Membership_On | Membership_Composite,
  Membership_InOnOut   = Membership_In | Membership_On | Membership_Out,
  Membership_InOut     = Membership_In | Membership_Out
};

#endif
