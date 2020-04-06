//-----------------------------------------------------------------------------
// Created on: 03 April 2020
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

#ifndef asiVisu_DeviationDataProvider_h
#define asiVisu_DeviationDataProvider_h

// asiVisu includes
#include <asiVisu_MeshEScalarDataProvider.h>

// asiData includes
#include <asiData_ThicknessNode.h>

//-----------------------------------------------------------------------------

//! Data provider for thickness fields.
class asiVisu_ThicknessDataProvider : public asiVisu_MeshEScalarDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ThicknessDataProvider, asiVisu_MeshEScalarDataProvider)

public:

  //! Ctor.
  //! \param[in] N Thickness Node.
  asiVisu_EXPORT
    asiVisu_ThicknessDataProvider(const Handle(asiData_ThicknessNode)& N);

public:

  //! \return ids of the mesh elements having associated scalars.
  asiVisu_EXPORT virtual Handle(HIntArray)
    GetElementIDs() const;

  //! \return array of elemental scalars.
  asiVisu_EXPORT virtual Handle(HRealArray)
    GetElementScalars() const;

  //! \return min scalar bound.
  asiVisu_EXPORT virtual double
    GetMinScalar() const;

  //! \return max scalar bound.
  asiVisu_EXPORT virtual double
    GetMaxScalar() const;

protected:

  //! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process.
  //! \return source Parameters.
  asiVisu_EXPORT virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
