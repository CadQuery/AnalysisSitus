//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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
