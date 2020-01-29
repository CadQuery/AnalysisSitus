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
#include <asiAlgo_DictionaryItem.h>

// asiAlgo includes
#include <asiAlgo_DictionaryGroup.h>
#include <asiAlgo_DictionaryUnitSystem.h>
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_Logger.h>

// OCCT includes
#include <LDOM_Text.hxx>
#include <LDOM_Element.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NumericError.hxx>

#define INT_RANGE(X) Max((double)IntegerFirst(), Min((double)IntegerLast(), X) )
#define DBL_RANGE(X) Max(-1.e14, Min(1.e14, X) )

//! Constructor. 
//! \param theId [in] datum item id.
//! \param theGroup [in] component group item.
asiAlgo_DictionaryItem::asiAlgo_DictionaryItem(const TCollection_AsciiString& theId,
                                               const Handle(asiAlgo_DictionaryGroup)& theGroup)
: DDS_DicItem(),
  m_iPrecision(0),
  m_WidgetType(WT_Unknown),
  m_DataType(Unknown),
  m_Group(theGroup)
{
  SetId(theId);
  SetComponent(theGroup);
}

//! Fill internal data of dictionary item. Set Unit System tool
//! for the dictionary item.
//! \param theID [in] - the identifier string of datum item
//! \param theDatumElt [in] - the xml node for datum tag.
//! \param theGroupElt [in] - the xml node for group tag.
//! \param theDicElt [in] - the xml node for dictionary tag.
//! \param theUnitSystems [in] - list of units systems to prepare.
void asiAlgo_DictionaryItem::FillDataMap(TCollection_AsciiString theID, 
                                         const LDOM_Element& theDatumElt,
                                         const LDOM_Element& /*theGroupElt*/,
                                         const LDOM_Element& /*theDictElt*/,
                                         const TColStd_SequenceOfAsciiString& /*theUnitSystem*/)
{
  SetLabel(theDatumElt.getAttribute(DDS_Dictionary::KeyWord( "DATUM_LABEL" )));

  TCollection_AsciiString aFormat = theDatumElt.getAttribute(DDS_Dictionary::KeyWord( "DATUM_FORMAT" ));

  // -------------------------------------------------------------------------
  //  read domain tag
  // -------------------------------------------------------------------------

  LDOM_Element aDomain = 
    theDatumElt.GetChildByTagName(DDS_Dictionary::KeyWord( "DATUM_DOMAIN" ));
  if ( !aDomain.isNull() )
  {
    TCollection_AsciiString aType = 
      aDomain.getAttribute(DDS_Dictionary::KeyWord( "DOMAIN_TYPE" ));

    DataType aDataType = Unknown;

    if ( aType.IsEqual(DDS_Dictionary::KeyWord( "DOMAIN_TYPE_FLOAT" )) )
    {
      aDataType = Float;
    }
    else if ( aType.IsEqual(DDS_Dictionary::KeyWord( "DOMAIN_TYPE_STRING" )) )
    {
      aDataType = String;
    }
    else if ( aType.IsEqual(DDS_Dictionary::KeyWord( "DOMAIN_TYPE_INTEGER" )) )
    {
      aDataType = Integer;
    }
    else if ( aType.IsEqual(DDS_Dictionary::KeyWord( "DOMAIN_TYPE_LIST" )) )
    {
      aDataType = List;
    }
    else if ( aType.IsEqual(DDS_Dictionary::KeyWord( "DOMAIN_TYPE_BOOL" )) )
    {
      aDataType = Bool;
    }

    setDataType(aDataType);

    // read type specific attributes
    if ( aDataType == Integer || aDataType == Float )
    {
      // minimum value
      TCollection_AsciiString aMin = 
        aDomain.getAttribute(DDS_Dictionary::KeyWord( "DOMAIN_MIN" ));

      aMin.RemoveAll( ' ' );
      if ( aMin.IsRealValue() )
      {
        SetMin( (aDataType == Integer)
          ? INT_RANGE( aMin.RealValue() )
          : DBL_RANGE( aMin.RealValue() ) );
      }

      // maximum value
      TCollection_AsciiString aMax = 
        aDomain.getAttribute(DDS_Dictionary::KeyWord( "DOMAIN_MAX" ));

      aMax.RemoveAll( ' ' );
      if ( aMax.IsRealValue() )
      {
        SetMax( (aDataType == Integer)
          ? INT_RANGE( aMax.RealValue() )
          : DBL_RANGE( aMax.RealValue() ) );
      }
    }
    else if ( aDataType == List )
    {
      // list reference
      setListRefID(aDomain.getAttribute(DDS_Dictionary::KeyWord( "DOMAIN_LIST" )));
    }
    
    // calculate precision for float type
    if ( aDataType == Float )
      setPrecision(formatPrecision(aFormat));

    // default value
    TCollection_AsciiString aDefault = 
      aDomain.getAttribute(DDS_Dictionary::KeyWord( "DOMAIN_DEFAULT" ));
    aDefault.RemoveAll( ' ' );

    switch ( GetType() )
    {
      case DDS_DicItem::Integer:
      case DDS_DicItem::Float:
      {
        if ( aDefault.IsRealValue() )
        {
          SetDefaultValue( aDefault );
          SetDefaultValue( aDefault.RealValue() );
        }
        break;
      }

      case DDS_DicItem::List:
      case DDS_DicItem::String:
      case DDS_DicItem::Unknown:
        SetDefaultValue(aDefault);
    }
  }

  setValueFormat(aFormat);

  // -------------------------------------------------------------------------
  //  read dimension tag
  // -------------------------------------------------------------------------

  m_DimensionMap.Clear();

  // read dimenstion tag
  LDOM_Element aDimensionSection = 
    theDatumElt.GetChildByTagName(DDS_Dictionary::KeyWord( "DATUM_DIMENSION" ));
  if ( !aDimensionSection.isNull() )
  {
    LDOM_NodeList aDimensionList =
      aDimensionSection.getElementsByTagName(DDS_Dictionary::KeyWord( "DIMENSION" ));

    for ( int anIdx = 0; anIdx < aDimensionList.getLength(); anIdx++ )
    {
      LDOM_Node           dimNode    = aDimensionList.item(anIdx);
      const LDOM_Element& aDimension = (const LDOM_Element&) dimNode;

      // read dimension identifier
      TCollection_AsciiString aDimensionName = 
        aDimension.getAttribute(DDS_Dictionary::KeyWord( "DIMENSION_NAME" ));

      TCollection_AsciiString aDimensionValue =
        aDimension.getAttribute(DDS_Dictionary::KeyWord( "DIMENSION_VALUE" ));
      
      m_DimensionMap.Add(aDimensionName, Dimension(aDimensionValue));
    }
  }

  // -------------------------------------------------------------------------
  //  read ui tag
  // -------------------------------------------------------------------------

  // read ui tag
  LDOM_Element anUI =
    theDatumElt.GetChildByTagName(DDS_Dictionary::KeyWord( "DATUM_UI" ));
  if ( !anUI.isNull() )
  {
    TCollection_AsciiString aWidget = 
      anUI.getAttribute(DDS_Dictionary::KeyWord( "UI_WIDGET" ));

    // define widget type
    if ( aWidget.IsEqual( "LineEdit" ) )
      setWidgetType(WT_LineEdit);
    else if ( aWidget.IsEqual( "SpinBox" ) )
      setWidgetType(WT_SpinBox);
    else if ( aWidget.IsEqual( "ComboBox" ) )
      setWidgetType(WT_ComboBox);
    else if ( aWidget.IsEqual( "OpenFilePath" ) )
      setWidgetType(WT_OpenFilePath);
    else if ( aWidget.IsEqual( "SaveFilePath" ) )
      setWidgetType(WT_SaveFilePath);
    else if ( aWidget.IsEqual( "DirPath" ) )
      setWidgetType(WT_DirPath);
    else if ( aWidget.IsEqual( "CheckBox" ) )
      setWidgetType(WT_CheckBox);
    else if ( aWidget.IsEqual( "DatePicker" ) )
      setWidgetType(WT_DatePicker);
    else if ( aWidget.IsEqual( "ColorPicker" ) )
      setWidgetType(WT_ColorPicker);
    else if ( aWidget.IsEqual( "Selector" ) )
      setWidgetType(WT_Selector);
    else if ( aWidget.IsEqual( "Label" ) )
      setWidgetType(WT_Label);
    else if ( aWidget.IsEqual( "CustomSelector" ) )
      setWidgetType(WT_CustomSelector);

    parseWidgetParams(anUI);
  }

  // -------------------------------------------------------------------------
  //  read description tag
  // -------------------------------------------------------------------------

  // read description tag
  LDOM_Element aDescription = 
    theDatumElt.GetChildByTagName(DDS_Dictionary::KeyWord( "DATUM_DESCRIPTION" ));
  if ( !aDescription.isNull() )
  {
    LDOM_Node aNode = aDescription.getFirstChild();
    const LDOM_Text& aText = (const LDOM_Text&) aNode;
        
    LDOM_Text aDescrText(aText);
    if ( !aDescrText.isNull() )
    {
      TCollection_ExtendedString aDescrStr = aDescrText.getData();
      SetShortDescription(aDescrStr);
      SetLongDescription(aDescrStr);
    }
  }

}

