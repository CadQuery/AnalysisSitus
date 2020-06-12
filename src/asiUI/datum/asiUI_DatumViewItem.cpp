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

// asiUI includes
#include <asiUI_DatumViewItem.h>
#include <asiUI_DatumViewBase.h>
#include <asiUI_Datum.h>
#include <asiUI_Common.h>
#include <asiUI_DatumComboBox.h>

// asiAlgo includes
#include <asiAlgo_Dictionary.h>

// QT includes
#pragma warning(push, 0)
#include <QStandardItemModel>
#pragma warning(pop)

//! Returns one of the predefined pattern string for display value.
//! The method is only for convenience, the pattern can be generated
//! manually.
//! \param thePattern [in] the predefined pattern type.
//! \return the pattern.
QString asiUI_DatumViewItem::DefaultPattern(const Pattern thePattern)
{
  switch (thePattern)
  {
    case Pattern_LabelOnly: return "%label";
    case Pattern_ValueOnly: return "%value";
    case Pattern_UnitsOnly: return QObject::tr( "UNIT_DECORATION" ).arg( "%units" );
    case Pattern_LabelAndValue: return "%label %value";
    case Pattern_LabelAndUnits: return "%label " + QObject::tr( "UNIT_DECORATION" ).arg( "%units" );
    case Pattern_All: return "%label %value " + QObject::tr( "UNIT_DECORATION" ).arg( "%units" );
    default: return QString();
  }
}

//! Default constructor. Initializes empty item, without datum instance
//! used for value conversion and representation.
asiUI_DatumViewItem::asiUI_DatumViewItem() : QStandardItem()
{
  //
}

//! Set display value. The display value
//! is not formatted by datum until edited,
//! other roles (like edit, SiRole) use the
//! formatted and converted display values.
//! \param theValue [in] the value to set.
void asiUI_DatumViewItem::SetDisplayData(const QVariant& theValue)
{
  setData(theValue, Qt::DisplayRole);
}

//! \return display data.
QVariant asiUI_DatumViewItem::GetDisplayData() const
{
  return data(Qt::DisplayRole);
}

//! Set pattern for formatting display string.
//! \param thePattern [in] the pattern.
void asiUI_DatumViewItem::SetDisplayPattern(const QString& thePattern)
{
  m_Pattern = thePattern;
}

//! Get pattern used for formatting display string.
//! \return the pattern.
QString asiUI_DatumViewItem::GetDisplayPattern() const
{
  return m_Pattern;
}

//! Set edit value. The value is formatted
//! for display role and converted to SI system for SI role.
//! \param theValue [in] the value to set.
void asiUI_DatumViewItem::SetEditData(const QVariant& theValue)
{
  setData(theValue, Qt::EditRole);
}

//! \return edit data.
QVariant asiUI_DatumViewItem::GetEditData() const
{
  return data(Qt::EditRole);
}

//! Set model data. The value is converted
//! into local unit system for edit role 
//! and formatted for display role.
void asiUI_DatumViewItem::SetModelData(const QVariant& theValue)
{
  setData(theValue, DatumViewRole_ModelValue);
}

//! \return SI data.
QVariant asiUI_DatumViewItem::GetModelData() const
{
  return data(DatumViewRole_ModelValue);
}

//! Check that datum model value is in valid range.
//! \return TRUE if datum value is valid.
bool asiUI_DatumViewItem::CheckMinMax() const
{
  Handle(asiAlgo_DictionaryItem) aDictItem = this->dictionaryItem();

  if ( aDictItem.IsNull() )
    return true;

  asiAlgo_DictionaryItem::DataType aDataType = aDictItem->GetDataType();
  switch (aDataType)
  {
    case asiAlgo_DictionaryItem::Float   :
    case asiAlgo_DictionaryItem::Integer :
    {
      double aValue = GetModelData().toReal();
      return aValue >= aDictItem->GetMinValue()
        &&   aValue <= aDictItem->GetMaxValue();
    }

    default:
      return true;
  }
}

//! Update data representation of the item.
//! The item asks datum to format the internal
//! value of the item to a presentation defined
//! in data dictionary (units, format).
void asiUI_DatumViewItem::Update()
{
  Handle(asiAlgo_DictionaryItem) aDictionaryItem = this->dictionaryItem();
  if ( aDictionaryItem.IsNull() )
  {
    return;
  }

  // update item's data
  setData( data(DatumViewRole_ModelValue), DatumViewRole_ModelValue );
}

//! Provide a clone of the item.
//! \return pointer to a cloned instance.
QStandardItem* asiUI_DatumViewItem::clone() const
{
  return new asiUI_DatumViewItem();
}

