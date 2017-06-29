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