//! Get decimal precision for the value formatting.
//! \param theSystem [in] - the unit system identifier.
//! \return number of precision digits.
int asiAlgo_DictionaryItem::GetPrecision(const UnitSystem& /*theSystem*/) const
{
  return m_iPrecision;
}

//! This method allows you to redefine the value lists of the Datum. This is
//! mostly useful for customization of combo-box widgets which are going to
//! source their values not from XML configuration but from other inputs,
//! e.g. some dynamically specified collections of user values.
//! \param theValues [in] array of values.
//! \param theIndices [in] array of indices.
void asiAlgo_DictionaryItem::OverrideListOfValues(const Handle(HStringArray)& theValues,
                                                  const Handle(HIntArray)& theIndices)
{
  this->SetListOfValues(theValues, theIndices);
  TCollection_AsciiString aDefStr = this->GetDefaultValue();
  if ( aDefStr.IsEmpty() && theIndices->Length() > 0 )
    this->SetDefaultValue( theValues->Value( theValues->Lower() ) );
}

//! Get format string for unit system given by its identifier as input argument.
//! \param theUnitSystem [in] - the unit system identifier.
//! \param theIsCanonical [in] - the argument that forces the format string to be
//!        reduced according to the sprintf() specification (without extra non standard qualifiers).
//! \return format string.
TCollection_AsciiString asiAlgo_DictionaryItem::GetFormat(
  const UnitSystem& /*theSystem*/,
  const bool /*theIsCanonical*/) const
{ 
  return GetValueFormat();
}

