//-----------------------------------------------------------------------------
// Created on: 01 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_RecognizeBlends_h
#define asiAlgo_RecognizeBlends_h

// asiAlgo includes
#include <asiAlgo_Recognizer.h>

//! Utility to recognize blends.
class asiAlgo_RecognizeBlends : public asiAlgo_Recognizer
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_RecognizeBlends, asiAlgo_Recognizer)

public:

  //! Ctor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] progress  progress notifier.
  //! \param[in] plotter   imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_RecognizeBlends(const TopoDS_Shape&  masterCAD,
                            ActAPI_ProgressEntry progress = NULL,
                            ActAPI_PlotterEntry  plotter  = NULL);

  //! Ctor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] aag       AAG (will be created from CAD if NULL is passed).
  //! \param[in] progress  progress notifier.
  //! \param[in] plotter   imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_RecognizeBlends(const TopoDS_Shape&        masterCAD,
                            const Handle(asiAlgo_AAG)& aag,
                            ActAPI_ProgressEntry       progress = NULL,
                            ActAPI_PlotterEntry        plotter  = NULL);

  //! Ctor.
  //! \param[in] aag      AAG (should not be NULL here).
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_RecognizeBlends(const Handle(asiAlgo_AAG)& aag,
                            ActAPI_ProgressEntry       progress = NULL,
                            ActAPI_PlotterEntry        plotter  = NULL);

public:

  //! Performs recognition of fillets.
  //! \param[in] faceId 1-based ID of the seed face.
  //! \param[in] radius radius of interest.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT virtual bool
    Perform(const int    faceId,
            const double radius = 1e100);

private:

  //! Reaction to formulation changing.
  //! \param[in] formulation activated formulation.
  virtual void onFormulation(const asiAlgo_FeatureFormulation formulation)
  {
    switch ( formulation )
    {
      case FeatureFormulation_Full:
      case FeatureFormulation_SupportFace:
        Standard_ProgramError::Raise("Unsupported formulation");
      //
      case FeatureFormulation_GuessFace:
      default: break;
    }
  }

};

#endif
