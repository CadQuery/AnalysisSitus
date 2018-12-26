//-----------------------------------------------------------------------------
// Created on: 09 March 2017
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

  asiAlgo_EXPORT
    asiAlgo_PlateOnEdges(const TopoDS_Shape&  shape,
                         ActAPI_ProgressEntry progress,
                         ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Build(const TColStd_PackedMapOfInteger& edgeIndices,
          const unsigned int                continuity,
          Handle(Geom_BSplineSurface)&      support,
          TopoDS_Face&                      result);

  asiAlgo_EXPORT bool
    BuildSurf(const TColStd_PackedMapOfInteger& edgeIndices,
              const unsigned int                continuity,
              Handle(Geom_BSplineSurface)&      support);

  asiAlgo_EXPORT bool
    BuildSurf(const std::vector<TopoDS_Edge>& edges,
              const unsigned int              continuity,
              Handle(Geom_BSplineSurface)&    support);

protected:

  TopoDS_Shape        m_shape; //!< Working shape.
  Handle(asiAlgo_AAG) m_aag;   //!< AAG.

};

#endif
