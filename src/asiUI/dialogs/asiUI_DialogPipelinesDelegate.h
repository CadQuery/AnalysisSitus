//-----------------------------------------------------------------------------
// Created on: 20 December 2017
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiUI_DialogPipelinesDelegate_h
#define asiUI_DialogPipelinesDelegate_h

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QStyledItemDelegate>
#pragma warning(pop)

class QObject;

//! Delegate for the table of pipelines.
class asiUI_DialogPipelinesDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:

  //! Data roles.
  enum Roles
  {
    WidgetRole = Qt::UserRole + 1
  };

public:

  asiUI_DialogPipelinesDelegate(QObject* parent);

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
