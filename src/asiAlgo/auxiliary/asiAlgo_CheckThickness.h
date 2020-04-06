//-----------------------------------------------------------------------------
// Created on: 02 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAlgo_CheckThickness_h
#define asiAlgo_CheckThickness_h

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>
#include <asiAlgo_Mesh.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to check thickness of a CAD part.
class asiAlgo_CheckThickness : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_CheckThickness, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] shape    B-rep shape of a CAD part to analyze.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_CheckThickness(const TopoDS_Shape&  shape,
                           ActAPI_ProgressEntry progress = nullptr,
                           ActAPI_PlotterEntry  plotter  = nullptr);

  //! Ctor.
  //! \param[in] tris     facets of a CAD part to analyze.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_CheckThickness(const Handle(Poly_Triangulation)& tris,
                           ActAPI_ProgressEntry              progress = nullptr,
                           ActAPI_PlotterEntry               plotter  = nullptr);

public:

  //! Performs ray casting method of thickness analysis.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform_RayMethod();

  //! Performs sphere-based method of thickness anslysis.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform_SphereMethod();

public:

  //! Sets custom direction mode.
  //! \param[in] on true/false.
  void SetIsCustomDir(const bool on)
  {
    m_bIsCustomDir = on;
  }

  //! Sets custom direction of analysis.
  //! \param[in] dir direction to set.
  void SetCustomDir(const gp_Dir& dir)
  {
    m_customDir = dir;
  }

  //! \return result of thickness check which is a faceted representation
  //!         of the CAD part with associated distance field. The scalar
  //!         values representing the distance field are bounded to the
  //!         mesh nodes.
  const asiAlgo_Mesh& GetThicknessField() const
  {
    return m_resField;
  }

  //! \return the computed min thickness value.
  double GetMinThickness() const
  {
    return m_fMinThick;
  }

  //! \return the computed max thickness value.
  double GetMaxThickness() const
  {
    return m_fMaxThick;
  }

protected:

  Handle(asiAlgo_BVHFacets) m_bvh;          //!< BVH representation of a CAD part.
  bool                      m_bIsCustomDir; //!< Whether to use custom direction.
  gp_Dir                    m_customDir;    //!< Custom direction.
  asiAlgo_Mesh              m_resField;     //!< Mesh with a scalar field.
  double                    m_fMinThick;    //!< Min thickness.
  double                    m_fMaxThick;    //!< Max thickness.

};

#endif
