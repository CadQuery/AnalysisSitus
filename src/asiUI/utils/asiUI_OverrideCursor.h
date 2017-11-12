//-----------------------------------------------------------------------------
// Created on: 30 December 2016
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

#ifndef asiUI_OverrideCursor_h
#define asiUI_OverrideCursor_h

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QSet>
#include <QObject>
#pragma warning(pop)

//! Handy class for temporary exchanging the mouse cursor by "sand clock".
//! Simple creation of an instance of this class in a stack memory
//! does the trick. Cursor is reverted to initial state once that local
//! instance goes out of scope (destructed).
class asiUI_OverrideCursor : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_OverrideCursor();

  asiUI_EXPORT virtual
    ~asiUI_OverrideCursor();

public:

  asiUI_EXPORT virtual bool
    eventFilter(QObject*, QEvent*);

  asiUI_EXPORT virtual bool
    isActive() const;

  asiUI_EXPORT virtual void
    suspend();

  asiUI_EXPORT virtual void
    resume();

private:

  void           updateState();
  QSet<QWidget*> windows() const;

private:

  QList<QCursor> m_cursors;
  QSet<QWidget*> m_windows;

};

#endif
