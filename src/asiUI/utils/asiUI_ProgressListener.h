//-----------------------------------------------------------------------------
// Created on: 29 December 2016 (getting closer to NY)
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

#ifndef asiUI_ProgressListener_h
#define asiUI_ProgressListener_h

// asiUI includes
#include <asiUI_IStatusBar.h>
#include <asiUI_Logger.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#include <QObject>

//! Default slots for reaction on algorithmic events.
class asiUI_ProgressListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ProgressListener(const Handle(asiUI_IStatusBar)&         statusBar,
                           const Handle(ActAPI_IProgressNotifier)& notifier,
                           const Handle(asiUI_Logger)&             logger);

  asiUI_EXPORT virtual
    ~asiUI_ProgressListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected slots:

  asiUI_EXPORT void onStep       ();
  asiUI_EXPORT void onMessage    ();
  asiUI_EXPORT void onLogMessage ();
  asiUI_EXPORT void onStatus     (const int);
  asiUI_EXPORT void onCancel     ();
  asiUI_EXPORT void onInfo       ();

protected:

  Handle(asiUI_IStatusBar)         m_statusBar; //!< Status bar.
  Handle(ActAPI_IProgressNotifier) m_notifier;  //!< Progress notifier.
  Handle(asiUI_Logger)             m_logger;    //!< Logger instance.

};

#endif
