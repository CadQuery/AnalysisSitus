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
