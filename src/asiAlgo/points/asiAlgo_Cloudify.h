//-----------------------------------------------------------------------------
// Created on: 02 June 2016
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

#ifndef asiAlgo_Cloudify_h
#define asiAlgo_Cloudify_h

// A-Situs includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <BRepAdaptor_Surface.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a point cloud from a CAD model.
class asiAlgo_Cloudify : public ActAPI_IAlgorithm
{
public:

  asiAlgo_EXPORT
    asiAlgo_Cloudify(const double         uv_step,
                     ActAPI_ProgressEntry progress,
                     ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Sample_Faces(const TopoDS_Shape&                model,
                 Handle(asiAlgo_BaseCloud<double>)& point_cloud);

  asiAlgo_EXPORT bool
    Sample_Facets(const TopoDS_Shape&                model,
                  Handle(asiAlgo_BaseCloud<double>)& point_cloud);

protected:

  double chooseParametricStep(const BRepAdaptor_Surface& bas,
                              const bool                 isU,
                              const double               uMin,
                              const double               uMax,
                              const double               vMin,
                              const double               vMax) const;

protected:

  double m_fPrecision; //!< Precision of internal classification.
  double m_fLinStep;   //!< Linear step for point sampling.

};

#endif