// ---------------------------------------------------------------------------
//         unit conversion and unit string generation
// ---------------------------------------------------------------------------

//! Perform a conversion of value from local units used by data dictionary
//! to SI units system.
//! \param theValue [in] - the input value to convert.
//! \return value in SI units.
double asiAlgo_DictionaryItem::ToSI(const double theValue)
{
  return ToSI(theValue, asiAlgo_Dictionary::GetLocalUnitSystem());
}

//! Perform a conversion of value from specified system of units
//! to SI units system.
//! \param theValue [in] - the input value to convert.
//! \param theSystem [in] - system of units.
//! \return value in SI units.
double asiAlgo_DictionaryItem::ToSI(const double theValue,
                                           const asiAlgo_DictionaryUnitSystem& theSystem) const
{
  // flags indicating whether the loop computes value for 
  // scale-only or scale-with-offset units
  bool hasScale  = false;
  bool hasOffset = false;

  double aResult = 0.0;
  double aFactor = 1.0;

  for ( int anIdx = 1; anIdx <= m_DimensionMap.Extent(); anIdx++ )
  {
    // skip extra dimension
    TCollection_AsciiString aDimensionName = m_DimensionMap.FindKey(anIdx);
    if ( aDimensionName.IsEqual( "extra" ) )
      continue;

    // log and skip invalid dimensions
    Handle(asiAlgo_DictionaryDimension) aDimension = m_Group->GetDimension(aDimensionName);
    if ( aDimension.IsNull() )
    {
      if ( asiAlgo_Dictionary::IsConversionErrorLog() )
      {
        Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
        if ( aNotifier.IsNull() )
          return theValue;

        aNotifier->SendLogMessage( LogErr(Normal) << "DICTIONARY_IVALID_DIMENSION"
          << aDimensionName << GetId() );
      }

      continue;
    }

    // log and skip invalid units
    Handle(asiAlgo_DictionaryUnit) aUnit = 
      aDimension->GetUnit(theSystem.GetUnit(aDimensionName));
    if ( aUnit.IsNull() )
    {
      if ( asiAlgo_Dictionary::IsConversionErrorLog() )
      {
        Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
        if ( aNotifier.IsNull() )
          return theValue;

        aNotifier->SendLogMessage( LogErr(Normal) << "DICTIONARY_CANNOT_GET_UNIT_FOR_DIMENSION"
          << aDimensionName << GetId() );
      }

      continue;
    }

    // skip null powers
    const Dimension& aPowers = m_DimensionMap.FindFromIndex(anIdx);
    int aPower = aPowers.GetSummaryPower();
    if ( !aPower )
      continue;

    double aScale  = aUnit->GetScaleFactor();
    double aOffset = aUnit->GetOffset();

    // unit consistency check
    if ( hasOffset || ( aOffset != 0 && ( hasScale || Abs(aPower) > 1 ) ) )
    {
      if ( asiAlgo_Dictionary::IsConversionErrorLog() )
      {
        Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
        if ( aNotifier.IsNull() )
          return theValue;

        aNotifier->SendLogMessage( LogErr(Normal) << "DICTIONARY_TO_SI_INVALID_UNIT"
          << GetUnits(theSystem) << GetId() );
      }

      return theValue;
    }

    try
    {
      OCC_CATCH_SIGNALS

      if ( aOffset != 0 )
      { 
        // offset with scale calculation
        // case of ^-1 also computed by adding power to theValue
        aResult  = Pow(Pow(theValue, aPower) * aScale - aUnit->GetOffset(), aPower);
      
        // continue loop with offset
        hasOffset = true;
      }
      // plain scale calcuation
      else
      {
        // scale-only calcuation
        aFactor *= Pow(aScale, aPower);
      
        // continue loop with scale only
        hasScale  = true;
      }
    }
    catch ( Standard_NumericError aNumericError )
    {
      Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
      if ( aNotifier.IsNull() )
        return theValue;

      aNotifier->SendLogMessage( LogErr(High) << "DICTIONARY_TO_SI_CONVERSION_FAILED"
        << aNumericError.GetMessageString() << GetUnits(theSystem) << GetId() << theValue );

      return theValue;
    }
  }

  return !hasOffset ? theValue * aFactor : aResult;
}

