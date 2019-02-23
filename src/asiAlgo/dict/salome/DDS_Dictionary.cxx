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

#include "DDS_Dictionary.h"
#include "DDS_KeyWords.h"

// OCCT includes
#include <LDOMString.hxx>
#include <LDOMParser.hxx>
#include <UnitsAPI.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

Handle(DDS_Dictionary) DDS_Dictionary::myDictionary = Handle(DDS_Dictionary)();

/*!
  \class DDS_Dictionary
  \brief This class provides an information about used datums, 
         reading them from XML file.

  There is the only instance of the class DDS_Dictionary in the application
  which can be retrieved by method Get().

  Datum is a set of parameters describing a phisical characteristic.
  These parameters are loaded from the XML file which has the following format:

  \verbatim
  <D_URI>
  <COMPONENT COMPONENT_NAME="component_name">
    <UNIT_SYSTEMS>
      <UNIT_SYSTEM UNIT_SYSTEM_NAME="[system_internal_name]" UNIT_SYSTEM_LABEL="[system_label]">
      <UNIT_SYSTEM UNIT_SYSTEM_NAME="[system_internal_name]" UNIT_SYSTEM_LABEL="[system_label]">
      ...
      <UNIT_SYSTEM UNIT_SYSTEM_NAME="[system_internal_name]" UNIT_SYSTEM_LABEL="[system_label]">
    </UNIT_SYSTEMS>

    <DATUM DATUM_UNITS="[base_system_internal_name]" DATUM_FORMAT="[sprintf_format_specification]"
           DATUM_ID="[datum_id]" DATUM_LABEL="[datum_label]" DATUM_REQUIRED="[requred_value]"
           DATUM_FILTER="[string_regular_expression]">
      <DY_DOMAIN>
        <VALUE_DESCR VD_DEFV="[default_value]" VD_MAXV="[max_value]" VD_MINV="[min_value]" VD_TYPE="[value_type]"/>
        <VALUE_LIST_REF VLR_LIST="[referenced_list_id]"/>
      </DY_DOMAIN>
      <DESCR>
        <SHORT_D>[brief_desription_text]</SHORT_D>
        <LONG_D>[full_description_text]</LONG_D>
      </DESCR>
      <OPTIONS>
        <OPTION OPTION_NAME="[option_name_1]">[option_value_1]</OPTION>
        <OPTION OPTION_NAME="[option_name_2]">[option_value_2]</OPTION>
        ...
        <OPTION OPTION_NAME="[option_name_n]">[option_value_n]</OPTION>
      </OPTIONS>
    </DATUM>

    ...

    <VALUE_LIST VALUE_LIST_ID="[list_id]" VALUE_LIST_NAME="[list_name]">
      <VALUE_LIST_VALUE VALUE_LIST_VALUEID="[list_element_id]" VALUE_LIST_VALUEICON="[list_element_icon_file]">[list_element_name]</VALUE_LIST_VALUE>
      <VALUE_LIST_VALUE VALUE_LIST_VALUEID="[list_element_id]" VALUE_LIST_VALUEICON="[list_element_icon_file]">[list_element_name]</VALUE_LIST_VALUE>
      ...
      <VALUE_LIST_VALUE VALUE_LIST_VALUEID="[list_element_id]" VALUE_LIST_VALUEICON="[list_element_icon_file]">[list_element_name]</VALUE_LIST_VALUE>
    </VALUE_LIST>

    ...

    </COMPONENT>
  </D_URI>

  \endverbatim

  In above description of the datum XML file format internal keys are used as XML tags
  and attributes names. Real XML keywords are defined by DDS_KeyWords class.

  XML file should have one main tag named "dictionary" (key "D_URI"). This tag
  should contain one or several components. 
  Component is an independent set of datums and units systems.
  Components are defined by XML tag named "component" (key "COMPONENT") with 
  attribute "name" (key COMPONENT_NAME).
  Component name is used as component identifier and should be unique. 
  
  Component tag can contain:

  - Tag "unit_systems" (key UNIT_SYSTEMS) defines a set of used units systems.
  At least one unit system named SI ("System International") should exist.
  If this system does not exist, it will be created automatically.
  Each units system is defined by XML tag "unit system" (key UNIT_SYSTEM) under 
  the tag "unit_systems" with attributes "name" (key UNIT_SYSTEM_NAME) 
  and "label" (key UNIT_SYSTEM_LABEL). Name is an identifier of the units system and
  label is its human readable description.

  - One or several tags "datum" (key DATUM). For this tag the following attributes 
  can be defined:
    -# Identifier (key DATUM_ID) specifies the unique id string for the datum.
    -# Label (key DATUM_LABEL) specifies human readable name of the datum.
    -# Measure units (key DATUM_UNITS) for the given units system. Attribute name 
    defines a name of units system and a keyword got from DDS_KeyWords by key DATUM_UNITS.
    For example, for "SI" units system and default keyword the attribute name is "SIunits".
    This attribute should be specified for each declared units system. 
    Value of this attribute should be a string describing measure units.
    For possible designations for measure units and their multiple prefixes
    please refer to the UnitsAPI package of the OpenCascade library
    (files Units.dat and Lexi_Expr.dat). Measure units are used for numerical
    values conversion from one units system to another one.
    -# Format (key DATUM_FORMAT) specifies the format string which will be used
    during initial formatting of the value. This string should be specified 
    in sprintf() format.
    -# Filter (key DATUM_FILTER) specifies the regualr expression. The value (string)
    entered by the user will be checked up to match to this regular expression
    (if it defined). Non matched strings will be rejected.
    -# Required value (key DATUM_REQUIRED). If this attributed si defined and its value
    is \c true then user can't leave an input non-filled - parameter must be explicitly
    entered by the user).

  - One or several tags "list definition" (key VALUE_LIST). Each such tag defines
  the list of items for enumerable data. Attribute "list id" (key VALUE_LIST_ID) 
  specifies the identifier string for the list and attribute "list name"
  (key VALUE_LIST_NAME) defines a list name string. Each list item is described
  by tag "list value" (key VALUE_LIST_VALUE) under the tag "list definition". 
  Each this tag contains item string text and have the following attributes:
    -# "list item id" (key VALUE_LIST_VALUEID) - integer numerical identifier for 
    the item
    -# "list item icon" (key VALUE_LIST_VALUEICON) - icon file name for the item

  Tag "datum" can have child subtags "description" and "options".

  - Tag "description" (key DESCR) contains two sub tags:
    -# "short description" (key SHORT_D) specifies a brief datum description text
    -# "long description" (key LONG_D) specifies a detailed description text

  - Tag "options" (key OPTIONS) contains one or more sub tags "option" (key OPTION).
  Each of these XML elements should contain text option value and attribute
  "name" (key OPTION_NAME) which specifies option name.

  Each tag "datum" defines most common parameters of phisical characteristic.
  These parameters are placed in two groups:
  -# Domain parameters under the tag "domain" (key DY_DOMAIN). This tag can 
  contain value description tag (key VALUE_DESCR) for descrete data which is
  described by following parameters:
    - default value (key VD_DEFV)
    - maximum value (key VD_MAXV)
    - minimum value (key VD_MINV)
    - type of value (key VD_TYPE), possible values are String, Integer, Float, List
  -# list reference tag (key VALUE_LIST_REF) for enumerable data described by
  "list reference" attribute (key VLR_LIST) which references to the list
  (see "list definition" tag) by list id.

  Below is an example of the XML file using default keywords.

  \verbatim
  <datadictionary version="1.0">
    <component name="My Component">

      <!-- specify two unit systems -->

      <unitSystems>
        <system name="SI" label="System international">
        <system name="AS" label="Anglo - sacson system">
      </unitSystems>

      <!-- specify datum -->
      <!-- units of measure for SI - meters (m), for AS - inches (in) -->
      <datum SIunits="m" ASunits="in" format="%.25f" id="X" label="X coordinate" required="">
        <domain>
          <!-- default value not specified -->
          <valueDescr default="" max="999999999.999" min="0.000" type="Float"/>
        </domain>
        <description>
          <shortDescr>X coordinate for object</shortDescr>
          <longDescr>X multiplier of object coordinates. Describe position of object in direction of X axis</longDescr>
        </description>
      </datum>

      <datum SIunits="m" ASunits="in" format="%.25f" id="Y" label="Y coordinate" required="">
        <domain>
          <valueDescr default="" max="999999999.999" min="0.000" type="Float"/>
        </domain>
        <description>
          <shortDescr>Y coordinate for object</shortDescr>
          <longDescr>Y multiplier of object coordinates. Describe position of object in direction of Y axis</longDescr>
        </description>
      </datum>

      <!-- datum for object name with filter which not allow to input more that 8 letters,
           numbers or unerscores with first letter only -->
      <datum format="%.8us" id="ObjectName" label="Name" required="yes"
             filter="^([A-Z]+)([A-Z,0-9,_]*)$">
        <domain>
          <!-- limits and default not specified, type is string -->
          <valueDescr default="" max="" min="" type="String" />
        </domain>
        <description>
          <!-- long description not specified -->
          <shortDescr>Name of object</shortDescr>
        </description>
      </datum>


      <!-- datum for enumeration of side -->
      <datum format="" id="Side" label="Side" required="">
        <domain>
          <!-- default list item is item with id 0 -->
          <valueDescr default="0" type="List"/>
          <!-- list reference on list named "side_list" -->
          <listRef list="side_list"/>
        </domain>
        <description>
          <shortDescr>Side of object</shortDescr>
        </description>
      </datum>

      <!-- list definition for enumeration of side -->
      <valueList listid="side_list" name="Side">
        <value id="1">left</value>
        <value id="2">right</value>
        <value id="3">top</value>
        <value id="4">bottom</value>
        <value id="0">undefined</value>
      </valueList>

    </component>
  </datadictionary>
  \endverbatim
*/

