//-----------------------------------------------------------------------------
// Created on: 16 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_STEP_delegate_h
#define gui_dialog_STEP_delegate_h

// Qt includes
#pragma warning(push, 0)
#include <QStyledItemDelegate>
#pragma warning(pop)

class QObject;

//! Delegate for the table of STEP interoperability options.
class gui_dialog_STEP_delegate : public QStyledItemDelegate
{
  Q_OBJECT

public:

  //! Data roles.
  enum Roles
  {
    WidgetRole = Qt::UserRole + 1
  };

public:

  gui_dialog_STEP_delegate(QObject *theParent);

public:

  virtual QWidget*
    createEditor(QWidget*                    parent,
                 const QStyleOptionViewItem& option,
                 const QModelIndex&          index) const;

  virtual void
    setEditorData(QWidget*           editor,
                  const QModelIndex& index) const;

  virtual void
    setModelData(QWidget*            editor,
                 QAbstractItemModel* model,
                 const QModelIndex&  index) const;

};

#endif
