// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef DDS_DICITEM_H
#define DDS_DICITEM_H

#include "DDS.h"

// OCCT includes
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_MapOfReal.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DefineIndexedDataMap.hxx>
#include <NCollection_Sequence.hxx>

class LDOM_Element;
class DDS_Dictionary;

//  Class, containing all information about one parameter:
//  unique    : id
//  obligative: label, type, short description, required
//  optional  : format, units,
//              min value, max value, default value.

class DDS_DicItem : public Standard_Transient
{
public:
  /*! Enum describes type of datum value */
  enum Type
  {
    String,  //!< String type of value
    Float,   //!< Real (double) numeric type of value
    Integer, //!< Integer (int) numeric type of value
    List,    //!< List type of enumerable value
    Unknown  //!< Unknown or undefined type of value
  };

  /*! Enum describes the flags for existance of domain data */
  enum Data
  {
    MinValue = 0x01,    //!< Flag of minimum value definition existence
    MaxValue = 0x02,    //!< Flag of maximum value definition existence
    DefaultValue = 0x04 //!< Flag of default value definition existence
  };

  // This struct is intended for map of Format, Units, Precision and Scale
  struct UnitData
  {
    double           myZero;
    double           myScale;
    TCollection_AsciiString myUnits;
    TCollection_AsciiString myFormat;
    int        myPrecision;
  };

  typedef TCollection_AsciiString UnitSystem;

public:
  Standard_EXPORT DDS_DicItem();

  Standard_EXPORT TCollection_AsciiString            GetId() const;
  Standard_EXPORT DDS_DicItem::Type                  GetType() const;
  Standard_EXPORT TCollection_ExtendedString         GetLabel() const;
  Standard_EXPORT TCollection_ExtendedString         GetFilter() const;
  Standard_EXPORT TCollection_ExtendedString         GetRequired() const;
  Standard_EXPORT DDS_MsgType                        GetWarningLevel() const;
  Standard_EXPORT TCollection_ExtendedString         GetLongDescription() const;
  Standard_EXPORT TCollection_ExtendedString         GetShortDescription() const;
  Standard_EXPORT TCollection_AsciiString            GetComponent() const;

  Standard_EXPORT virtual TCollection_AsciiString    GetUnits() const;
  Standard_EXPORT virtual TCollection_AsciiString    GetUnits( const UnitSystem& ) const;

  Standard_EXPORT TCollection_ExtendedString         GetDefaultValue() const;
  Standard_EXPORT TCollection_ExtendedString         GetDefaultValue( const UnitSystem& ) const;

  Standard_EXPORT double                      GetMinValue() const;
  Standard_EXPORT double                      GetMinValue( const UnitSystem& ) const;

  Standard_EXPORT double                      GetMaxValue() const;
  Standard_EXPORT double                      GetMaxValue( const UnitSystem& ) const;

  Standard_EXPORT virtual int           GetPrecision() const;
  Standard_EXPORT virtual int           GetPrecision( const UnitSystem& ) const;

  Standard_EXPORT virtual TCollection_AsciiString    GetFormat( const bool = true ) const;
  Standard_EXPORT virtual TCollection_AsciiString    GetFormat( const UnitSystem&,
                                                                const bool = true ) const;
  Standard_EXPORT TCollection_ExtendedString         GetNameOfValues() const;
  Standard_EXPORT bool                   GetListOfValues( Handle(TColStd_HArray1OfExtendedString)&,
                                                                      Handle(TColStd_HArray1OfInteger)& ) const;
  Standard_EXPORT bool                   GetListOfValues( Handle(TColStd_HArray1OfExtendedString)&,
                                                                      Handle(TColStd_HArray1OfInteger)&,
                                                                      Handle(TColStd_HArray1OfExtendedString)& ) const;
  Standard_EXPORT bool                   GetSpecialValues( TColStd_MapOfReal& ) const;

  Standard_EXPORT double                      GetMinZoom() const;
  Standard_EXPORT double                      GetMaxZoom() const;
  Standard_EXPORT double                      GetZoomOrder() const;

  Standard_EXPORT virtual double              ToSI( const double ) const;
  Standard_EXPORT virtual double              FromSI( const double ) const;

  Standard_EXPORT virtual double              ToSI( const double, const UnitSystem& ) const;
  Standard_EXPORT virtual double              FromSI( const double, const UnitSystem& ) const;

  Standard_EXPORT bool                   HasData( const int ) const;

  Standard_EXPORT TCollection_ExtendedString         GetOption( const TCollection_AsciiString& ) const;
  Standard_EXPORT bool                   GetOptionNames( TColStd_SequenceOfAsciiString& ) const;

protected:
  
  Standard_EXPORT void                               SetId( const TCollection_AsciiString& );

