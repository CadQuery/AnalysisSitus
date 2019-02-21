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
#include <asiUI_DatumComboBox.h>

// SUIT includes
#pragma warning(push, 0)
#include <QtxComboBox.h>
#include <QDS_ComboBox.h>
#include <QEvent>
#include <QKeyEvent>
#pragma warning(pop)

//! Constructor.
//! \param theDicID [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for sub-widget controls.
//! \param theDatumFlags [in] datum sub-widget and option flags.
asiUI_DatumComboBox::asiUI_DatumComboBox(const QString& theDicID, 
                                           QWidget* theParent,
                                           const int theDatumFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDicID, theParent, convertFlags(theDatumFlags));

  init(m_pEditor);

  connect(m_pEditor, SIGNAL(activated(int)), this, SIGNAL(Activated(int)));
  connect(m_pEditor, SIGNAL(activated(const QString&)),
          this, SIGNAL(Activated(const QString&)));
}

//! Check if combo box allows text editing.
//! \return true if combo box is editable.
bool asiUI_DatumComboBox::IsEditable() const
{
  return m_pEditor->editable();
}

//! Enable/disable text editing.
//! \param theIsEditable [in] enable editing if true, or
//!        disable if false.
void asiUI_DatumComboBox::SetEditable(const bool theIsEditable)
{
  return m_pEditor->setEditable(theIsEditable);
}

//! Get number of items in the combo box.
//! \param theIsTotal [in] return only visible items count if false,
//!        otherwise get total number of items in combo box list.
int asiUI_DatumComboBox::GetItemCount(bool theIsTotal) const
{
  return m_pEditor->count(theIsTotal);
}

//! Get items identifiers.
//! \param theIDs [out] list of item IDs to return.
//! \param theIsTotal [in] return only visible items count if false,
//!        otherwise get total number of items in combo box list.
void asiUI_DatumComboBox::GetValues(QList<int>& theIDs, bool theIsTotal) const
{
  return m_pEditor->values(theIDs, theIsTotal);
}

//! Get visibility state of the item specified by theID.
//! \param theID [in] item id.
//! \return item's visibility state.
bool asiUI_DatumComboBox::GetVisibility(const int theID) const
{
  return m_pEditor->state(theID);
}

//! Set the visibility state of the item specified by theID.
//! If theID is -1 then specified state will be set to all items.
//! If theIsAppend append is set to true, keep current statuses already set
//! for other items, otherwise the statuses will be cleared.
//! \param theID [in] item id.
//! \param theIsVisible [in] visibility state.
//! \param theIsAppend [in] if true, keep statuses set for other items.
void asiUI_DatumComboBox::SetVisibility(const int  theID,
                                         const bool theIsVisible,
                                         const bool theIsAppend)
{
  m_pEditor->setState(theIsVisible, theID, theIsAppend);
}

//! Set the visibility state for set of items specified by theIDs list.
//! If theIsAppend append is set to true, keep current statuses already set
//! for other items, otherwise the statuses will be cleared.
//! \param theIDs [in] list of item ids.
//! \param theIsVisible [in] visibility state.
//! \param theIsAppend [in] if true, keep statuses set for other items.
void asiUI_DatumComboBox::SetVisibility(const QList<int>& theIDs,
                                         const bool theIsVisible,
                                         const bool theIsAppend)
{
  m_pEditor->setState(theIsVisible, theIDs, theIsAppend);
}

//! Add custom user items into the combo box.
//! These items will be added into the combo box list and will
//! act just like the standard dictionary items from list.
//! This method allows overriding the items.
//! \param theIDs [in] item IDs
//! \param theNames [in] item name strings.
void asiUI_DatumComboBox::SetValues(const QList<int>& theIDs,
                                     const QStringList& theNames)
{
  m_pEditor->SetValues(theIDs, theNames);
}

//! Add custom items into the combo box.
//! These items will be added into the combo box list and will
//! act just like the standard dictionary items from list.
//! The ids for items will be generated automatically, starting
//! from 0. (0, 1, 2, ...).
//! \param theNames [in] item name strings.
void asiUI_DatumComboBox::SetValues(const QStringList& theNames)
{
  QList<int> aIDs;
  QStringList::Iterator aStrIt;
  for ( int aIdx = 0; aStrIt != theNames.end(); ++aStrIt, ++aIdx )
    aIDs << aIdx;

  this->SetValues(aIDs, theNames);
}

//! Get item's id from its name string.
//! \param theName [in] item's name.
//! \return item id or -1 if not found.
int asiUI_DatumComboBox::GetIdForName(const QString& theName) const
{
  return m_pEditor->stringToValue(theName);
}