//! Perform a conversion of value from SI units to local system of units
//! used by data dictionary.
//! \param theValue [in] - the input value to convert.
//! \return value in local system of units.
double asiAlgo_DictionaryItem::FromSI(const double theValue)
{
  return FromSI(theValue, asiAlgo_Dictionary::GetLocalUnitSystem());
}

//! Perform a conversion of value from SI units to a specified system of units.
//! \param theValue [in] - the input value to convert.
//! \param theSystem [in] - system of units.
//! \return value in the specified system of units.
double asiAlgo_DictionaryItem::FromSI(const double theValue,
                                      const asiAlgo_DictionaryUnitSystem& theSystem) const
{
  // flags indicating whether the loop computes value for 
  // scale-only or scale-with-offset units
  bool hasScale  = false;
  bool hasOffset = false;

  double aResult = 0.0;
  double aFactor = 1.0;

  for ( int anIdx = 1; anIdx <= m_DimensionMap.Extent(); anIdx++ )
  {
    // skip extra dimension
    TCollection_AsciiString aDimensionName = m_DimensionMap.FindKey(anIdx);
    if ( aDimensionName.IsEqual( "extra" ) )
      continue;

    // log and skip invalid dimensions
    Handle(asiAlgo_DictionaryDimension) aDimension = m_Group->GetDimension(aDimensionName);
    if ( aDimension.IsNull() )
    {
      if ( asiAlgo_Dictionary::IsConversionErrorLog() )
      {
        Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
        if ( aNotifier.IsNull() )
          return theValue;

        aNotifier->SendLogMessage( LogErr(Normal) << "DICTIONARY_IVALID_DIMENSION"
          << aDimensionName << GetId() );
      }

      continue;
    }

    // log and skip invalid units
    Handle(asiAlgo_DictionaryUnit) aUnit = 
      aDimension->GetUnit(theSystem.GetUnit(aDimensionName));
    if ( aUnit.IsNull() )
    {
      if ( asiAlgo_Dictionary::IsConversionErrorLog() )
      {
        Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
        if ( aNotifier.IsNull() )
          return theValue;

        aNotifier->SendLogMessage( LogErr(Normal) << "DICTIONARY_CANNOT_GET_UNIT_FOR_DIMENSION"
          << aDimensionName << GetId() );
      }

      continue;
    }

    // skip null powers
    const Dimension& aPowers = m_DimensionMap.FindFromIndex(anIdx);
    int aPower = aPowers.GetSummaryPower();
    if ( !aPower )
      continue;

    double aScale  = aUnit->GetScaleFactor();
    double aOffset = aUnit->GetOffset();

    // unit consistency check
    if ( hasOffset || ( aOffset != 0 && ( hasScale || Abs(aPower) > 1 ) ) )
    {
      if ( asiAlgo_Dictionary::IsConversionErrorLog() )
      {
        Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
        if ( aNotifier.IsNull() )
          return theValue;

        aNotifier->SendLogMessage( LogErr(Normal) << "DICTIONARY_FROM_SI_INVALID_UNIT"
          << GetUnits(theSystem) << GetId() );
      }

      return theValue;
    }

    try
    {
      OCC_CATCH_SIGNALS

      if ( aOffset != 0 )
      {
        // offset with scale calculation
        // case of ^-1 also computed by adding power to theValue
        aResult = Pow((Pow(theValue, aPower) + aOffset) / aScale, aPower);

        // continue loop with offset
        hasOffset = true;
      }
      else
      {
        // scale-only calcuation
        aFactor *= Pow(aScale, -aPower);
      
        // continue loop with scale only
        hasScale  = true;
      }
    }
    catch ( Standard_NumericError aNumericError )
    {
      Handle(ActAPI_IProgressNotifier) aNotifier = asiAlgo_Dictionary::GetNotifier();
      if ( aNotifier.IsNull() )
        return theValue;

      aNotifier->SendLogMessage( LogErr(High) << "DICTIONARY_FROM_SI_CONVERSION_FAILED"
        << aNumericError.GetMessageString() << GetUnits(theSystem) << GetId() << theValue );

      return theValue;
    }
  }

  return !hasOffset ? theValue * aFactor : aResult;
}

