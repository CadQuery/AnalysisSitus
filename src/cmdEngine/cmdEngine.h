//-----------------------------------------------------------------------------
// Created on: 19 September 2017
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

#ifndef cmdEngine_h
#define cmdEngine_h

#define cmdEngine_NotUsed(x) x

#ifdef cmdEngine_EXPORTS
  #define cmdEngine_EXPORT __declspec(dllexport)
#else
  #define cmdEngine_EXPORT __declspec(dllimport)
#endif

//-----------------------------------------------------------------------------

// asiTcl includes
#include <asiTcl_Interp.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiUI includes
#include <asiUI_ViewerPart.h>

//-----------------------------------------------------------------------------

//! B-Rep inspection commands.
class cmdEngine
{
public:

  cmdEngine_EXPORT static void
    Factory(const Handle(asiTcl_Interp)&      interp,
            const Handle(Standard_Transient)& data);

public:

  static Handle(asiEngine_Model) model;       //!< Data Model instance.
  static asiUI_ViewerPart*       pViewerPart; //!< Part viewer.

};

#endif
