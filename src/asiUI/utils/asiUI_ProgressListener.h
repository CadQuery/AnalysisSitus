//-----------------------------------------------------------------------------
// Created on: 29 December 2016 (getting closer to NY)
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

#ifndef asiUI_ProgressListener_h
#define asiUI_ProgressListener_h

// asiUI includes
#include <asiUI_IStatusBar.h>
#include <asiUI_Logger.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

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
