//-----------------------------------------------------------------------------
// Created on: 14 March 2016
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

#ifndef asiAlgo_OBJ_h
#define asiAlgo_OBJ_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

// Active Data includes
#include <ActAux_Common.h>

//! Services to work with obj files.
namespace asiAlgo_OBJ
{
  //! Saves the passed shape to OBJ file extracting its associated triangulation.
  //! \param theShape    [in] B-Rep to access tessellation.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const TopoDS_Shape&            theShape,
          const TCollection_AsciiString& theFilename);

};

#endif
