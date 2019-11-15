//-----------------------------------------------------------------------------
// Created on: 15 February 2016
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
//    * Neither the name of the copyright holder(s) nor the
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

#ifndef asiAlgo_MeshGen_h
#define asiAlgo_MeshGen_h

// A-Situs includes
#include <asiAlgo.h>

// Mesh includes
#include <asiAlgo_MeshInfo.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//-----------------------------------------------------------------------------

//! Services related to mesh generation.
namespace asiAlgo_MeshGen
{
  //! Automatically selects a linear deflection for meshing as a ratio of
  //! a diagonal of a shape's bounding box.
  //! \param[in] shape shape to select the deflection value for.
  //! \return autocomputed linear deflection.
  asiAlgo_EXPORT double
    AutoSelectLinearDeflection(const TopoDS_Shape& shape);

  //! Automatically selects the angular defection value for the passed shape.
  //! \param[in] shape shape in question.
  //! \return selected angular deflection.
  asiAlgo_EXPORT double
    AutoSelectAngularDeflection(const TopoDS_Shape& shape);

  //! Generates surface mesh by native OCCT tools. The generated facets will
  //! be distributed by the corresponding CAD faces. Use BRep_Tool::Triangulation()
  //! function to access them.
  //!
  //! \param[in,out] shape                 shape to tessellate.
  //! \param[in]     linearDeflection      linear (chord) deflection.
  //! \param[in]     angularDeflection_deg angular deflection (in degrees).
  //! \param[out]    info                  output mesh summary.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    DoNative(const TopoDS_Shape& shape,
             const double        linearDeflection,
             const double        angularDeflection_deg,
             asiAlgo_MeshInfo&   info);

  //! Generates surface mesh by native OCCT tools. The generated facets will
  //! be distributed by the corresponding CAD faces. Use BRep_Tool::Triangulation()
  //! function to access them.
  //!
  //! This function automatically selects the linear and the angular deflection
  //! parameters.
  //!
  //! \param[in,out] shape shape to tessellate.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    DoNative(const TopoDS_Shape& shape);

};

#endif