/*!
  \brief Constructor.

  Create an instance of the dictionary. Can not be used directly.
  Use Get() method instead.
*/
DDS_Dictionary::DDS_Dictionary() : Standard_Transient()
{}

/*!
  \brief Copy constructor (put in private section to prevent object copying).
*/
DDS_Dictionary::DDS_Dictionary( const DDS_Dictionary& )
{}

/*!
  \brief Assignment operator (put in private section to prevent object copying).
*/
void DDS_Dictionary::operator=( const DDS_Dictionary& )
{}

/*!
  \brief Get the names of defined units systems from all components.
  \param theSystems returning sequence of units systems names.
*/
void DDS_Dictionary::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystems ) const
{
  theSystems.Clear();

  NCollection_Map<TCollection_AsciiString> aMap;
  for ( int i = 1; i <= myGroupMap.Extent(); i++ )
  {
    TColStd_SequenceOfAsciiString theSeq;
    myGroupMap.FindFromIndex( i )->GetUnitSystems( theSeq );
    for ( int s = 1; s <= theSeq.Length(); s++ )
    {
      if ( aMap.Contains( theSeq.Value( s ) ) )
        continue;

      theSystems.Append( theSeq.Value( s ) );
      aMap.Add( theSeq.Value( s ) );
    }
  }

}