//! Get unit string for local system of units used by
//! data dictionary.
//! \return unit string generated for units used in
//!         the data dictionary system of units.
TCollection_AsciiString asiAlgo_DictionaryItem::GetUnits() const
{
  return GetUnits(asiAlgo_Dictionary::GetLocalUnitSystem());
}

//! Get unit string for specified system of units.
//! \param theSystem [in] the system of units.
//! \return unit string generated for units used in
//!         the specified system of units.
TCollection_AsciiString
  asiAlgo_DictionaryItem::GetUnits(const asiAlgo_DictionaryUnitSystem& theSystem) const
{
  TCollection_AsciiString aUnitPos, aUnitNeg;
  TCollection_AsciiString aExtra;
  Standard_Character aTmp[200];

  for ( int anIdx = 1; anIdx <= m_DimensionMap.Extent(); anIdx++ )
  {
    TCollection_AsciiString aDimensionName = m_DimensionMap.FindKey(anIdx);
    if ( aDimensionName.IsEqual( "extra" ) )
    {
      aExtra = m_DimensionMap.FindFromIndex(anIdx).GetPowerString();
      continue;
    }

    Handle(asiAlgo_DictionaryDimension) aDimension = m_Group->GetDimension(aDimensionName);
    if ( aDimension.IsNull() )
      continue;

    Handle(asiAlgo_DictionaryUnit) aUnit = 
      aDimension->GetUnit(theSystem.GetUnit(aDimensionName));
    if ( aUnit.IsNull() )
      continue;

    const Dimension& aPowers = m_DimensionMap.FindFromIndex(anIdx);

    int aPower[2];
    aPower[0] = aPowers.GetFirstPower();
    aPower[1] = aPowers.GetSecondPower();
    
    for ( int anIdx2 = 0; anIdx2 < 2; anIdx2++ )
    {
      if ( aPower[anIdx2] == 0 )
        continue;

      TCollection_AsciiString& aUnitStr =
        aPower[anIdx2] > 0 ? aUnitPos : aUnitNeg;
      
      if ( aUnitStr.Length() )
        aUnitStr += ".";

      aUnitStr += aUnit->GetName();
      if ( aPower[anIdx2] != 1 )
      {
#ifdef _WIN32
        sprintf_s(aTmp, 200, "^%d", aPower[anIdx2]);
#else
        snprintf(aTmp, 200, "^%d", aPower[anIdx2]);
#endif
        aUnitStr += aTmp;
      }
    }
  }

  if ( aUnitPos.Length() && ( aUnitNeg.Length() || aExtra.Length() ) )
    aUnitPos += TCollection_AsciiString(".");

  if ( aUnitNeg.Length() && aExtra.Length() )
    aUnitNeg += TCollection_AsciiString(".");

  return aUnitPos + aUnitNeg + aExtra;
}

