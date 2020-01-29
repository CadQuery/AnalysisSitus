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

// Own include
#include <asiUI_DatumViewDelegate.h>

// asiUI includes
#include <asiUI_DatumViewBase.h>
#include <asiUI_DatumCheckBox.h>
#include <asiUI_DatumColorItem.h>
#include <asiUI_DatumPathItem.h>
#include <asiUI_DatumItemEditor.h>
#include <asiUI_DatumViewItem.h>
#include <asiUI_DatumCustomSelector.h>

//! Datum view delegate constructor.
//! \param theParent [in] parent view for the delegate.
//! For proper datum generation, the view should support
//! asiUI_DatumViewInterface.
asiUI_DatumViewDelegate::asiUI_DatumViewDelegate(QWidget* theParentView)
: asiUI_DatumDelegate(theParentView)
{
  connect( this, SIGNAL( EditorCreated(QWidget*, const QModelIndex&) ),
           this, SLOT( onEditorCreated(QWidget*, const QModelIndex&) ) );

  connect( this, SIGNAL( CustomSelectorTriggered(const QModelIndex&) ),
           this, SLOT( onCustomSelectorTriggered(const QModelIndex&) ) );
}

//! Create datum editor for the given model index.
//! This method should be redefined to provide a
//! datum editor for the edited model index.
//! \param theParent [in] parent widget.
//! \param theIndex [in] model index.
asiUI_DatumItemEditor* asiUI_DatumViewDelegate::createDatum(
                                                       QWidget* theParent,
                                                       const QModelIndex& theIndex) const
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return nullptr;

  QString aDatumId = aViewBaseApi->DatumId(theIndex);

  if ( aDatumId.isEmpty() )
    return nullptr;

  asiUI_Datum* aDatum = aViewBaseApi->AccessFactory()->CreateEditor(
    aDatumId, theParent, asiUI_Datum::Control);

  return new asiUI_DatumItemEditor(aDatum, theParent);
}

//! Check if the value for the specified index is color.
//! For the items that gets this flag defined as true,
//! corresponding presentation is provided by filling
//! the item's rectangle with color.
//! \param theIndex [in] model index.
//! \return Boolean flag indicating whether the items value is color.
bool asiUI_DatumViewDelegate::isColorItem(const QModelIndex& theIndex) const
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return false;

  return ( qobject_cast<asiUI_DatumColorItem*>( aViewBaseApi->AccessViewDatum(theIndex) ) != 0 );
}

//! Check if the value for the specified index is Boolean.
//! For the items that gets this flag defined as true,
//! corresponding presentation is provided by drawing
//! check-box in the item's rect.
//! \param theIndex [in] model index.
//! \return Boolean flag indicating whether the items value is Boolean.
bool asiUI_DatumViewDelegate::isBoolItem(const QModelIndex& theIndex) const
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return false;

  return ( qobject_cast<asiUI_DatumCheckBox*>( aViewBaseApi->AccessViewDatum(theIndex) ) != 0 );
}

//! Check if the item is represented by datum path editor
//! Path datums requires different representation and handling.
//! \param theIndex [in] index of item to check.
//! \return path editor instance or null, if not a path editor.
asiUI_DatumPathItem* asiUI_DatumViewDelegate::isPathItem(const QModelIndex& theIndex) const
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return nullptr;

  return qobject_cast<asiUI_DatumPathItem*>( aViewBaseApi->AccessViewDatum(theIndex) );
}

//! Check if the item represents custom selector field.
//! If yes, the method should return corresponding datum.
//! \param theIndex [in] model index.
//! \return custom selector datum.
asiUI_DatumCustomSelector*
  asiUI_DatumViewDelegate::isCustomSelector(const QModelIndex& theIndex) const
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return nullptr;

  return qobject_cast<asiUI_DatumCustomSelector*>( aViewBaseApi->AccessViewDatum(theIndex) );
}

//! Specify centered alignment for check-boxes.
//! \return alignment to use for all check-box datum editors.
Qt::Alignment asiUI_DatumViewDelegate::checkBoxAlignment() const
{
  return Qt::AlignCenter;
}

//! Provide model role for storing and getting value for putting in editor.
//! \param theIndex [in] the model index.
//! \return integer id corresponding to the role.
int asiUI_DatumViewDelegate::valueRole(const QModelIndex& asiUI_NotUsed(theIndex)) const
{
  return DatumViewRole_ModelValue;
}

//! Provide model role for getting display string for datum.
//! \param theIndex [in] the model index.
//! \return integer id corresponding to the role.
int asiUI_DatumViewDelegate::displayRole(const QModelIndex& asiUI_NotUsed(theIndex)) const
{
  return Qt::DisplayRole;
}

//! Provide editor created callback notification to base
//! layer of datum views.
//! \param theEditor [in] the created editor widget.
//! \param theIndex [in] the item model index.
void asiUI_DatumViewDelegate::onEditorCreated(QWidget* theEditor, const QModelIndex& theIndex)
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return;

  aViewBaseApi->NotifyDelegateEditorCreated(theEditor, theIndex);
}

//! Provide custom selection trigger callback
//! to base layer of datum views.
//! \param theIndex [in] the index of triggered item.
void asiUI_DatumViewDelegate::onCustomSelectorTriggered(const QModelIndex& theIndex)
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return;

  aViewBaseApi->NotifyCustomSelectorTriggered(theIndex);
}