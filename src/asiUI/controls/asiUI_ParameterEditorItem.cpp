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

// asiUI includes
#include <asiUI_ParameterEditorItem.h>
#include <asiUI_DatumColorItem.h>
#include <asiUI_DatumCheckBox.h>
#include <asiUI_DatumPathItem.h>
#include <asiUI_Common.h>

//! Constructor
//! \param theParent [in] - parent item. this item will be created as child
//!        of defined parent.
//! \param theName [in] item's name.
//! \param theNodalId [in] nodal id.
//! \param theDicId [in] dictionary id.
//! \param theDatum [in] datum editor.
asiUI_ParameterEditorItem::asiUI_ParameterEditorItem(
                                      QTreeWidgetItem* theParent,
                                      const QString& theName,
                                      const int& theNodalId,
                                      const Handle(DDS_DicItem)& theDicItem,
                                      asiUI_Datum* theDatum)
: QTreeWidgetItem(theParent)
{
  m_iNodalId  = theNodalId;
  m_pDatum    = theDatum;
  m_DicItem   = theDicItem;

  SetName(theName);
  SetComputed(false);
}

//! Destructor.
asiUI_ParameterEditorItem::~asiUI_ParameterEditorItem()
{
  delete m_pDatum;
}

//! Set item's name.
//! \param theName [in] item's name.
void asiUI_ParameterEditorItem::SetName(const QString& theName)
{
  setData(Column_Name, Qt::EditRole, theName);
}

//! Get data for specified item's role. This implementation redirects 
//! edit data of QColor to background role to provide visualization
//! of item's color.
//! \param theColumn [in] - the item's column, for which the data is queried.
//! \param theRole [in] - the data role.
QVariant asiUI_ParameterEditorItem::data(int theColumn, int theRole) const
{
  if (theColumn != Column_Value && theColumn != Column_Name)
  {
    return QVariant();
  }

  switch ( theRole )
  {
    // display string for copy - paste and usual display
    case Qt::DisplayRole :
      return (theColumn == Column_Name)
        ? m_DisplayName
        : m_LongFmtValue;

    case Role_ParamDictID : return GetDicId();
    case Role_ParamNodeID : return m_iNodalId;
    case Role_ShortFmtID  : return m_ShortFmtValue;
  }

  return QTreeWidgetItem::data(theColumn, theRole);
}

//! Set data to the item's roles.
//! \param theColumn [in] the edited column of the item.
//! \param theRole [in] the role of the data.
//! \param theValue [in] the new value.
void asiUI_ParameterEditorItem::setData(int theColumn, int theRole, const QVariant &theValue)
{
  switch ( theRole )
  {
    // change only display string
    case Qt::DisplayRole :
    {
      QString aString = theValue.toString();
      if ( theColumn == Column_Name && aString != m_DisplayName )
      {
        m_DisplayName = aString;
        emitDataChanged();
      }
      else if ( theColumn == Column_Value && aString != m_LongFmtValue )
      {
        m_LongFmtValue  = aString;
        m_ShortFmtValue = aString;
        emitDataChanged();
      }
    }
    break;

    case Qt::EditRole :
    {
      QString aString = theValue.toString();

      if ( theColumn == Column_Name )
      {
        m_DisplayName = nameString(aString);
      }
      else
      {
        m_LongFmtValue = !theValue.isValid() 
          ? asiUI_ParameterEditorView::tr("ERROR") 
          : valueString(aString, true);
        m_ShortFmtValue = !theValue.isValid()
          ? m_LongFmtValue
          : valueString(aString, false);
      }

      QTreeWidgetItem::setData(theColumn, theRole, theValue);
    }
    break;

    // process other roles of value column
    default:
      QTreeWidgetItem::setData(theColumn, theRole, theValue);
      break;
  }
}

//! Convert variant value to a display string that will be shown in value
//! column of parameter editor in non-editing state.
//! \param theValue [in] the value to convert.
//! \param theLongFmt [in] the long formatting.
//! \return string representation of the value.
QString asiUI_ParameterEditorItem::valueString(const QVariant& theValue,
                                                const bool theLongFmt)
{
  if ( m_pDatum != 0 )
  {
    m_pDatum->SetValue(theValue);

    return m_pDatum->GetDisplayedString(theLongFmt);
  }

  return theValue.toString();
}

//! Calculate display string for name column with units.
//! \param theName [in] the original item's name
//! \return name string with units
QString asiUI_ParameterEditorItem::nameString(const QString& theName)
{
  QString aName = theName;
  if ( !m_DicItem.IsNull() )
  {
    // check if the item has units.
    TCollection_AsciiString aUnits = m_DicItem->GetUnits();
    if ( !aUnits.IsEmpty() )
    {
      aName += " [" + asiUI_Common::ToQString(aUnits) + "]";
    }
  }

  return aName;
}

//! Checks if color editor used for the item.
bool asiUI_ParameterEditorItem::IsColorEditor() const
{
  return ( qobject_cast<asiUI_DatumColorItem*>( m_pDatum ) != 0 );
}

//! Checks if bool editor used for the item.
bool asiUI_ParameterEditorItem::IsBoolEditor() const
{
  return ( qobject_cast<asiUI_DatumCheckBox*>( m_pDatum ) != 0 );
}

//! Checks if bool editor used for the item.
bool asiUI_ParameterEditorItem::IsPathEditor() const
{
  return ( qobject_cast<asiUI_DatumPathItem*>( m_pDatum ) != 0 );
}
