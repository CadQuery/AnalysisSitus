//-----------------------------------------------------------------------------
// Created on: 19 September 2016
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

// Own include
#include <asiUI_DialogCommandsDelegate.h>

// Qt includes
#pragma warning(push, 0)
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor.
asiUI_DialogCommandsDelegate::asiUI_DialogCommandsDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{}

//-----------------------------------------------------------------------------

//! Qt overridden function to create an editor widget to edit item.
QWidget*
  asiUI_DialogCommandsDelegate::createEditor(QWidget*                    parent,
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

//-----------------------------------------------------------------------------

//! Qt overridden function to initialize the editor with the item's current data.
void asiUI_DialogCommandsDelegate::setEditorData(QWidget*           editor,
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

//-----------------------------------------------------------------------------

//! Qt overridden function to store data from editor widget in specified model.
void asiUI_DialogCommandsDelegate::setModelData(QWidget*            editor,
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