//! Get data of the item.
//! \param theRole [in] the data role.
//! \return the data under a certain role.
QVariant asiUI_DatumViewItem::data(int theRole) const
{
  Handle(asiAlgo_DictionaryItem) aDictionaryItem = this->dictionaryItem();

  switch( theRole )
  {
    case Qt::DisplayRole :
    {
      if ( m_Pattern.isEmpty() )
      {
        return m_ShortFmtValue;
      }

      QString aDisplayStr = m_ShortFmtValue.toString();
      QString aLabelStr = ExtStr2QStr ( aDictionaryItem->GetLabel() );
      QString aUnitsStr = ExtStr2QStr ( aDictionaryItem->GetUnits() );
      return fromPattern(m_Pattern, aDisplayStr, aLabelStr, aUnitsStr);
    }

    case Qt::EditRole             : return m_LocalValue;
    case DatumViewRole_ModelValue : return m_ModelValue;
    case DatumViewRole_CopyRole :
    {
      if ( m_Pattern.isEmpty() )
      {
        return m_LongFmtValue.isValid() ? m_LongFmtValue : m_ShortFmtValue;
      }

      QString aDisplayStr = m_LongFmtValue.isValid() ? m_LongFmtValue.toString() : m_ShortFmtValue.toString();
      QString aLabelStr = ExtStr2QStr ( aDictionaryItem->GetLabel() );
      QString aUnitsStr = ExtStr2QStr ( aDictionaryItem->GetUnits() );
      return fromPattern(m_Pattern, aDisplayStr, aLabelStr, aUnitsStr);
    }

    case DatumViewRole_StatusUnits:
    case Qt::ToolTipRole:
    {
      if ( aDictionaryItem.IsNull() )
      {
        return QStandardItem::data(theRole);
      }

      return AsciiStr2QStr( aDictionaryItem->GetUnits() );
    }

    default: return QStandardItem::data(theRole);
  }
}

//! Set data to the item.
//! Provide custom data conversion.
//! \param theValue [in] the new value.
//! \param theRole [in] the role of the value.
void asiUI_DatumViewItem::setData(const QVariant &theValue, int theRole)
{
  Handle(asiAlgo_DictionaryItem) aDictionaryItem = this->dictionaryItem();

  switch ( theRole )
  {
    // set displayed string only
    case DatumViewRole_FormattedValue :
    {
      const QVariant& aPrevValue = m_LongFmtValue.isValid() ? m_LongFmtValue : m_ShortFmtValue;
      bool isChanged = theValue.isValid() != aPrevValue.isValid();
      if ( !isChanged && theValue.isValid() )
      {
        isChanged |= theValue.type() != aPrevValue.type();
        isChanged |= theValue        != aPrevValue;
      }

      if ( aDictionaryItem.IsNull() )
      {
        m_ModelValue    = theValue;
        m_LocalValue    = theValue;
        m_ShortFmtValue = theValue;
        m_LongFmtValue  = theValue;
        if ( isChanged )
        {
          emitDataChanged();
        }
        return;
      }

      m_ModelValue    = toModel(theValue, aDictionaryItem);
      m_LocalValue    = fromModel(m_ModelValue, aDictionaryItem);
      m_ShortFmtValue = format(m_LocalValue, Formatting_Short, aDictionaryItem);
      m_LongFmtValue  = format(m_LocalValue, Formatting_Long, aDictionaryItem);
      if ( isChanged )
      {
        emitDataChanged();
      }
      return;
    }

    case DatumViewRole_LocalValue :
    {
      const QVariant& aPrevValue = m_LocalValue;
      bool isChanged = theValue.isValid() != aPrevValue.isValid();
      if ( !isChanged && theValue.isValid() )
      {
        isChanged |= theValue.type() != aPrevValue.type();
        isChanged |= theValue        != aPrevValue;
      }

      if ( aDictionaryItem.IsNull() )
      {
        m_ModelValue    = theValue;
        m_LocalValue    = theValue;
        m_ShortFmtValue = theValue;
        m_LongFmtValue  = theValue;
        if ( isChanged )
        {
          emitDataChanged();
        }
        return;
      }

      m_ModelValue = toModel(theValue, aDictionaryItem);
      m_LocalValue = theValue;

      m_ShortFmtValue = format( fromModel( m_ModelValue, aDictionaryItem ), Formatting_Short, aDictionaryItem);
      m_LongFmtValue  = format( fromModel( m_ModelValue, aDictionaryItem ), Formatting_Long, aDictionaryItem);
      if ( isChanged )
      {
        emitDataChanged();
      }
      return;
    }

    case DatumViewRole_ModelValue :
    {
      const QVariant& aPrevValue = m_ModelValue;
      bool isChanged = theValue.isValid() != aPrevValue.isValid();
      if ( !isChanged && theValue.isValid() )
      {
        isChanged |= theValue.type() != aPrevValue.type();
        isChanged |= theValue        != aPrevValue;
      }

      if ( aDictionaryItem.IsNull() )
      {
        m_ModelValue    = theValue;
        m_LocalValue    = theValue;
        m_ShortFmtValue = theValue;
        m_LongFmtValue  = theValue;
        if ( isChanged )
        {
          emitDataChanged();
        }
        return;
      }

      m_ModelValue = theValue;
      m_LocalValue = fromModel(theValue, aDictionaryItem);

      m_ShortFmtValue = format( fromModel( m_ModelValue, aDictionaryItem ), Formatting_Short, aDictionaryItem);
      m_LongFmtValue  = format( fromModel( m_ModelValue, aDictionaryItem ), Formatting_Long, aDictionaryItem);
      if ( isChanged )
      {
        emitDataChanged();
      }
      return;
    }

    default:
      break;
  }

  return QStandardItem::setData(theValue, theRole);
}

