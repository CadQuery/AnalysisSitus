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

#include "DDS_KeyWords.h"

/*!
  \class DDS_KeyWords
  \brief This class defines the pairs of internal keys and XML tags or 
  attributes names. 

  This class allows user to define a set of XML keywords used in the datums
  definition (XML) file.

  Default keywords table:
  - \c D_URI - dictionary
  - \c COMPONENT - component
  - \c COMPONENT_NAME - name
  - \c UNIT_SYSTEMS - unitSystems
  - \c UNIT_SYSTEM - system
  - \c UNIT_SYSTEM_NAME - name
  - \c UNIT_SYSTEM_LABEL - label
  - \c DATUM - datum
  - \c DATUM_ID - id
  - \c DATUM_LABEL - label
  - \c DATUM_UNITS - units
  - \c DATUM_FORMAT - format
  - \c DATUM_FILTER - filter
  - \c DATUM_REQUIRED - required
  - \c VALUE_LIST - valueList
  - \c VALUE_LIST_ID - listid
  - \c VALUE_LIST_NAME - name
  - \c VALUE_LIST_TYPE - type
  - \c VALUE_LIST_VALUE - value
  - \c VALUE_LIST_VALUEID - id
  - \c VALUE_LIST_VALUEICON - icon
  - \c DY_DOMAIN - domain
  - \c WARNING_LEVEL - warningLevel
  - \c WRONG_VALUE - wrongValue
  - \c VALUE_DESCR - valueDescr
  - \c VALUE_LIST_REF - listRef
  - \c DESCR - description
  - \c LONG_D - longDescr
  - \c SHORT_D - shortDescr
  - \c VD_TYPE - type
  - \c VD_DEFV - default
  - \c VD_MAXV - max
  - \c VD_MINV - min
  - \c VD_SPEC - specVal
  - \c VLR_LIST - list
  - \c PRS - presentation
  - \c LATERAL_ZOOM - lateralZoom
  - \c LZ_MINV - min
  - \c LZ_MAXV - max
  - \c LZ_ORDER - order
  - \c OPTIONS - options
  - \c OPTION - option
  - \c OPTION_NAME - name

  There is the only instance of the class DDS_KeyWords in the application
  which can be retrieved by method Get().
*/

/*!
  \brief Constructor.
  Create an instance of the object. Can not be used directly.
  Use Get() method instead.
*/
DDS_KeyWords::DDS_KeyWords() : Standard_Transient()
{
  SetKeyWord( "D_URI",                "dictionary" );

  SetKeyWord( "COMPONENT",            "component" );
  SetKeyWord( "COMPONENT_NAME",       "name" );

  SetKeyWord( "UNIT_SYSTEMS",         "unitSystems" );
  SetKeyWord( "UNIT_SYSTEM",          "system" );
  SetKeyWord( "UNIT_SYSTEM_NAME",     "name" );
  SetKeyWord( "UNIT_SYSTEM_LABEL",    "label" );

  SetKeyWord( "DATUM",                "datum" );
  SetKeyWord( "DATUM_ID",             "id" );
  SetKeyWord( "DATUM_LABEL",          "label" );
  SetKeyWord( "DATUM_UNITS",          "units" );
  SetKeyWord( "DATUM_FORMAT",         "format" );
  SetKeyWord( "DATUM_FILTER",         "filter" );
  SetKeyWord( "DATUM_REQUIRED",       "required" );

  SetKeyWord( "VALUE_LIST",           "valueList" );
  SetKeyWord( "VALUE_LIST_ID",        "listid" );
  SetKeyWord( "VALUE_LIST_NAME",      "name" );
  SetKeyWord( "VALUE_LIST_TYPE",      "type" );
  SetKeyWord( "VALUE_LIST_VALUE",     "value" );
  SetKeyWord( "VALUE_LIST_VALUEID",   "id" );
  SetKeyWord( "VALUE_LIST_VALUEICON", "icon" );

  SetKeyWord( "DY_DOMAIN",            "domain" );
  SetKeyWord( "WARNING_LEVEL",        "warningLevel" );
  SetKeyWord( "WRONG_VALUE",          "wrongValue" );
  SetKeyWord( "VALUE_DESCR",          "valueDescr" );
  SetKeyWord( "VALUE_LIST_REF",       "listRef" );

  SetKeyWord( "DESCR",                "description" );
  SetKeyWord( "LONG_D",               "longDescr" );
  SetKeyWord( "SHORT_D",              "shortDescr" );

  SetKeyWord( "VD_TYPE",              "type" );
  SetKeyWord( "VD_DEFV",              "default" );
  SetKeyWord( "VD_MAXV",              "max" );
  SetKeyWord( "VD_MINV",              "min" );
  SetKeyWord( "VD_SPEC",              "specVal" );
  SetKeyWord( "VLR_LIST",             "list" );
  SetKeyWord( "PRS",                  "presentation" );
  SetKeyWord( "LATERAL_ZOOM",         "lateralZoom" );
  SetKeyWord( "LZ_MINV",              "min" );
  SetKeyWord( "LZ_MAXV",              "max" );
  SetKeyWord( "LZ_ORDER",             "order" );

  SetKeyWord( "OPTIONS",              "options" );
  SetKeyWord( "OPTION",               "option" );
  SetKeyWord( "OPTION_NAME",          "name" );
}

/*!
  \brief Get the only instance of the class DDS_KeyWords.
  \return the only instance of the class
*/
Handle(DDS_KeyWords) DDS_KeyWords::Get()
{
  static Handle(DDS_KeyWords) keyWords;

  if ( keyWords.IsNull() )
    keyWords = new DDS_KeyWords();

  return keyWords;
}

/*!
  \brief Get the XML keyword by specified internal key \a key.

  If the internal key is not registered, empty string is returned.
  \param key internal key
  \return XML keyword 
*/
TCollection_AsciiString DDS_KeyWords::GetKeyWord( const TCollection_AsciiString& key ) const
{
  TCollection_AsciiString keyWord;
  if ( myKeyWord.IsBound( key ) )
    keyWord = myKeyWord.Find( key );
  return keyWord;
}

/*!
  \brief Register the pair of the internal key \a key and XML keyword \a keyWord.
  \param key internal key
  \param keyWord XML keyword
*/
void DDS_KeyWords::SetKeyWord( const TCollection_AsciiString& key,
                               const TCollection_AsciiString& keyWord )
{
  if ( myKeyWord.IsBound( key ) )
    myKeyWord.UnBind( key );

  myKeyWord.Bind( key, keyWord );
}
