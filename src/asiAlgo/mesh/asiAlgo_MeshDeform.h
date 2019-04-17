//-----------------------------------------------------------------------------
// Created on: 17 April 2019
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

#ifndef asiAlgo_MeshDeform_h
#define asiAlgo_MeshDeform_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

class asiAlgo_MeshDeform : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshDeform, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_MeshDeform(const double         corners[6],
                       const double         ratioX,
                       const double         ratioY,
                       const double         ratioZ,
                       ActAPI_ProgressEntry progress = NULL,
                       ActAPI_PlotterEntry  plotter  = NULL);

public:

  asiAlgo_EXPORT bool
    Perform(Handle(Poly_Triangulation)& mesh,
            const TopLoc_Location&      loc);

public:

  //! \return input mesh.
  const Handle(Poly_Triangulation)& GetInput() const
  {
    return m_input;
  }

  //! \return resulting (deformed) mesh.
  const Handle(Poly_Triangulation)& GetResult() const
  {
    return m_result;
  }

private:

  Handle(Poly_Triangulation) m_input;  //! Input triangulation.
  Handle(Poly_Triangulation) m_result; //! Triangulation after deformation.
  double m_corners[6];                      //! AABB box of mesh
  /*const*/ TopLoc_Location m_loc;              //! location of mesh
  double m_ratioX;                          //! ratio for maximum x deformation
  double m_ratioY;                          //! ratio for maximum y deformation
  double m_ratioZ;                          //! ratio for maximum z deformation

  double Lx, Ly, Lz, Ox, Oy, Oz;
};

#endif