//-----------------------------------------------------------------------------
// Created on: 21 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
