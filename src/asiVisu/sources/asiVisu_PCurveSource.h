//-----------------------------------------------------------------------------
// Created on: 02 December 2015
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

#ifndef asiVisu_PCurveSource_h
#define asiVisu_PCurveSource_h

// Visualization includes
#include <asiVisu_CurveSource.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! Source of polygonal data representing a curve on surface.
class asiVisu_PCurveSource : public asiVisu_CurveSource
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PCurveSource, vtkPolyDataAlgorithm);
  static asiVisu_PCurveSource* New();

// Kernel methods:
public:

  void
    SetEdgeOnFace(const TopoDS_Edge& edge,
                  const TopoDS_Face& face);

protected:

  asiVisu_PCurveSource();
  ~asiVisu_PCurveSource();

private:

  asiVisu_PCurveSource(const asiVisu_PCurveSource&);
  asiVisu_PCurveSource& operator=(const asiVisu_PCurveSource&);

private:

  TopoDS_Edge m_edge; //!< Edge.
  TopoDS_Face m_face; //!< Face.

};

#endif
