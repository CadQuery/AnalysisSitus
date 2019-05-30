//-----------------------------------------------------------------------------
// Created on: 30 May 2019
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

#ifndef asiUI_ObjectBrowserListener_h
#define asiUI_ObjectBrowserListener_h

// asiUI includes
#include <asiUI_CommonFacilities.h>
#include <asiUI_ObjectBrowser.h>

#pragma warning(disable : 4251)

//-----------------------------------------------------------------------------

//! Basic slots for object browser.
class asiUI_EXPORT asiUI_ObjectBrowserListener : public QObject
{
  Q_OBJECT

public:

  //! Constructor accepting all necessary facilities.
  //! \param[in] cf common facilities.
  asiUI_ObjectBrowserListener(const Handle(asiUI_CommonFacilities)& cf);

  //! Dtor.
  virtual
    ~asiUI_ObjectBrowserListener();

public:

  //! Connects this listener to the controls widget.
  virtual void
    Connect();

protected slots:

  //! Reaction to selection change in OB.
  //! \param[in] nodes selected Nodes.
  virtual void
    onSelectionChanged(const ActAPI_DataObjectIdList& nodes);

protected:

  //! Does something for a single Node. This method is invoked for every
  //! selected object one after another.
  //! \param[in] nid ID of the selected Data Node to process.
  virtual void
    processNode(const ActAPI_DataObjectId& nid);

protected:

  //! Common facilities.
  Handle(asiUI_CommonFacilities) m_cf;

};

#pragma warning(default : 4251)

#endif
