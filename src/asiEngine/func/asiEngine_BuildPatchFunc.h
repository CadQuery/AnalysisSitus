//-----------------------------------------------------------------------------
// Created on: 09 July 2019
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

#ifndef asiEngine_BuildPatchFunc_HeaderFile
#define asiEngine_BuildPatchFunc_HeaderFile

// asiEngine includes
#include <asiEngine.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActData_BaseTreeFunction.h>

// OpenCascade includes
#include <Geom_BSplineSurface.hxx>

// Forward declarations.
class asiData_RePatchNode;
class asiEngine_Model;

//! Tree function for automatic (re-)constructing surface patches in
//! reverse engineering scenarios.
//!
//! Inputs are the Parameters from:
//! - Min number of knots from the Patch Node.
//! - Coedge Node 1 under the Patch Node.
//! - Coedge Node 2 under the Patch Node.
//! - ...
//! - Coedge Node K under the Patch Node.
//! - Edge Node for the Coedge 1.
//! - Edge Node for the Coedge 2.
//! - ...
//! - Edge Node for the Coedge K.
//!
//! Outputs:
//! - Surface Parameter of the Patch Node.
//!
//! The inputs enumerated above are the sensistives of this Tree Function.
class asiEngine_BuildPatchFunc : public ActData_BaseTreeFunction
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiEngine_BuildPatchFunc, ActData_BaseTreeFunction)

public:

  //! Instantiation routine.
  //! \return Tree Function instance.
  asiEngine_EXPORT static Handle(asiEngine_BuildPatchFunc)
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

  //! This method verifies that the passed list of input Parameters satisfies
  //! the expected (variable) signature of the Tree Function.
  //! \param[in] inputs input Parameters.
  //! \return false if the input arguments are of an unexpected format.
  virtual bool
    validateInput(const Handle(ActAPI_HParameterList)& inputs) const;

  //! This method returns empty list because this Tree Function has a variable
  //! input signature.
  //! \return expected input signature.
  virtual ActAPI_ParameterTypeStream
    inputSignature() const;

  //! \return expected output signature.
  virtual ActAPI_ParameterTypeStream
    outputSignature() const;

private:

  //! Extracts mesh nodes for approximation.
  //! \param[in]  model Data Model instance.
  //! \param[in]  patch Patch Node being processed.
  //! \param[out] pts   extracted points to approximate.
  //! \return true in case of success, false -- otherwise.
  bool extractMeshNodes(const Handle(asiEngine_Model)&     model,
                        const Handle(asiData_RePatchNode)& patch,
                        Handle(asiAlgo_BaseCloud<double>)& pts) const;

  //! Approximates mesh nodes with a B-surface.
  //! \param[in]  initSurf   initial surface.
  //! \param[in]  pts        points to approximate.
  //! \param[in]  lambda     value of the fairing coefficient.
  //! \param[out] resultSurf approximated surface.
  //! \return true in case of success, false -- otherwise.
  bool approxMeshNodes(const Handle(Geom_BSplineSurface)&       initSurf,
                       const Handle(asiAlgo_BaseCloud<double>)& pts,
                       const double                             lambda,
                       Handle(Geom_BSplineSurface)&             resultSurf) const;

private:

  asiEngine_BuildPatchFunc() {} //!< Ctor.

};

#endif
