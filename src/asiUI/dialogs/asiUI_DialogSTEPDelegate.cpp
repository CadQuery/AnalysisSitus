//-----------------------------------------------------------------------------
// Created on: 16 May 2016
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
#include <asiUI_DialogSTEPDelegate.h>

// Qt includes
#pragma warning(push, 0)
#include <QComboBox>
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
    case 2: // BOOL
    {
      wEditor = new QComboBox(parent);
      QComboBox* wCombo = static_cast<QComboBox*>(wEditor);
      //
      wCombo->addItem("false", 0);
      wCombo->addItem("true", 1);
      break;
    }
    case 3: // STRING
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
    case 2: // BOOL
    {
      QComboBox* wEditor = qobject_cast<QComboBox*>(editor);
      wEditor->setCurrentIndex( index.data(Qt::EditRole).toInt() );
      break;
    }
    case 3: // STRING
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
    case 2: // BOOL
    {
      QComboBox* wEditor = qobject_cast<QComboBox*>(editor);
      model->setData(index, wEditor->currentIndex(), Qt::EditRole);
      break;
    }
    case 3: // STRING
    {
      QLineEdit* wEditor = qobject_cast<QLineEdit*>(editor);
      model->setData(index, wEditor->text(), Qt::EditRole);
      break;
    }
    default: break;
  }
}
