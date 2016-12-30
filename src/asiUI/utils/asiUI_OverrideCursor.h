//-----------------------------------------------------------------------------
// Created on: 30 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
