//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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

#ifndef asiAlgo_FindSameHosts_h
#define asiAlgo_FindSameHosts_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to find unique host surfaces.
class asiAlgo_FindSameHosts
{
public:

  typedef NCollection_Sequence<TColStd_PackedMapOfInteger> t_groups;

public:

  asiAlgo_EXPORT
    asiAlgo_FindSameHosts(const TopTools_IndexedMapOfShape& faces);

public:

  asiAlgo_EXPORT void
    operator()(t_groups& groups) const;

protected:

  bool findSame(const int                   reference,
                TColStd_PackedMapOfInteger& found) const;

protected:

  const TopTools_IndexedMapOfShape& m_faces;

private:

  asiAlgo_FindSameHosts(const asiAlgo_FindSameHosts& _h) : m_faces(_h.m_faces) {}
  void operator=(const asiAlgo_FindSameHosts&) {}

};

#endif
