//-----------------------------------------------------------------------------
// Created on: 22 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
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

#ifndef asiUI_DatumViewDelegate_HeaderFile
#define asiUI_DatumViewDelegate_HeaderFile

// asiUI includes
#include <asiUI.h>
#include <asiUI_DatumDelegate.h>

// Qt includes
#pragma warning(push, 0)
#include <QItemDelegate>
#include <QPersistentModelIndex>
#pragma warning(pop)

//! Default delegate that is used for editing items of datum views.
//! The delegates interacts with parent datum view through through
//! asiUI_DatumViewInterface.
//! It creates separate editors to the ones used in datum view for 
//! value conversion purposes, so no value input customization on
//! delegate's level is expected.
class asiUI_EXPORT asiUI_DatumViewDelegate : public asiUI_DatumDelegate
{
  Q_OBJECT

public:

  asiUI_DatumViewDelegate(QWidget* theParentView = 0);

protected:

  virtual asiUI_DatumItemEditor*
    createDatum(QWidget* theParent,
                const QModelIndex& theIndex) const;

  virtual bool
    isColorItem(const QModelIndex& theIndex) const;

  virtual bool
    isBoolItem(const QModelIndex& theIndex) const;

  virtual asiUI_DatumPathItem*
    isPathItem(const QModelIndex& theIndex) const;

  virtual asiUI_DatumCustomSelector*
    isCustomSelector(const QModelIndex& theIndex) const;

  virtual Qt::Alignment
    checkBoxAlignment() const;

  virtual int valueRole(const QModelIndex& theIndex) const;

  virtual int displayRole(const QModelIndex& theIndex) const;

protected slots:

  void
    onEditorCreated(QWidget* theEditor, const QModelIndex& theIndex);

  void
    onCustomSelectorTriggered(const QModelIndex& theIndex);
};

#endif
