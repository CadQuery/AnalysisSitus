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

#ifndef asiAlgo_DictionaryItem_HeaderFile
#define asiAlgo_DictionaryItem_HeaderFile

// asiAlgo includes
#include <asiAlgo_DictionaryGroup.h>

// Active Data includes
#include <ActAux_Common.h>

// SUIT includes
#include <DDS_DicItem.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

class asiAlgo_DictionaryUnitSystem;
class asiAlgo_DictionaryGroup;

typedef NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> asiAlgo_ParameterMap;

#pragma warning(disable: 4263)

//-----------------------------------------------------------------------------

//! Datum Dictionary item.
//! The items provides datum definition, including:
//! - dimensions;
//! - minimum and maximum value;
//! - value type;
//! - label, description;
//! - value string format;
//! - widget type for editing.
class asiAlgo_DictionaryItem : public DDS_DicItem
{
public:

  //! User defined editor widget type. The type
  //! is used for selecting one of the allowed
  //! item editors in GUI Layer.
  enum WidgetType
  {
    WT_LineEdit,
    WT_SpinBox,
    WT_ComboBox,
    WT_OpenFilePath,
    WT_SaveFilePath,
    WT_DirPath,
    WT_CheckBox,
    WT_DatePicker,
    WT_ColorPicker,
    WT_Selector,
    WT_Label,
    WT_CustomSelector,
    WT_Unknown
  };

  //! Value type. The type is used
  //! for selecting appropriate editor
  //! in GUI layer.
  enum DataType
  {
    Unknown,
    Integer,
    Float,
    String,
    List,
    Bool
  };

protected:

  // helper structure to hold dimension power values
  struct Dimension
  {
    Dimension()
    {
      m_iFirstPower  = 0;
      m_iSecondPower = 0;
    }

    Dimension(const TCollection_AsciiString& thePowerString)
    {
      m_PowerString = thePowerString;
      
      TCollection_AsciiString aPower1 = thePowerString.Token(" ", 1);
      TCollection_AsciiString aPower2 = thePowerString.Token(" ", 2);

      m_iFirstPower  = aPower1.IsIntegerValue() ? aPower1.IntegerValue() : 0;
      m_iSecondPower = aPower2.IsIntegerValue() ? aPower2.IntegerValue() : 0;
    }

    TCollection_AsciiString GetPowerString() const
    {
      return m_PowerString;
    }

    int GetFirstPower() const
    {
      return m_iFirstPower;
    }

    int GetSecondPower() const
    {
      return m_iSecondPower;
    }

    int GetSummaryPower() const
    {
      return m_iFirstPower + m_iSecondPower;
    }

  private:

    TCollection_AsciiString m_PowerString;
    int        m_iFirstPower;
    int        m_iSecondPower;
  };

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DictionaryItem, DDS_DicItem)

public:

  asiAlgo_EXPORT asiAlgo_DictionaryItem(const TCollection_AsciiString&         theId,
                                        const Handle(asiAlgo_DictionaryGroup)& theGroup);

// editor related data
public:

  //! Get editor widget type.
  //! \return widget type.
  asiAlgo_EXPORT
    WidgetType GetWidgetType() const
    {
      return m_WidgetType;
    }

  //! Get map of widget parameters.
  asiAlgo_EXPORT
    const asiAlgo_ParameterMap& GetWidgetParams() const
    {
      return m_WidgetParams;
    }

  //! Get data type for dictionary item. 
  //! The type is used for selecting
  //! appropriate editor in GUI
  //! layer.
  asiAlgo_EXPORT
    DataType GetDataType() const
    { 
      return m_DataType;
    }

  //! Get format string. The format string describes 
  //! conversion rules for string representation
  //! of the item's data.
  asiAlgo_EXPORT
    TCollection_AsciiString GetValueFormat() const
    {
      return m_Format;
    }

  //! Get reference identifier of list item 
  //! associated with the datum.
  //! \return list item id string.
  asiAlgo_EXPORT
    TCollection_AsciiString GetListRefID() const
    {
      return m_ListRef;
    }

  asiAlgo_EXPORT virtual int
    GetPrecision(const UnitSystem& theUnitSystem) const;