  Standard_EXPORT void                               SetComponent( const Handle(Standard_Transient)& );

  Standard_EXPORT void                               SetLabel( const TCollection_AsciiString& );

  Standard_EXPORT void                               SetFilter( const TCollection_AsciiString& );

  Standard_EXPORT void                               SetRequired( const TCollection_AsciiString& );

  Standard_EXPORT void                               SetWarningLevel( const int& );

  Standard_EXPORT void                               SetMinZoom( const double& );
  Standard_EXPORT void                               SetMaxZoom( const double& );
  Standard_EXPORT void                               SetZoomOrder( const double& );

  Standard_EXPORT void                               SetShortDescription( const TCollection_ExtendedString& );
  Standard_EXPORT void                               SetLongDescription( const TCollection_ExtendedString& );

  Standard_EXPORT bool                               SetOption( const TCollection_AsciiString&, const TCollection_AsciiString& );
  Standard_EXPORT void                               SetType( const DDS_DicItem::Type& );

  Standard_EXPORT void                               SetMin( const double& );
  Standard_EXPORT void                               SetMax( const double& );

  Standard_EXPORT void                               SetDefaultValue( const double& );
  Standard_EXPORT void                               SetDefaultValue( const TCollection_AsciiString& );

  Standard_EXPORT void                               SetListOfValues( const Handle(TColStd_HArray1OfExtendedString)&,
                                                                      const Handle(TColStd_HArray1OfInteger)& );
  Standard_EXPORT void                               SetListOfValues( const Handle(TColStd_HArray1OfExtendedString)&,
                                                                      const Handle(TColStd_HArray1OfInteger)&,
                                                                      const Handle(TColStd_HArray1OfExtendedString)& );

protected:

  Standard_EXPORT DDS_DicItem( const DDS_DicItem& );

  Standard_EXPORT void                               operator=( const DDS_DicItem& );

  Standard_EXPORT virtual void                       FillDataMap( TCollection_AsciiString, const LDOM_Element&,
                                                                  const LDOM_Element&, const LDOM_Element&,
                                                                  const TColStd_SequenceOfAsciiString& );

private:

  Standard_EXPORT void                               PrepareFormats( const TCollection_AsciiString& );
  Standard_EXPORT void                               GetDefaultFormat();
  Standard_EXPORT UnitSystem                         GetActiveUnitSystem() const;

  Standard_EXPORT void                               GetStringFormat( const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString&,
                                                                      TCollection_AsciiString& );
  Standard_EXPORT void                               GetIntegerFormat( const TCollection_AsciiString&,
                                                                       const TCollection_AsciiString&,
                                                                       const TCollection_AsciiString&,
                                                                       const TCollection_AsciiString&,
                                                                       const Standard_Character,
                                                                       TCollection_AsciiString& );
  Standard_EXPORT void                               GetFloatFormat( const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString&,
                                                                      const Standard_Character,
                                                                      TCollection_AsciiString& );
  Standard_EXPORT void                               Split( const TCollection_AsciiString&,
                                                            Handle(TColStd_HArray1OfExtendedString)& );

  Standard_EXPORT UnitData*                          GetUnitData( const UnitSystem& ) const;

private:
  typedef NCollection_DataMap<TCollection_AsciiString, TCollection_ExtendedString> OptionsMap;

private:
  TCollection_AsciiString                    myId;
  TCollection_ExtendedString                 myLabel;
  TCollection_ExtendedString                 myFilter;
  TCollection_ExtendedString                 myRequired;

  int                                        myType;
  int                                        myWarnLevel;

  TCollection_ExtendedString                 myLongDescr;
  TCollection_ExtendedString                 myShortDescr;

  double                                     myMax;
  double                                     myMin;
  double                                     myDefValue;
  TCollection_ExtendedString                 myDefString;

  int                                        myData;

  // valueList
  TCollection_ExtendedString                 myListName;

  Handle(TColStd_HArray1OfExtendedString)    myListRef;
  Handle(TColStd_HArray1OfInteger)           myListRefID;
  Handle(TColStd_HArray1OfExtendedString)    myListRefIcons;

  // presentation
  double                                     myMinZoom;
  double                                     myMaxZoom;
  double                                     myZoomOrder;

  Handle(Standard_Transient)                 myComponent;

  OptionsMap                                 myOptions;
  // unitData
  NCollection_DataMap<UnitSystem, UnitData>  myUnitData;

  friend class DDS_DicGroup;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(DDS_DicItem, Standard_Transient)
};

typedef NCollection_Sequence<Handle(DDS_DicItem)> DDS_BaseCollectionOfDicItems;

DEFINE_INDEXEDDATAMAP(DDS_IndexedDataMapOfDicItems, DDS_BaseCollectionOfDicItems,
                      TCollection_AsciiString, Handle(DDS_DicItem))

#endif
