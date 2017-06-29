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

#ifndef asiUI_StatusBarImpl_h
#define asiUI_StatusBarImpl_h

// asiUI includes
#include <asiUI_IStatusBar.h>

// Qt includes
#pragma warning(push, 0)
#include <QPointer>
#pragma warning(pop)

class asiUI_StatusBar;

//! Implementation of status bar interface for Analysis Situs.
class asiUI_StatusBarImpl : public asiUI_IStatusBar
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_StatusBarImpl, asiUI_IStatusBar)

public:

  asiUI_EXPORT
    asiUI_StatusBarImpl(asiUI_StatusBar* statusBar);

public:

  asiUI_EXPORT virtual void
    SetStatusText(const TCollection_AsciiString& statusText);

  asiUI_EXPORT virtual void
    ShowProgress(const bool isShown);

  asiUI_EXPORT virtual void
    UpdateProgress(const int percentage);

  asiUI_EXPORT virtual void
    SetProgressText(const TCollection_AsciiString& progressText);

  asiUI_EXPORT virtual void
    SetProgressInfinite(const bool isInfinite);

  asiUI_EXPORT virtual void
    SetInfoMessage(const TCollection_AsciiString& textMsg);

  asiUI_EXPORT asiUI_StatusBar*
    GetStatusBar() const;

private:

  //! Status bar widget.
  QPointer<asiUI_StatusBar> m_pStatusBar;

};

#endif
