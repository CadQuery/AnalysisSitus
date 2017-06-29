//-----------------------------------------------------------------------------
// Created on: 30 December 2016
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
