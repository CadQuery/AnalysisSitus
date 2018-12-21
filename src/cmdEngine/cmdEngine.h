//-----------------------------------------------------------------------------
// Created on: 19 September 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include <asiUI_CommonFacilities.h>

//-----------------------------------------------------------------------------

//! B-Rep inspection commands.
class cmdEngine
{
public:

  cmdEngine_EXPORT static void
    Factory(const Handle(asiTcl_Interp)&      interp,
            const Handle(Standard_Transient)& data);

public:

  cmdEngine_EXPORT static void
    Commands_Data(const Handle(asiTcl_Interp)&      interp,
                  const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Editing(const Handle(asiTcl_Interp)&      interp,
                     const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Inspection(const Handle(asiTcl_Interp)&      interp,
                        const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Interaction(const Handle(asiTcl_Interp)&      interp,
                         const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Interop(const Handle(asiTcl_Interp)&      interp,
                     const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Modeling(const Handle(asiTcl_Interp)&      interp,
                      const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Naming(const Handle(asiTcl_Interp)&      interp,
                    const Handle(Standard_Transient)& data);

  cmdEngine_EXPORT static void
    Commands_Viewer(const Handle(asiTcl_Interp)&      interp,
                    const Handle(Standard_Transient)& data);

public:

  cmdEngine_EXPORT static void
    ClearViewers(const bool repaint = true);

public:

  static Handle(asiEngine_Model)        model; //!< Data Model instance.
  static Handle(asiUI_CommonFacilities) cf;    //!< UI common facilities.

};

#endif
