//-----------------------------------------------------------------------------
// Created on: 21 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
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

#ifndef asiUI_IParamEditor_HeaderFile
#define asiUI_IParamEditor_HeaderFile

// asiUI includes
#include <asiUI.h>

// Active Data includes
#include <ActAPI_IParameter.h>

//-----------------------------------------------------------------------------

//! Interface for parameter editor UI control. This control is designed to
//! give the user access to the Active Data parameters for a particular
//! Data Node. This class is pure abstract. Its sub-classes should encapsulate
//! the real widget to tackle the Data Model contents.
class asiUI_IParamEditor : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_IParamEditor, Standard_Transient)

public:

  //! Sets the list of parameters to handle.
  virtual void
    SetParameters(const Handle(ActAPI_HIndexedParameterMap)& params) = 0;

  //! Updates and redisplays values of the handled parameters.
  virtual void
    UpdateParameters() = 0;

  //! Checks whether the user interaction with the parameter editor is
  //! enabled.
  //! \return Boolean value that indicates whether the parameter editor is
  //!         enabled for interaction or not.
  virtual bool
    IsEnabled() const = 0;

  //! Enables/disables user interaction with the parameter editor.
  //! \param[in] isEnabled indicates whether the parameter editor
  //!                      should be enabled or disabled.
  virtual void
    SetEnabled(const bool isEnabled) = 0;

  //! Checks whether the parameter editor is in a read-only state.
  //! \return Boolean value indicating the parameter editor is in
  //!         a read-only state or not.
  virtual bool
    IsReadOnly() const = 0;

  //! Enables/disables read-only state for the parameter editor.
  //! \param[in] isReadOnly specifies whether the parameter editor's
  //!                       state should be set to read-only or not.
  virtual void
    SetReadOnly(const bool isReadOnly) = 0;
};

#endif
