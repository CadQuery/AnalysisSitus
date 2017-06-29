//-----------------------------------------------------------------------------
// Created on: 03 March 2017
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

#ifndef asiAlgo_PlateOnPoints_h
#define asiAlgo_PlateOnPoints_h

// A-Situs includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineSurface.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a plate surface on the given point set.
class asiAlgo_PlateOnPoints : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PlateOnPoints, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_PlateOnPoints(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Build(const std::vector<gp_Pnt>&   points,
          Handle(Geom_BSplineSurface)& result);

};

#endif