// Dictionary specific methods
public:

  asiAlgo_EXPORT void OverrideListOfValues(const Handle(HStringArray)& ARR,
                                           const Handle(HIntArray)& INDX);

  asiAlgo_EXPORT virtual void
    FillDataMap(TCollection_AsciiString              theID, 
                const LDOM_Element&                  theDatumElt,
                const LDOM_Element&                  theGroupElt,
                const LDOM_Element&                  theDicElt,
                const TColStd_SequenceOfAsciiString& theUnitSystems);

  asiAlgo_EXPORT virtual TCollection_AsciiString
    GetFormat(const UnitSystem&      theUnitSystem,
              const bool theIsCanonical = true) const;

// unit conversion and unit string generation methods
public:

  asiAlgo_EXPORT virtual double
    ToSI(const double theValue);

  asiAlgo_EXPORT virtual double
    ToSI(const double                 theValue,
         const asiAlgo_DictionaryUnitSystem& theSystem) const;

  asiAlgo_EXPORT virtual double
    FromSI(const double theValue);

  asiAlgo_EXPORT virtual double
    FromSI(const double                 theValue,
           const asiAlgo_DictionaryUnitSystem& theSystem) const;

  asiAlgo_EXPORT virtual TCollection_AsciiString
    GetUnits() const;

  asiAlgo_EXPORT virtual TCollection_AsciiString
    GetUnits(const asiAlgo_DictionaryUnitSystem& theSystem) const;

protected:

  asiAlgo_EXPORT virtual double
    ToSI(const double theValue,
         const UnitSystem&   theUnitSystem) const;

  asiAlgo_EXPORT virtual double
    FromSI(const double theValue,
           const UnitSystem&   theUnitSystem) const;

  asiAlgo_EXPORT virtual TCollection_AsciiString
    GetUnits(const UnitSystem& theUnitSystem) const;

  asiAlgo_EXPORT double
    getConversionFactor(const asiAlgo_DictionaryUnitSystem& theSystem) const;

protected:

  asiAlgo_EXPORT void
    parseWidgetParams(const LDOM_Element& theUI);

  //! Set data type for dictionary item. This type defines editor
  //! widget and data conversion rules.
  asiAlgo_EXPORT virtual void
    setDataType(const DataType theDataType);

protected:

  //! Set editor widget type.
  void setWidgetType(const WidgetType theWidgetType)
  {
    m_WidgetType = theWidgetType;
  }

  //! Set format string. The format string describes conversion
  //! rules for string representation of the item's data.
  //! It might be used in sprintf(...) function.
  //! \param theFormat [in] - the format value to be set.
  void setValueFormat(const TCollection_AsciiString& theFormat)
  {
    m_Format = theFormat;
  }

  //! Set list reference id. The id will be used for list dictionary
  //! item lookup.
  //! \param theRefId [in] - the reference list identifier string.
  void setListRefID(const TCollection_AsciiString& theRefID)
  {
    m_ListRef = theRefID;
  }
  
  //! Set precision used for float/double value representations.
  //! \param thePrecision [in] number of digits.
  void setPrecision(const int thePrecision)
  {
    m_iPrecision = thePrecision;
  }

  //! Get item's parent dictionary group.
  //! \return dictionary group reference.
  Handle(asiAlgo_DictionaryGroup) GetGroup() const
  {
    return m_Group;
  }

  friend class asiAlgo_DictionaryGroup;

  typedef NCollection_IndexedDataMap<TCollection_AsciiString, Dimension> DimensionMap;

private:
  
  int formatPrecision(const TCollection_AsciiString& theFormat) const;

private:

  //! Decimal precision.
  int m_iPrecision;

  //! Format string.
  TCollection_AsciiString m_Format;

  //! List reference.
  TCollection_AsciiString m_ListRef;

  //! Editor widget parameters.
  asiAlgo_ParameterMap m_WidgetParams;

  //! Editor widget type.
  WidgetType m_WidgetType;

  //! Data type for dictionary item.
  DataType m_DataType;

  //! Parent dictionary group.
  Handle(asiAlgo_DictionaryGroup) m_Group;

  //! dimension values.
  DimensionMap m_DimensionMap;
};

#pragma warning(default: 4263)

#endif
