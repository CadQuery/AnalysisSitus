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

// Own include
#include <asiUI_ParameterEditorDelegate.h>

// asiUI includes
#include <asiUI_DatumItemEditor.h>
#include <asiUI_WidgetFactory.h>
#include <asiUI_ParameterEditorItem.h>
#include <asiUI_DatumItemEditor.h>
#include <asiUI_DatumPathItem.h>
#include <asiUI_DatumCustomSelector.h>

//! Constructor.
//! Connects own signals to view's slots.
//! \param theEditor [in] - the parameter view to be used with this delegate.
asiUI_ParameterEditorDelegate::asiUI_ParameterEditorDelegate(asiUI_ParameterEditorView* theView)
: asiUI_DatumDelegate(theView),
  m_pTreeView(theView)
{
  connect( this, SIGNAL( EditorCreated(QWidget*, const QModelIndex&) ),
           theView, SLOT( onEditorCreated(QWidget*, const QModelIndex&) ) );

  connect( this, SIGNAL( CustomSelectorTriggered(const QModelIndex&) ),
           theView, SLOT( onCustomSelectorTriggered(const QModelIndex&) ) );

  SetRowMargin(5);
}

//! Create datum editor for the given model index.
//! This method should be redefined to provide a
//! datum editor for the edited model index.
//! \param theParent [in] parent widget.
//! \param theIndex [in] model index.
asiUI_DatumItemEditor*
  asiUI_ParameterEditorDelegate::createDatum(QWidget*           theParent,
                                             const QModelIndex& theIndex) const
{
  if ( m_pTreeView.isNull() )
    return NULL;
  
  asiUI_Datum* aDatum = m_pTreeView->GetItem(theIndex)->GetDatum();
  if ( !aDatum )
    return NULL;

  return new asiUI_DatumItemEditor(aDatum, theParent);
}

//! Check if the value for the specified index is color.
//! For the items that gets this flag defined as true,
//! corresponding presentation is provided by filling
//! the item's rect with color.
//! \param theIndex [in] model index.
//! \return boolean flag indicating whether the items value
//!         is color.
bool asiUI_ParameterEditorDelegate::isColorItem(const QModelIndex& theIndex) const
{
  if ( theIndex.column() != Column_Value )
    return false;

  if ( m_pTreeView.isNull() )
    return false;

  asiUI_ParameterEditorItem* anItem = m_pTreeView->GetItem(theIndex);
  if ( !anItem )
    return false;

  return anItem->IsColorEditor();
}

//! Check if the value for the specified index is boolean.
//! For the items that gets this flag defined as true,
//! corresponding presentation is provided by drawing
//! check-box in the item's rect.
//! \param theIndex [in] model index.
//! \return boolean flag indicating whether the items value
//!         is boolean.
bool asiUI_ParameterEditorDelegate::isBoolItem(const QModelIndex& theIndex) const
{
  if ( theIndex.column() != Column_Value )
    return false;

  if ( m_pTreeView.isNull() )
    return false;

  asiUI_ParameterEditorItem* anItem = m_pTreeView->GetItem(theIndex);
  if ( !anItem )
    return false;

  return anItem->IsBoolEditor();
}

//! Check if the item is represented by datum path editor
//! Path datums requires different representation and handling.
//! \param theIndex [in] index of item to check.
//! \return path editor instance or null, if not a path editor.
asiUI_DatumPathItem* asiUI_ParameterEditorDelegate::isPathItem(const QModelIndex& theIndex) const
{
  if ( theIndex.column() != Column_Value )
    return nullptr;

  if ( m_pTreeView.isNull() )
    return nullptr;

  asiUI_ParameterEditorItem* anItem = m_pTreeView->GetItem(theIndex);
  if ( !anItem )
    return nullptr;

  return qobject_cast<asiUI_DatumPathItem*>( anItem->GetDatum() );
}

//! Check if the item represents custom selector field.
//! If yes, the method should return corresponding datum.
//! \param theIndex [in] model index.
//! \return custom selector datum.
asiUI_DatumCustomSelector*
  asiUI_ParameterEditorDelegate::isCustomSelector(const QModelIndex& theIndex) const
{
  if ( theIndex.column() != Column_Value )
    return nullptr;

  if ( m_pTreeView.isNull() )
    return nullptr;

  asiUI_ParameterEditorItem* anItem = m_pTreeView->GetItem(theIndex);
  if ( !anItem )
    return nullptr;

  return qobject_cast<asiUI_DatumCustomSelector*>( anItem->GetDatum() );
}

//! Specify centered alignment for check-boxes.
//! \return alignment to use for all check-box datum editors.
Qt::Alignment asiUI_ParameterEditorDelegate::checkBoxAlignment() const
{
  return Qt::AlignLeft | Qt::AlignVCenter;
}

//! Provide model role for storing and getting value for putting in editor.
//! \param theIndex [in] the model index.
//! \return integer id corresponding to the role.
int asiUI_ParameterEditorDelegate::valueRole(const QModelIndex& asiUI_NotUsed(theIndex)) const
{
  return Qt::EditRole;
}

//! Provide model role for getting display string for datum.
//! \param theIndex [in] the model index.
//! \return integer id corresponding to the role.
int asiUI_ParameterEditorDelegate::displayRole(const QModelIndex& theIndex) const
{
  if ( theIndex.column() != Column_Value )
    return Qt::DisplayRole;

  asiUI_ParameterEditorItem* anItem = m_pTreeView->GetItem(theIndex);
  if ( !anItem )
    return Qt::DisplayRole;

  if ( anItem->IsLabel() )
    return Qt::EditRole; // Otherwise, we see some incorrect value [ssv].

  return asiUI_ParameterEditorItem::Role_ShortFmtID;
}