//! Do value formatting in datum style.
//! \param theValue [in] the value to format
//! \param theDictItem [in] dictionary item.
QString asiUI_DatumViewItem::format(const QVariant& theValue,
                                     const Formatting theFormat,
                                     const Handle(asiAlgo_DictionaryItem)& theDictItem) const
{
  if ( theDictItem.IsNull() )
  {
    return theValue.toString();
  }
  
  asiUI_DatumFormatPtr aFormatter = 
    asiUI_DatumFormat::Make( QString( theDictItem->GetValueFormat().ToCString() ) );

  if ( aFormatter.isNull() )
  {
    return theValue.toString();
  }

  asiAlgo_DictionaryItem::DataType aDataType = theDictItem->GetDataType();
  switch (aDataType)
  {
    case asiAlgo_DictionaryItem::Float   : return aFormatter->Format( theValue.toDouble(), theFormat == Formatting_Long );
    case asiAlgo_DictionaryItem::Integer : return aFormatter->Format( theValue.toInt(),    theFormat == Formatting_Long );
    case asiAlgo_DictionaryItem::String  : return aFormatter->Format( theValue.toString(), theFormat == Formatting_Long );
    default:
      return theValue.toString();
  }
}

//! Match model id associated with the passed item for list data type.
//! \param theValue [in] the input value.
//! \param theDict [in] dictionary data definition.
//! \return the found id of the input value.
int asiUI_DatumViewItem::listIdByItem(const QVariant& theValue, const Handle(asiAlgo_DictionaryItem)& theDict) const
{
  Handle(TColStd_HArray1OfExtendedString) anItems;
  Handle(TColStd_HArray1OfInteger) anIndexes;
  theDict->GetListOfValues(anItems, anIndexes);

  // check if the passed value presents in the list
  int anIndexIt  = anItems->Lower();
  int aStringsIt = anIndexes->Lower();
  QString aIncomingString     = theValue.toString();

  for ( ; aStringsIt <= anIndexes->Upper(); ++aStringsIt, ++anIndexIt )
  {
    if ( ExtStr2QStr( anItems->Value(aStringsIt) ) == aIncomingString )
    {
      break;
    }
  }

  bool isFound = aStringsIt <= anIndexes->Upper();

  int anID = isFound ? anIndexes->Value(anIndexIt) : -1;

  if ( anID == -1 ) // not found, check for default value
  {
    TCollection_ExtendedString aDefaultString = theDict->GetDefaultValue();

    aStringsIt = anItems->Lower();
    anIndexIt  = anIndexes->Lower();

    for ( ; aStringsIt <= anItems->Upper(); ++aStringsIt, ++anIndexIt )
    {
      if ( anItems->Value(aStringsIt) == aDefaultString )
      {
        break;
      }
    }

    isFound = aStringsIt <= anIndexes->Upper();

    anID = isFound ? anIndexes->Value(anIndexIt) : -1;
  }

  return anID;
}

//! Select value from items list if specified by id.
//! \param theValue [in] the input value.
//! \param theDictItem [in] dictionary item.
//! \return the found id of the input value.
QVariant asiUI_DatumViewItem::listItemById(const int theId, const Handle(asiAlgo_DictionaryItem)& theDict) const
{
  Handle(TColStd_HArray1OfExtendedString) anItems;
  Handle(TColStd_HArray1OfInteger) anIndexes;
  theDict->GetListOfValues(anItems, anIndexes);

  // check if the passed value presents in the list
  int aStringIt = anItems->Lower();
  int anIndexIt = anIndexes->Lower();
  int anID      = (int) theId;

  for ( ; anIndexIt <= anIndexes->Upper(); ++anIndexIt )
  {
    if ( anIndexes->Value(anIndexIt) == anID )
    {
      break;
    }
  }

  bool isFound = anIndexIt <= anIndexes->Upper();
  if ( isFound )
  {
    return ExtStr2QStr( anItems->Value(anIndexIt) );
  }
  
  // seek for id of default value
  TCollection_ExtendedString aDefaultValue = theDict->GetDefaultValue();

  anIndexIt = anIndexes->Lower();

  for ( ; aStringIt <= anItems->Upper(); ++aStringIt, ++anIndexIt )
  {
    if ( anItems->Value(aStringIt) == aDefaultValue )
    {
      break;
    }
  }

  isFound = aStringIt <= anItems->Upper();
  if ( isFound )
  {
    return ExtStr2QStr( anItems->Value(aStringIt) );
  }

  return QVariant();
}

