//-----------------------------------------------------------------------------
// Created on: 30 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_OverrideCursor.h>

// Qt includes
#pragma warning(push, 0)
#include <QWidget>
#include <QApplication>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! ctor. Initializes wait cursor.
asiUI_OverrideCursor::asiUI_OverrideCursor() : QObject()
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  m_windows = this->windows();

  QApplication::instance()->installEventFilter(this);
}

//-----------------------------------------------------------------------------

//! Destructor. Releases the overridden cursor.
asiUI_OverrideCursor::~asiUI_OverrideCursor()
{
  QApplication::restoreOverrideCursor();
  QApplication::instance()->removeEventFilter(this);

  // Workaround for removing all override cursors
  this->suspend();
}

//-----------------------------------------------------------------------------

bool asiUI_OverrideCursor::eventFilter(QObject* o, QEvent* e)
{
  if ( o->isWidgetType() && ( e->type() == QEvent::Show ||
                              e->type() == QEvent::Hide ||
                              e->type() == QEvent::ShowToParent ||
                              e->type() == QEvent::HideToParent ) )
  {
    QWidget* w = (QWidget*) o;

    if ( w->isWindow() )
      this->updateState();
  }
  return QObject::eventFilter(o, e);
}

//-----------------------------------------------------------------------------

bool asiUI_OverrideCursor::isActive() const
{
  return m_cursors.isEmpty();
}

//-----------------------------------------------------------------------------

void asiUI_OverrideCursor::suspend()
{
  if ( !this->isActive() )
    return;

  while ( QApplication::overrideCursor() )
  {
    m_cursors.prepend( *QApplication::overrideCursor() );
    QApplication::restoreOverrideCursor();
  }
}

//-----------------------------------------------------------------------------

void asiUI_OverrideCursor::resume()
{
  if ( this->isActive() )
    return;

  for ( QList<QCursor>::const_iterator it = m_cursors.begin(); it != m_cursors.end(); ++it )
    QApplication::setOverrideCursor( *it );

  m_cursors.clear();
}

//-----------------------------------------------------------------------------

void asiUI_OverrideCursor::updateState()
{
  bool           changed    = false;
  QSet<QWidget*> curWinList = this->windows();
  //
  for ( QSet<QWidget*>::iterator it = curWinList.begin(); it != curWinList.end() && !changed; ++it )
    changed = !m_windows.contains( *it );

  if ( changed )
    this->suspend();
  else
    this->resume();
}

//-----------------------------------------------------------------------------

QSet<QWidget*> asiUI_OverrideCursor::windows() const
{
  QSet<QWidget*> res;
  QWidgetList lst = QApplication::topLevelWidgets();

  for ( QWidgetList::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QWidget* w = *it;

    if ( w->isVisible() )
      res.insert( w );
  }
  return res;
}
