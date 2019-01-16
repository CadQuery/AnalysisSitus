//-----------------------------------------------------------------------------
// Created on: 11 January 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiAlgo_MeshInterPlane_HeaderFile
#define asiAlgo_MeshInterPlane_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_Plane.hxx>

//-----------------------------------------------------------------------------

//! Utility to intersect mesh with a plane. This algorithm is based on VTK
//! cutter which returns an unordered set of points. As the second (and
//! optional) stage, we sort the obtained points with concave hull utility,
//! thus making the section ready for subsequent curve approximation.
class asiAlgo_MeshInterPlane : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshInterPlane, ActAPI_IAlgorithm)

public:

  //! Constructs intersection tool.
  //! \param[in] mesh     triangulation to intersect.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshInterPlane(const Handle(Poly_Triangulation)& mesh,
                           ActAPI_ProgressEntry              progress,
                           ActAPI_PlotterEntry               plotter);

public:

  //! Performs intersection.
  //! \param[in] plane  plane to intersect the mesh with.
  //! \param[in] doSort indicates whether to sort the obtained points.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const Handle(Geom_Plane)& plane,
            const bool                doSort);

protected:

  Handle(Poly_Triangulation) m_mesh; //!< Mesh to check.

};

#endif