double asiAlgo_DictionaryItem::ToSI(const double theValue,
                                    const UnitSystem& /*theSystem*/) const
{
  return ToSI(theValue, asiAlgo_Dictionary::GetLocalUnitSystem());
}

double asiAlgo_DictionaryItem::FromSI(const double theValue,
                                      const UnitSystem& /*theSystem*/) const
{
  return FromSI(theValue, asiAlgo_Dictionary::GetLocalUnitSystem());
}

TCollection_AsciiString
  asiAlgo_DictionaryItem::GetUnits(const UnitSystem& /*theUnitSystem*/) const
{
  return GetUnits(asiAlgo_Dictionary::GetLocalUnitSystem());
}

// ---------------------------------------------------------------------------
//                       protected class fields modificators
// ---------------------------------------------------------------------------

//! Discover widget parameters contained
//! in <ui> tag.
//! \param theUI [in] the tag's element.
void asiAlgo_DictionaryItem::parseWidgetParams(const LDOM_Element& theUI)
{
  /* =======================================
   *  Parse "widgetParams" attribute string
   * ======================================= */

  TCollection_AsciiString 
    aParamAttr = theUI.getAttribute( DDS_Dictionary::KeyWord("UI_PARAMS") );

  if ( !aParamAttr.IsEmpty() )
  {
    const TCollection_AsciiString aSepCh     = DDS_Dictionary::KeyWord( "SEPARATOR_CHARACTER" );
    const TCollection_AsciiString anAssignCh = DDS_Dictionary::KeyWord( "ASSIGN_CHARACTER" );

    int aTokenIdx = 0;
    TCollection_AsciiString aToken = aParamAttr.Token( aSepCh.ToCString() );
    if ( aToken.IsEmpty() )
      aToken = aParamAttr;

    while ( !aToken.IsEmpty() )
    {
      // parse assignment
      int aAssignIdx = aToken.Search(anAssignCh);
      if ( aAssignIdx > 0 )
      {
        aAssignIdx--;
        TCollection_AsciiString aValue = aToken.Split( aAssignIdx + anAssignCh.Length() );
        aToken.Trunc(aAssignIdx);
        aToken.RightAdjust();
        aValue.LeftAdjust();
        aValue.RightAdjust();

        m_WidgetParams.Bind(aToken, aValue);
      }

      aToken = aParamAttr.Token( aSepCh.ToCString(), ++aTokenIdx );
    }
  }

  /* =======================================
   *  Parse <widgetParam> sub tags
   * ======================================= */

  const LDOMString aSubTagKey  = DDS_Dictionary::KeyWord("UI_PARAM_TAG");
  const LDOMString aParamIdKey = DDS_Dictionary::KeyWord("ID");
 
  // get list of sub tags
  LDOM_NodeList aSubTags = theUI.getElementsByTagName(aSubTagKey);

  // iterate over subtags to collect widget parameters
  for ( int anIt = 0; anIt < aSubTags.getLength(); anIt++ )
  {
    LDOM_Node           paramTagNode = aSubTags.item(anIt);
    const LDOM_Element& aParamTag    = (const LDOM_Element&) paramTagNode;

    // read id
    const TCollection_AsciiString aID = aParamTag.getAttribute(aParamIdKey);
    if ( aID.IsEmpty() || m_WidgetParams.IsBound(aID) )
      continue;

    // read value
    LDOM_Node        valueNode = aParamTag.getFirstChild();
    const LDOM_Text& aValue    = (const LDOM_Text&) valueNode;
    if ( aValue.isNull() )
      continue;

    LDOMString aOptionTxt = aValue.getData();
    m_WidgetParams.Bind(aID, aOptionTxt);
  }
}

