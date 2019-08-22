//-----------------------------------------------------------------------------
// Created on: 22 August 2019
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

#ifndef asiVisu_DeviationDataProvider_h
#define asiVisu_DeviationDataProvider_h

// asiVisu includes
#include <asiVisu_MeshNScalarDataProvider.h>

// asiData includes
#include <asiData_DeviationNode.h>

//-----------------------------------------------------------------------------

//! Data provider for deviation fields.
class asiVisu_DeviationDataProvider : public asiVisu_MeshNScalarDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_DeviationDataProvider, asiVisu_MeshNScalarDataProvider)

public:

  //! Ctor accepting Triangulation Parameter.
  //! \param[in] P parameter holding triangulation to visualize.
  asiVisu_EXPORT
    asiVisu_DeviationDataProvider(const Handle(asiData_DeviationNode)& P);

public:

  //! \return ids of the mesh nodes having associated scalars.
  virtual Handle(HIntArray)
    GetNodeIDs() const;

  //! \return array of scalars.
  virtual Handle(HRealArray)
    GetNodeScalars() const;

};

#endif
