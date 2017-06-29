//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
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

// Own include
#include <asiUI_ViewerDomainListener.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] host geometry viewer.
//! \param model          [in] Data Model instance.
asiUI_ViewerDomainListener::asiUI_ViewerDomainListener(asiUI_ViewerPart*              wViewerPart,
                                                       asiUI_ViewerDomain*            wViewerDomain,
                                                       asiUI_ViewerSurface*           wViewerSurface,
                                                       const Handle(asiEngine_Model)& model)
: QObject          (),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerDomainListener::~asiUI_ViewerDomainListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_ViewerDomainListener::Connect()
{
  // Nothing yet
}
