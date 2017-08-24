//-----------------------------------------------------------------------------
// Created on: 24 August 2017
// Created by: Sergey SLYADNEV
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

#ifndef cmdInspector_h
#define cmdInspector_h

#define cmdInspector_NotUsed(x) x

#ifdef cmdInspector_EXPORTS
  #define cmdInspector_EXPORT __declspec(dllexport)
#else
  #define cmdInspector_EXPORT __declspec(dllimport)
#endif

//-----------------------------------------------------------------------------

// asiTcl includes
#include <asiTcl_Interp.h>

// asiEngine includes
#include <asiEngine_Model.h>

// exe includes
#include <exe_CommonFacilities.h>

//-----------------------------------------------------------------------------

//! B-Rep inspection commands.
class cmdInspector
{
public:

  cmdInspector_EXPORT static void
    Factory(const Handle(asiTcl_Interp)&      interp,
            const Handle(Standard_Transient)& data);

public:

  cmdInspector_EXPORT static void
    UpdateUI();

public:

  static Handle(exe_CommonFacilities) cf; //!< Common facilities.

};

#endif
