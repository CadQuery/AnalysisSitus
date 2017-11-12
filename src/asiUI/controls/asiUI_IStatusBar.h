//-----------------------------------------------------------------------------
// Created on: 27 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiUI_IStatusBar_h
#define asiUI_IStatusBar_h

// asiUI includes
#include <asiUI.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

//! Interface for status bar.
class asiUI_IStatusBar : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_IStatusBar, Standard_Transient)

public:

  //! Shows specified text as a status hint.
  //! \param statusText [in] text to show as a status hint.
  virtual void
    SetStatusText(const TCollection_AsciiString& statusText) = 0;

  //! Shows progress indicator in the status bar.
  //! \param isOn [in] visibility of the status bar.
  virtual void
    ShowProgress(const bool isOn) = 0;

  //! Sets text for progress bar widget.
  //! \param progressText [in] text to display.
  virtual void
    SetProgressText(const TCollection_AsciiString& progressText) = 0;

  //! Switches on/off infinite state for the progress bar.
  //! \param isInfinite [in] true/false.
  virtual void
    SetProgressInfinite(const bool isInfinite) = 0;

  //! Updates the progress indicator using specified percentage value.
  //! \param percentage [in] percentage value to set.
  virtual void
    UpdateProgress(const int percentage) = 0;

  //! Shows specified information message.
  //! \param textMsg [in] text to show as information label.
  virtual void
    SetInfoMessage(const TCollection_AsciiString& textMsg) = 0;
};

#endif