/*!
  \brief Get the names of defined units systems from the specified component
         \a theComponent.

  If component is not found, empty list is returned.

  \param theSystems returning sequence of units systems names.
  \param theComponent component name
*/
void DDS_Dictionary::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystems,
                                     const TCollection_AsciiString& theComponent ) const
{
  theSystems.Clear();
  if ( myGroupMap.Contains( theComponent ) )
    myGroupMap.FindFromKey( theComponent )->GetUnitSystems( theSystems );
}

/*!
  \brief Get the label of the units system \a theSystem.

  Searches the given units system in all components. If units system is not found
  in any component, empty string is returned.

  \param theSystem units system
  \return units system label
*/
TCollection_ExtendedString DDS_Dictionary::GetUnitSystemLabel( const TCollection_AsciiString& theSystem ) const
{
  TCollection_ExtendedString aLabel;
  for ( int i = 1; i <= myGroupMap.Extent() && !aLabel.Length(); i++ )
    aLabel = myGroupMap.FindFromIndex( i )->GetUnitSystemLabel( theSystem );
  return aLabel;
}

/*!
  \brief Get the label of the units system \a theSystem from the
         component \a theComponent.

  Searches the specified units system in the specified component only.
  If units system is not found, empty string is returned.

  \param theSystem units system
  \param theComponent component name
  \return units system label
*/
TCollection_ExtendedString DDS_Dictionary::GetUnitSystemLabel( const TCollection_AsciiString& theSystem,
                                                               const TCollection_AsciiString& theComponent ) const
{
  TCollection_ExtendedString aLabel;
  if ( myGroupMap.Contains( theComponent ) )
    aLabel = myGroupMap.FindFromKey( theComponent )->GetUnitSystemLabel( theSystem );
  return aLabel;
}

/*!
  \brief Get the name of active units system from the first found component. 
  
  If at least one component exists, then its active units system name 
  is returned. Otherwise, empty string is returned.

  \return active units system name
*/
TCollection_AsciiString DDS_Dictionary::GetActiveUnitSystem() const
{
  TCollection_AsciiString aSystem;
  if ( myGroupMap.Extent() )
    aSystem = myGroupMap.FindFromIndex( 1 )->GetActiveUnitSystem();
  return aSystem;
}

