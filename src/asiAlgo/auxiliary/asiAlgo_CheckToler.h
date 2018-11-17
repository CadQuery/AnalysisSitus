//-----------------------------------------------------------------------------
// Created on: 20 April 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiAlgo_CheckToler_HeaderFile
#define asiAlgo_CheckToler_HeaderFile

// asiAlgo includes
#include <asiAlgo_TopoGraph.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to check tolerances in a B-Rep model.
class asiAlgo_CheckToler : public ActAPI_IAlgorithm
{
public:

  //! ctor.
  //! \param[in] shape    B-Rep shape to check.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_CheckToler(const TopoDS_Shape&  shape,
                       ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

public:

  //! Performs tolerance analysis for the given tolerance ranges.
  //! \param[in]  tolerRanges   tolerance ranges.
  //! \param[out] tolerShapes   sub-shapes falling into the tolerance ranges.
  //! \param[out] outOfRangeMin sub-shape with a tolerance less than min passed.
  //! \param[out] outOfRangeMax sub-shape with a tolerance greater than max passed.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    PerformRanges(const std::vector<double>& tolerRanges,
                  std::vector<TopoDS_Shape>& tolerShapes,
                  TopoDS_Shape&              outOfRangeMin,
                  TopoDS_Shape&              outOfRangeMax) const;

public:

  //! \return shape to analyze.
  const TopoDS_Shape& GetShape() const
  {
    return m_shape;
  }

protected:

  void analyzeSubShape(const TopoDS_Shape&        subshape,
                       const std::vector<double>& tolerRanges,
                       std::vector<TopoDS_Shape>& tolerShapes,
                       TopoDS_Shape&              outOfRangeMin,
                       TopoDS_Shape&              outOfRangeMax) const;

protected:

  //! Shape to analyze.
  TopoDS_Shape m_shape;

};

#endif