//! Convert value to local unit system using datum dictionary definition.
//! \param theValue [in] the converted value.
//! \param theDictItem [in] dictionary item.
QVariant asiUI_DatumViewItem::fromModel(const QVariant& theValue, const Handle(asiAlgo_DictionaryItem)& theDict) const
{
  if ( theDict.IsNull() )
  {
    return theValue; // if no dictionary data, define "as is"
  }
  
  switch ( theDict->GetDataType() )
  {
    // get model value converted to local system of units
    case asiAlgo_DictionaryItem::Float   :
    case asiAlgo_DictionaryItem::Integer :
    {
      return theDict->FromSI( theValue.toDouble() );
    }
    
    // get title string associated with the model index
    case asiAlgo_DictionaryItem::List :
    {
      return listItemById( theValue.toInt(), theDict );
    }
    
    default: return theValue;
  }
}

//! Convert value to model role using data dictionary definition.
//! \param theValue [in] the converted value.
//! \param theDictItem [in] dictionary item.
QVariant asiUI_DatumViewItem::toModel(const QVariant& theValue, const Handle(asiAlgo_DictionaryItem)& theDict) const
{
  if ( theDict.IsNull() )
  {
    return theValue; // if no dictionary data, define "as is"
  }

  switch ( theDict->GetDataType() )
  {
    // get model value converted from local system of units
    case asiAlgo_DictionaryItem::Float   :
    case asiAlgo_DictionaryItem::Integer :
    {
      return theDict->ToSI( theValue.toDouble() );
    }

    case asiAlgo_DictionaryItem::List :
    {
      return listIdByItem( theValue, theDict );
    }

    default: return theValue;
  }
}

//! Generate display string from specified pattern.
//! Patterns are generally used in headers.
//! \param thePattern [in] the pattern string.
//! \param theValue [in] the formatted display value.
//! \param theLabel [in] the label from dictionary item.
//! \param theUnits [in] the units from dictionary item.
QString asiUI_DatumViewItem::fromPattern(const QString& thePattern,
                                          const QString& theValue,
                                          const QString& theLabel,
                                          const QString& theUnits) const
{
  QString aResultStr = thePattern;
  aResultStr.replace("%label", theLabel);
  aResultStr.replace("%value", theValue);
  aResultStr.replace("%units", theUnits);
  return aResultStr;
}

//! Access view's layer to retrieve datum associated with the
//! item cell. The ownership and maintenance of the datum is up to
//! view's layer. Naturally, the datum is used a value formatting
//! and conversion tool in accordance with its representation
//! specific exploit by datum view.
//! \return safe pointer to the datum instance.
Handle(asiAlgo_DictionaryItem) asiUI_DatumViewItem::dictionaryItem() const
{
  asiUI_DatumViewModel* aModel = qobject_cast<asiUI_DatumViewModel*>( model() );

  // this kind of relationship seems to be weak, sure, can be implemented as
  // a little bit more transparent dependency...
  if ( !aModel )
  {
    aModel = qobject_cast<asiUI_DatumViewModel*>( model()->parent() );
  }
  
  if ( !aModel )
  {
    return nullptr;
  }

  asiUI_DatumViewBaseInterface* aViewBaseApi = dynamic_cast<asiUI_DatumViewBaseInterface*>( aModel->parent() );
  
  if ( !aViewBaseApi )
  {
    return nullptr;
  }

  QString aDatumId;

  asiUI_DatumViewModel::Domain aItemDomain = aModel->ItemDomain(this);

  switch (aItemDomain)
  {
    case asiUI_DatumViewModel::Domain_HeaderBand :
    case asiUI_DatumViewModel::Domain_HeaderSections :
    {
      aDatumId = aViewBaseApi->HeaderDatumId( aModel->Header(this), aModel->HeaderIndex(this) );
    }
    break;

    case asiUI_DatumViewModel::Domain_View :
    {
      aDatumId = index().isValid() ? aViewBaseApi->DatumId( index() ) : QString();
    }
    break;

    case asiUI_DatumViewModel::Domain_Undefined:
    default:
      break;
  }

  if ( aDatumId.isEmpty() )
  {
    return nullptr;
  }

  Handle(asiAlgo_DictionaryItem) aDictItem = asiAlgo_Dictionary::GetDictionaryItem( QStr2AsciiStr(aDatumId) );

  return aDictItem;
}
