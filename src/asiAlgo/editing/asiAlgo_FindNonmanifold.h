//-----------------------------------------------------------------------------
// Created on: 18 May 2016
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

#ifndef asiAlgo_FindNonmanifold_h
#define asiAlgo_FindNonmanifold_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//-----------------------------------------------------------------------------

//! Utilities for detection of non-manifold geometry in CAD.
namespace asiAlgo_FindNonmanifold
{
  //! Finds non-manifold edges.
  //! \param shape   [in]     input CAD model.
  //! \param nmEdges [out]    non-manifold edges.
  //! \param Journal [in/out] journal.
  asiAlgo_EXPORT
    void FindEdges(const TopoDS_Shape&         shape,
                   TopTools_IndexedMapOfShape& nmEdges,
                   ActAPI_ProgressEntry        Journal);
};

#endif
