//-----------------------------------------------------------------------------
// Created on: 09 December 2016
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

#ifndef asiVisu_Common_h
#define asiVisu_Common_h

// Visualization includes
#include <asiVisu.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// OCCT includes
#include <NCollection_SparseArray.hxx>

//-----------------------------------------------------------------------------

//! Elementary data chunk representing vector value.
struct asiVisu_VectorTuple
{
  double F[3];

  asiVisu_VectorTuple()
  {
    F[0] = 0.0; F[1] = 0.0; F[2] = 0.0;
  }

  asiVisu_VectorTuple(const double _F1,
                      const double _F2,
                      const double _F3)
  {
    F[0] = _F1; F[1] = _F2; F[2] = _F3;
  }
};

//! Collection of vector values associated with sparse integer IDs.
typedef NCollection_SparseArray<asiVisu_VectorTuple> asiVisu_VectorMap;
typedef NCollection_Shared<asiVisu_VectorMap>        asiVisu_HVectorMap;

//-----------------------------------------------------------------------------

#endif
