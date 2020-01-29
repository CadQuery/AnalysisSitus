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

#ifndef DDS_DICTIONARY_H
#define DDS_DICTIONARY_H

#include "DDS_DicGroup.h"

// OCCT includes
#include <LDOMString.hxx>
#include <Standard_Transient.hxx>

class LDOM_Element;
class TCollection_AsciiString;

class DDS_Dictionary : public Standard_Transient
{
public:
  Standard_EXPORT static Handle(DDS_Dictionary)  Get();

  // Return instance of data dictionary. Create instance if it is null.

  Standard_EXPORT void                           GetKeys( const TCollection_AsciiString&, TColStd_SequenceOfAsciiString& ) const;

  Standard_EXPORT Handle(DDS_DicItem)            GetDicItem( const TCollection_AsciiString& ) const;
  Standard_EXPORT Handle(DDS_DicItem)            GetDicItem( const TCollection_AsciiString&,
                                                             const TCollection_AsciiString& ) const;

  Standard_EXPORT void                           GetUnitSystems( TColStd_SequenceOfAsciiString& ) const;
  Standard_EXPORT void                           GetUnitSystems( TColStd_SequenceOfAsciiString&,
                                                                 const TCollection_AsciiString& ) const;
  Standard_EXPORT TCollection_ExtendedString     GetUnitSystemLabel( const TCollection_AsciiString& ) const;
  Standard_EXPORT TCollection_ExtendedString     GetUnitSystemLabel( const TCollection_AsciiString&,
                                                                     const TCollection_AsciiString& ) const;
  Standard_EXPORT TCollection_AsciiString        GetActiveUnitSystem() const;
  Standard_EXPORT TCollection_AsciiString        GetActiveUnitSystem( const TCollection_AsciiString& ) const;
  Standard_EXPORT void                           SetActiveUnitSystem( const TCollection_AsciiString& );
  Standard_EXPORT void                           SetActiveUnitSystem( const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString& );


  static Standard_EXPORT bool        Load( const TCollection_AsciiString );

  static Standard_EXPORT double           ToSI( const double, const Standard_CString );
  static Standard_EXPORT double           FromSI( const double, const Standard_CString );

  static Standard_EXPORT LDOMString              KeyWord( const TCollection_AsciiString& );

protected:

  Standard_EXPORT virtual Handle(DDS_DicGroup)   CreateGroup( const TCollection_AsciiString& ) const;

protected:

  Standard_EXPORT DDS_Dictionary();

  Standard_EXPORT DDS_Dictionary( const DDS_Dictionary& );

  Standard_EXPORT void                           operator=( const DDS_Dictionary& );

  Standard_EXPORT virtual void                   FillDataMap( const LDOM_Element& );

  Standard_EXPORT static void                    SetDictionary(const Handle(DDS_Dictionary)& theDict);

protected:

  DDS_IndexedDataMapOfDicGroups                  myGroupMap;

private:

  static Handle(DDS_Dictionary)                  myDictionary;

public:
  DEFINE_STANDARD_RTTI_INLINE(DDS_Dictionary, Standard_Transient)
};

#endif
