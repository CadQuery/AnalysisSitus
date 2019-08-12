//-----------------------------------------------------------------------------
// Created on: 04 August 2019
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

#ifndef asiEngine_SmoothenCornersFunc_HeaderFile
#define asiEngine_SmoothenCornersFunc_HeaderFile

// asiEngine includes
#include <asiEngine.h>

// Active Data includes
#include <ActData_BaseTreeFunction.h>

//-----------------------------------------------------------------------------

//! Tree function for smooth connecting curves in a curve network.
class asiEngine_SmoothenCornersFunc : public ActData_BaseTreeFunction
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiEngine_SmoothenCornersFunc, ActData_BaseTreeFunction)

public:

  //! Instantiation routine.
  //! \return Tree Function instance.
  asiEngine_EXPORT static Handle(asiEngine_SmoothenCornersFunc)
    Instance();

  //! Static accessor for the GUID associated with the Tree Function.
  //! \return requested GUID.
  asiEngine_EXPORT static const char*
    GUID();

  //! Accessor for the GUID.
  //! \return GUID.
  asiEngine_EXPORT virtual const char*
    GetGUID() const;

public:

  //! Returns true if this Tree Function is HEAVY, false -- otherwise.
  //! \return always false.
  virtual bool IsHeavy() const
  {
    return false;
  }

private:

  //! Executes Tree Function on the given input and output arguments.
  //! \param[in]      inputs   input Parameters.
  //! \param[in, out] outputs  output Parameters.
  //! \param[in]      userData custom user data.
  //! \return execution status.
  virtual int
    execute(const Handle(ActAPI_HParameterList)& inputs,
            const Handle(ActAPI_HParameterList)& outputs,
            const Handle(Standard_Transient)&    userData) const;

  //! \return expected input signature.
  virtual ActAPI_ParameterTypeStream
    inputSignature() const;

  //! \return expected output signature.
  virtual ActAPI_ParameterTypeStream
    outputSignature() const;

private:

  asiEngine_SmoothenCornersFunc() {} //!< Ctor.

private:

  mutable bool m_bPlotterEnabled; //!< Indicates whether plotter is enabled.

};

#endif
