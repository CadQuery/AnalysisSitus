//-----------------------------------------------------------------------------
// Created on: 01 June 2020
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

#ifndef asiEngine_Isomorphism_h
#define asiEngine_Isomorphism_h

// asiEngine includes
#include <asiEngine_Model.h>

//-----------------------------------------------------------------------------

//! Data Model API for finding isomorphisms of AAG.
class asiEngine_Isomorphism
{
public:

  //! Settings for isomorphism searching.
  enum Flags
  {
    ExcludeConvexOnly = 0x001,
    ExcludeBase       = 0x002,
    Verbose           = 0x004
  };

public:

  //! Ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_Isomorphism(const Handle(asiEngine_Model)& model,
                        ActAPI_ProgressEntry           progress = nullptr,
                        ActAPI_PlotterEntry            plotter  = nullptr)
  //
  : m_model(model), m_progress(progress), m_plotter(plotter) {}

public:

  //! Finds all isomorphisms.
  //! \param[in]  featureName  name of the data object representing the
  //!                          feature to match.
  //! \param[out] featureFaces found feature faces.
  //! \param[in]  flags        optional flags to affect how isomorphisms
  //!                          are to be found.
  //! \return true in case of success, false -- otherwise.
  asiEngine_EXPORT bool
    Compute(const TCollection_AsciiString& featureName,
            TColStd_PackedMapOfInteger&    featureFaces,
            const int                      flags = 0);

protected:

  Handle(asiEngine_Model) m_model; //!< Data Model instance.

  /* Diagnostic tools */
  mutable ActAPI_ProgressEntry m_progress; //!< Progress notifier.
  mutable ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
