//-----------------------------------------------------------------------------
// Created on: 04 December 2015
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

#ifndef asiUI_Viewer_h
#define asiUI_Viewer_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>

// Qt includes
#pragma warning(push, 0)
#include <QWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Base class for all viewers.
class asiUI_EXPORT asiUI_Viewer : public QWidget
{
  Q_OBJECT

public:

  asiUI_Viewer(QWidget* parent = NULL);

  virtual ~asiUI_Viewer();

public:

  virtual void Repaint() = 0;

public:

  const vtkSmartPointer<asiVisu_PrsManager>& PrsMgr() const { return m_prs_mgr; }

protected:

  vtkSmartPointer<asiVisu_PrsManager> m_prs_mgr; //!< Presentation Manager.

};

#endif