/*!
  \brief Get the name of active units system from the component \a theComponent.

  If this component exists, its active units system name is returned.
  Otherwise, empty string is returned.

  \param theComponent component name
  \return active units system name
*/
TCollection_AsciiString DDS_Dictionary::GetActiveUnitSystem( const TCollection_AsciiString& theComponent ) const
{
  TCollection_AsciiString aSystem;
  if ( myGroupMap.Contains( theComponent ) )
    aSystem = myGroupMap.FindFromKey( theComponent )->GetActiveUnitSystem();
  return aSystem;
}

/*!
  \brief Set the active units system. 

  This units system will be activated in each existing component, 
  if it component has this units system.

  \param theSystem units system to be made active
*/
void DDS_Dictionary::SetActiveUnitSystem( const TCollection_AsciiString& theSystem )
{
  for ( int i = 1; i <= myGroupMap.Extent(); i++ )
    myGroupMap.FindFromIndex( i )->SetActiveUnitSystem( theSystem );
}

/*!
  \brief Set the active units system for the component \a theComponent. 

  If specified units system doesn't exist in the component, nothing happens.

  \param theSystem units system to be made active
  \param theComponent component name
*/
void DDS_Dictionary::SetActiveUnitSystem( const TCollection_AsciiString& theSystem,
                                          const TCollection_AsciiString& theComponent )
{
  if ( myGroupMap.Contains( theComponent ) )
    myGroupMap.FindFromKey( theComponent )->SetActiveUnitSystem( theSystem );
}

/*!
  \brief Get the only instance of the data dictionary.
  \return the only instance of the data dictionary
*/
Handle(DDS_Dictionary) DDS_Dictionary::Get()
{
  if ( myDictionary.IsNull() )
    myDictionary = new DDS_Dictionary();

  return myDictionary;
}

/*!
  \brief Load datum definitions in the dictionary from the XML file
  \a theFileName.
  \param theFileName XML file name
  \return \c true if loading is succeded or \c false otherwise.
*/
bool DDS_Dictionary::Load( const TCollection_AsciiString theFileName )
{
  static NCollection_Map<TCollection_AsciiString> _LoadMap;

  if ( _LoadMap.Contains( theFileName ) )
    return true;

  Handle(DDS_Dictionary) aDic = Get();
  if ( aDic.IsNull() )
    return false;

  LDOMParser aParser;
  if ( aParser.parse( theFileName.ToCString() ) )
    return false;

  LDOM_Document aDoc = aParser.getDocument();
  LDOM_Element aDocElement = aDoc.getDocumentElement();
  if ( !aDocElement.isNull() )
    aDic->FillDataMap( aDocElement );

  _LoadMap.Add( theFileName );

  return true;
}

/*!
  \brief Get XML keyword as LDOMString by specified \a key.

  If key doesn't exist, empty string is returned. 
  This function is provided for convenience.
  
  \param key keyword name
  \return keyword value
*/
LDOMString DDS_Dictionary::KeyWord( const TCollection_AsciiString& key )
{
  LDOMString keyWord;
  Handle(DDS_KeyWords) aKeyWords = DDS_KeyWords::Get();
  if ( !aKeyWords.IsNull() )
  {
    TCollection_AsciiString aStr = aKeyWords->GetKeyWord( key );
    if ( aStr.Length() )
      keyWord = LDOMString( aStr.ToCString() );
  }
  return keyWord;
}

/*!
  \brief Get the data dictionary item by specified identifier \a theID
  from the component \a theComponent.
  
  If the component or item is not found, null handle is returned.
  \param theID data dictionary item ID
  \param theComponent component name
  \return handle to the data dictionary item
*/
Handle(DDS_DicItem) DDS_Dictionary::GetDicItem( const TCollection_AsciiString& theID,
                                                const TCollection_AsciiString& theComponent ) const
{
  Handle(DDS_DicItem) aDicItem;
  Handle(DDS_DicGroup) aDicGroup;
  if ( myGroupMap.Contains( theComponent ) )
    aDicGroup = myGroupMap.FindFromKey( theComponent );
  if ( !aDicGroup.IsNull() )
    aDicItem = aDicGroup->GetDicItem( theID );
  return aDicItem;
}

