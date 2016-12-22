//-----------------------------------------------------------------------------
// Created on: 16 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogSTEPDelegate.h>

#pragma warning(push, 0)
#include <QLineEdit>
#include <QSpinBox>
#pragma warning(pop)

//! Constructor.
asiUI_DialogSTEPDelegate::asiUI_DialogSTEPDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{}

//! Qt overridden function to create an editor widget to edit item.
QWidget* asiUI_DialogSTEPDelegate::createEditor(QWidget*                    parent,
                                                 const QStyleOptionViewItem& /*option*/,
                                                 const QModelIndex&          index) const
{
  QWidget* wEditor = NULL;
  //
  if ( !index.column() )
    return wEditor;

  const int type = index.data(WidgetRole).toInt();
  switch ( type )
  {
    case 0: // INTEGER
    {
      wEditor = new QSpinBox(parent);
      static_cast<QSpinBox*>(wEditor)->setRange(0, INT_MAX);
      break;
    }
    case 1: // REAL
    {
      wEditor = new QDoubleSpinBox(parent);
      static_cast<QDoubleSpinBox*>(wEditor)->setRange(0., FLT_MAX);
      static_cast<QDoubleSpinBox*>(wEditor)->setDecimals(4);
      break;
    }
    case 2: // STRING
    {
      wEditor = new QLineEdit(parent);
      break;
    }
    default: break;
  }

  return wEditor;
}

//! Qt overridden function to initialize the editor with the item's current data.
void asiUI_DialogSTEPDelegate::setEditorData(QWidget*           editor,
                                             const QModelIndex& index) const
{
  if ( !index.column() )
    return;

  const int type = index.data(WidgetRole).toInt();
  switch ( type )
  {
    case 0: // INTEGER
    {
      QSpinBox* wEditor = qobject_cast<QSpinBox*>(editor);
      wEditor->setValue( index.data(Qt::EditRole).toString().toInt() );
      break;
    }
    case 1: // REAL
    {
      QDoubleSpinBox* wEditor = qobject_cast<QDoubleSpinBox*>(editor);
      wEditor->setValue( index.data(Qt::EditRole).toString().toDouble() );
      break;
    }
    case 2: // STRING
    {
      QLineEdit* wEditor = qobject_cast<QLineEdit*>(editor);
      wEditor->setText( index.data(Qt::EditRole).toString() );
      break;
    }
    default: break;
  }
}

//! Qt overridden function to store data from editor widget in specified model.
void asiUI_DialogSTEPDelegate::setModelData(QWidget*            editor,
                                            QAbstractItemModel* model,
                                            const QModelIndex&  index) const
{
  if ( !index.column() )
    return;

  const int type = index.data(WidgetRole).toInt();
  switch ( type )
  {
    case 0: // INTEGER
    {
      QSpinBox* wEditor = qobject_cast<QSpinBox*>(editor);
      model->setData(index, QString::number( wEditor->value() ), Qt::EditRole);
      break;
    }
    case 1: // REAL
    {
      QDoubleSpinBox* wEditor = qobject_cast<QDoubleSpinBox*>(editor);
      model->setData(index, QString::number( wEditor->value() ), Qt::EditRole);
      break;
    }
    case 2: // STRING
    {
      QLineEdit* wEditor = qobject_cast<QLineEdit*>(editor);
      model->setData(index, wEditor->text(), Qt::EditRole);
      break;
    }
    default: break;
  }
}
