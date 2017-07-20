//-----------------------------------------------------------------------------
// Created on: 09 March 2017
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

#ifndef asiAlgo_PlateOnEdges_h
#define asiAlgo_PlateOnEdges_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineSurface.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a plate surface on the given edge set.
class asiAlgo_PlateOnEdges : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PlateOnEdges, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_PlateOnEdges(const Handle(asiAlgo_AAG)& aag,
                         ActAPI_ProgressEntry       progress,
                         ActAPI_PlotterEntry        plotter);

public:

  asiAlgo_EXPORT bool
    Build(const TColStd_PackedMapOfInteger& edgeIndices,
          const unsigned int                continuity,
          Handle(Geom_BSplineSurface)&      support,
          TopoDS_Face&                      result);

protected:

  Handle(asiAlgo_AAG) m_aag; //!< AAG.

};

#endif
