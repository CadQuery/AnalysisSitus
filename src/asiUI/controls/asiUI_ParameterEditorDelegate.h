//-----------------------------------------------------------------------------
// Created on: 21 February 2019
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

#ifndef asiUI_ParameterEditorDelegate_HeaderFile
#define asiUI_ParameterEditorDelegate_HeaderFile

// asiUI includes
#include <asiUI.h>
#include <asiUI_ParameterEditorView.h>
#include <asiUI_DatumDelegate.h>

// Qt includes
#pragma warning(push, 0)
#include <QItemDelegate>
#include <QPointer>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Delegate for initializing editors corresponding to parameter type
class asiUI_ParameterEditorDelegate : public asiUI_DatumDelegate
{
  Q_OBJECT

public:

  //! Column enumeration.
  enum ColumnID
  {
    Column_Name     = 0,
    Column_Computed = 1,
    Column_Value    = 2
  };

public:

  asiUI_EXPORT
    asiUI_ParameterEditorDelegate(asiUI_ParameterEditorView* theView);

protected:

  asiUI_EXPORT virtual asiUI_DatumItemEditor* 
    createDatum(QWidget*           theParent,
                const QModelIndex& theIndex) const;

  asiUI_EXPORT virtual bool
    isColorItem(const QModelIndex& theIndex) const;

  asiUI_EXPORT virtual bool
    isBoolItem(const QModelIndex& theIndex) const;

  asiUI_EXPORT virtual asiUI_DatumPathItem*
    isPathItem(const QModelIndex& theIndex) const;

  asiUI_EXPORT virtual asiUI_DatumCustomSelector*
    isCustomSelector(const QModelIndex& theIndex) const;

  asiUI_EXPORT virtual Qt::Alignment
    checkBoxAlignment() const;

protected:

  asiUI_EXPORT virtual int valueRole(const QModelIndex& theIndex) const;

  asiUI_EXPORT virtual int displayRole(const QModelIndex& theIndex) const;

private:

  //! reference to a parameter editor.
  QPointer<asiUI_ParameterEditorView> m_pTreeView;
};

#endif