//! Set data type for dictionary item. 
//! The type is used for selecting
//! appropriate editor in GUI
//! layer. 
//! This type defines editor widget.
void asiAlgo_DictionaryItem::setDataType(const DataType theDataType)
{
  switch ( theDataType )
  {
    case Integer:  SetType(DDS_DicItem::Integer); break;
    case Float:    SetType(DDS_DicItem::Float);   break;
    case String:   SetType(DDS_DicItem::String);  break;
    case List:     SetType(DDS_DicItem::List);    break;
    case Bool:     
    {
      SetType(DDS_DicItem::Integer);
      SetMin(0);
      SetMax(1);
      break;
    }
    default:
      SetType(DDS_DicItem::Unknown); break;
  }

  m_DataType = theDataType;
}

//! parse format string to find a value precision.
//! \param theFormat [in] format string.
//! \return precision.
int asiAlgo_DictionaryItem::formatPrecision(const TCollection_AsciiString& theFormat) const
{
  // checking % presenting
  if ( *theFormat.ToCString() != '%' )
    return 0;

  TCollection_AsciiString aStr = (theFormat.ToCString() + 1);
  Standard_Character aType = aStr.Value(aStr.Length());

  if ( ( aType != 'f' && aType != 'g' && aType != 'e' && aType != 'G' && aType != 'E' ) )
    return 0;

  // removing type character
  aStr.Trunc(aStr.Length() - 1);

  TCollection_AsciiString aFlags;
  while ( !aStr.IsEmpty() && aStr.Value(1) != '.' && ( aStr.Value(1) < '0' || aStr.Value(1) > '9' ) )
  {
    aFlags = aFlags + aStr.Value(1);
    aStr.Remove(1);
  }

  int aPos = 1;
  while ( aPos <= aStr.Length() && ( aStr.Value(aPos) == '.' ||
          ( aStr.Value(aPos) >= '0' && aStr.Value(aPos) <= '9' ) ) )
    aPos++;

  TCollection_AsciiString aTypePrefix;
  if ( aPos <= aStr.Length() )
  {
    aTypePrefix = aStr.SubString(aPos, aStr.Length());
    aStr.Trunc(aPos - 1);
  }

  int aBasePrecision = 0;

  // taking width and precision
  TCollection_AsciiString aPrecision;

  aPos = aStr.Search( "." );
  if ( aPos >= 0 ) 
  {
    // aPrecision is defined
    aPrecision = aStr.Split( aPos );
    aStr.Remove( aStr.Length() );
    if ( !aPrecision.IsEmpty() && aPrecision.IsIntegerValue() )
    {
      aBasePrecision = aPrecision.IntegerValue();
    }
  }

  return aBasePrecision;
}
