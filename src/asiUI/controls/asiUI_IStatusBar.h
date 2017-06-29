//-----------------------------------------------------------------------------
// Created on: 27 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
