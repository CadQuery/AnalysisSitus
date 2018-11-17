//-----------------------------------------------------------------------------
// Created on: 05 October 2018
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

#ifndef asiAlgo_MeshOffset_HeaderFile
#define asiAlgo_MeshOffset_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data (API) includes
#include <ActAPI_IAlgorithm.h>

// Active Data includes
#include <ActData_Mesh.h>

//-----------------------------------------------------------------------------

//! Utility to compute topology-preserving mesh offset.
class asiAlgo_MeshOffset : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshOffset, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] mesh     mesh in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshOffset(const Handle(ActData_Mesh)& mesh,
                       ActAPI_ProgressEntry        progress,
                       ActAPI_PlotterEntry         plotter);

public:

  //! Performs offset on the given signed distance.
  //! \param[in] dist distance value.
  asiAlgo_EXPORT void
    Perform(const double dist);

public:

  //! \return result mesh.
  const Handle(ActData_Mesh)& GetResult() const
  {
    return m_result;
  }

protected:

  //! Mesh in question.
  Handle(ActData_Mesh) m_input;

  //! Result mesh.
  Handle(ActData_Mesh) m_result;

};

#endif