/*!
  \brief Get the data dictionary item by specified identifier \a theID.

  Function searches the item in all components. If item is not found
  in all components, null handle is returned.

  \param theID data dictionary item ID
  \return handle to the data dictionary item
*/
Handle(DDS_DicItem) DDS_Dictionary::GetDicItem( const TCollection_AsciiString& theID ) const
{
  Handle(DDS_DicItem) aDicItem;
  for ( int i = 1; i <= myGroupMap.Extent() && aDicItem.IsNull(); i++ )
    aDicItem = myGroupMap.FindFromIndex( i )->GetDicItem( theID );
  return aDicItem;
}

/*!
  \brief Fill the internal data structures from the XML node.
  \param theDocElement document XML node
*/
void DDS_Dictionary::FillDataMap( const LDOM_Element& theDocElement )
{
  for ( LDOM_Element aComponentElem = theDocElement.GetChildByTagName( KeyWord( "COMPONENT" ) );
        !aComponentElem.isNull(); aComponentElem = aComponentElem.GetSiblingByTagName() )
  {
    TCollection_AsciiString aCompName = aComponentElem.getAttribute( KeyWord( "COMPONENT_NAME" ) );
    if ( !myGroupMap.Contains( aCompName ) )
      myGroupMap.Add( aCompName, CreateGroup( aCompName ) );
    Handle(DDS_DicGroup) aDicGroup = myGroupMap.FindFromKey( aCompName );
    aDicGroup->FillDataMap( aComponentElem, theDocElement );
    myGroupMap.Add( aCompName, aDicGroup );
  }
}

/*!
  \brief Convert numeric value \a theValue from specified measure units
  \a theUnits to "SI" measure units (mm for Length, radians for Angles, etc).
  \param theValue value being converted
  \param theUnits measure units
  \return converted value
*/
double DDS_Dictionary::ToSI( const double theValue, const Standard_CString theUnits )
{
  double aRetValue = theValue;
  if ( theUnits && *theUnits && strcmp( theUnits, "%" ) )
  {
    try {
#if OCC_VERSION_LARGE > 0x06010000
      OCC_CATCH_SIGNALS;
#endif
      aRetValue = UnitsAPI::AnyToSI( theValue, theUnits );
    }
    catch( Standard_Failure ) {
    }
  }
  else if ( theUnits && *theUnits )
    aRetValue = theValue / 100.0;

  return aRetValue;
}

/*!
  \brief Convert numeric value \a theValue to specified measure units
  \a theUnits from "SI" measure units (mm for Length, radians for Angles, etc).
  \param theValue value being converted
  \param theUnits measure units
  \return converted value
*/
double DDS_Dictionary::FromSI( const double theValue, const Standard_CString theUnits )
{
  double aRetValue = theValue;
  if ( theUnits && *theUnits && strcmp( theUnits, "%" ) )
  {
    try {
#if OCC_VERSION_LARGE > 0x06010000
      OCC_CATCH_SIGNALS;
#endif
      aRetValue = UnitsAPI::AnyFromSI( theValue, theUnits );
    }
    catch( Standard_Failure ) {
    }
  }
  else if ( theUnits && *theUnits )
    aRetValue = theValue * 100.0;

  return aRetValue;
}

/*!
  \brief Fill given string container \a seq with keys belonging to group with name \a theComponent
  \param theComponent name of group whose keys should be stored in the container
  \param seq returned string container with keys belonging to group; it is not cleared before filling
*/
void DDS_Dictionary::GetKeys( const TCollection_AsciiString& theComponent, TColStd_SequenceOfAsciiString& seq ) const
{
  Handle( DDS_DicGroup ) aDicGroup;
  if( myGroupMap.Contains( theComponent ) )
    aDicGroup = myGroupMap.FindFromKey( theComponent );

  if( !aDicGroup.IsNull() )
    aDicGroup->GetKeys( seq );
}

/*!
  \brief  Create instance of a dictionary group. This method can
          be used for customization data dictionary by specific
          groups design.
  \return New dictionary group instance.
*/
Handle(DDS_DicGroup) DDS_Dictionary::CreateGroup( const TCollection_AsciiString& theCompName ) const
{
  return new DDS_DicGroup( theCompName );
}

/*!
  \brief Set instance of dictionary. This method might be used in descendant classes to initialize
         custom dictionary and replace the default implementation.
         This method overrides static handle on dictionary that is available
         through Get() method
*/
void DDS_Dictionary::SetDictionary( const Handle(DDS_Dictionary)& theDict )
{
  myDictionary = theDict; 
}