//! Get item's name from its id.
//! \param theID [in] item's id.
//! \return item name string or empty string if id is invalid.
QString asiUI_DatumComboBox::GetNameForId(const int theID) const
{
  return m_pEditor->valueToString(theID);
}

//! Update unit system.
void asiUI_DatumComboBox::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumComboBox::getDatum() const
{
  return m_pEditor;
}

//! Forces combo box editor to enlarge popup list
//! of items to show up all values without scrolling.
//! Please notice, that after turning this flag off,
//! the list size is kept in actual state.
//! \param theToEnlarge [in] the boolean flag to turn on/off
//! list auto enlargement mode.
void asiUI_DatumComboBox::SetListAutoEnlarge(const bool theToEnlarge)
{
  m_pEditor->SetListAutoEnlarge(theToEnlarge);
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Constructor. Initializes controls.
asiUI_DatumComboBox::Editor::Editor(const QString& theDicID,
                                      QWidget* theParent,
                                      const int theFlags)
: QDS_ComboBox(theDicID, theParent, theFlags),
  m_bAutoEnlarge(false)
{
}

//! Set user values for the combo-box.
//! This method overrides some specific QDS logic
//! to provide a workaround for internal troubles.
void asiUI_DatumComboBox::Editor::SetValues(const QList<int>&  theIDs,
                                             const QStringList& theNames)
{
  QtxComboBox* aCbx = comboBox();

  int aCurrIdx = value().toInt();
  QString aStr = valueToString(aCurrIdx);

  // re-populate list
  bool isBlocked = blockSignals(true);

  setValues(theIDs, theNames);
  UpdateUnits(); 

  // workaround: restore names back
  aCbx->clear();
  aCbx->addItems(theNames);

  // restore the same selection
  int aNewId = stringToValue(aStr);
  if ( aNewId >= 0 )
  {
    setValue(aNewId);
    this->blockSignals(isBlocked);
    return;
  }

  // reset selection
  blockSignals(isBlocked);
  setValue( theIDs.first() );

  if ( m_bAutoEnlarge ) 
    comboBox()->setMaxVisibleItems( count() );
}

//! Update system of units.
void asiUI_DatumComboBox::Editor::UpdateUnits()
{
  unitSystemChanged( QString() );

  if ( m_bAutoEnlarge ) 
    comboBox()->setMaxVisibleItems( count() );
}

//! Forces combo box editor to enlarge popup list
//! of items to show up all values without scrolling.
//! Please notice, that after turning this flag off,
//! the list size is kept in actual state.
//! \param theToEnlarge [in] the boolean flag to turn on/off
//! list auto enlargement mode.
void asiUI_DatumComboBox::Editor::SetListAutoEnlarge(const bool theToEnlarge)
{
  m_bAutoEnlarge = theToEnlarge;

  if ( m_bAutoEnlarge ) 
    comboBox()->setMaxVisibleItems( count() );
}

//! Set integer value (index of list item).
//! This method handles invalid values
//! by resetting value to default one.
//! \param theVal [in] the index value.
void asiUI_DatumComboBox::Editor::setIntegerValue(const int theVal)
{
  int aIdx = theVal;
  if ( valueToString(theVal).isNull() )
    aIdx = stringToValue( defaultValue() );

  QDS_ComboBox::setIntegerValue(aIdx);
}

//! Set double value (index of list item).
//! This method handles invalid values
//! by resetting value to default one.
//! \param theVal [in] the index value.
void asiUI_DatumComboBox::Editor::setDoubleValue(const double theVal)
{
  int aIdx = (int)theVal;
  if ( valueToString(theVal).isNull() )
    aIdx = stringToValue( defaultValue() );

  QDS_ComboBox::setDoubleValue(aIdx);
}

//! Set string value.
//! This method handles invalid values
//! by resetting value to default one.
//! \param theVal [in] the index value.
void asiUI_DatumComboBox::Editor::setStringValue(const QString& theStr)
{
  QString aValue = theStr;
  if ( stringToValue(theStr) < 0 )
    aValue = defaultValue();

  QDS_ComboBox::setStringValue(aValue);
}

//! Ensure the "isClear" flag is properly managed.
//! \param theString [in] the string to set.
void asiUI_DatumComboBox::Editor::setString(const QString& theString)
{
  QDS_ComboBox::setString(theString);

  QtxComboBox* aCombo = comboBox();
  if ( !aCombo || aCombo->count() == 0 )
    return;

  int aIndex = -1;
  for ( int i = 0; i < aCombo->count() && aIndex == -1; i++ )
  {
    if ( aCombo->itemText( i ) == theString )
    {
      aIndex = i;
    }
  }

  aCombo->setCleared(aIndex == -1);
}
