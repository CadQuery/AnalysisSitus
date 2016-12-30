//-----------------------------------------------------------------------------
// Created on: 30 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
