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

#include "DDS_DicGroup.h"

#include "DDS_Dictionary.h"

// OCCT includes
#include <LDOMString.hxx>
#include <LDOM_Element.hxx>
#include <UnitsAPI.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

/*!
  \class DDS_DicGroup
  \brief This class provides a set of DDS_DicItem objects from one component.
*/

/*!
  \brief Constructor.

  Create the group with name \a name.

  \param name group name
*/
DDS_DicGroup::DDS_DicGroup( const TCollection_AsciiString& name )
: Standard_Transient(),
  myName( name ),
  myActiveSystem( UNIT_SYSTEM_SI )
{
}

/*!
  \brief Copy constructor (put in private section to prevent object copying).
*/
DDS_DicGroup::DDS_DicGroup( const DDS_DicGroup& )
{
}

/*!
  \brief Get the name of group (component).
  \return group name
*/
TCollection_AsciiString DDS_DicGroup::GetName() const
{
  return myName;
}

/*!
  \brief Get the names of all defined units systems.
  \param theSystemsSeq returning sequence of names
*/
void DDS_DicGroup::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystemSeq ) const
{
  theSystemSeq.Clear();
  for ( UnitSystemMap::Iterator it( myUnitSystem ); it.More(); it.Next() )
  {
    if ( it.Key() == TCollection_AsciiString( UNIT_SYSTEM_SI ) )
      theSystemSeq.Prepend( it.Key() );
    else
      theSystemSeq.Append( it.Key() );
  }
}

/*!
  \brief Get the label of units system \a name.
  
  If units system is not found, empty string is returned.

  \param make units system name
  \return units system label
*/
TCollection_ExtendedString DDS_DicGroup::GetUnitSystemLabel( const TCollection_AsciiString& name ) const
{
  TCollection_ExtendedString aLabel;
  if ( myUnitSystem.IsBound( name ) )
    aLabel = myUnitSystem.Find( name );
  return aLabel;
}

/*!
  \brief Get the name of active units system.
  \return active units system name
*/
TCollection_AsciiString DDS_DicGroup::GetActiveUnitSystem() const
{
  return myActiveSystem;
}

/*!
  \brief Set the active unit system.
  \param theSystem name of the units system to be made active
*/
void DDS_DicGroup::SetActiveUnitSystem( const TCollection_AsciiString& theSystem )
{
  if ( myUnitSystem.IsBound( theSystem ) )
    myActiveSystem = theSystem;
}

/*!
  \brief Assignment operator (put in private section to prevent object copying).
*/
void DDS_DicGroup::operator=( const DDS_DicGroup& )
{
}

/*!
  \brief Fill the internal data structures from XML parsed structures.
  \param theComponentData component data DOM node
  \param theDocElement document element DOM node
*/
void DDS_DicGroup::FillDataMap( const LDOM_Element& theComponentData, const LDOM_Element& theDocElement )
{
  TCollection_AsciiString aCompName = theComponentData.getAttribute( DDS_Dictionary::KeyWord( "COMPONENT_NAME" ) );

  LDOM_Element systems = theComponentData.GetChildByTagName( DDS_Dictionary::KeyWord( "UNIT_SYSTEMS" ) );
  if ( !systems.isNull() )
  {
    LDOM_NodeList systemList = systems.getElementsByTagName( DDS_Dictionary::KeyWord( "UNIT_SYSTEM" ) );
    for ( int i = 0; i < systemList.getLength(); i++ )
    {
      //const LDOM_Element& aSystem = (const LDOM_Element &)systemList.item( i );
      LDOM_Node aNode = systemList.item( i );
      const LDOM_Element& anElem = (const LDOM_Element&) aNode;
      LDOM_Element aSystem(anElem);
      TCollection_AsciiString aName = aSystem.getAttribute( DDS_Dictionary::KeyWord( "UNIT_SYSTEM_NAME" ) );
      TCollection_ExtendedString aLabel = aSystem.getAttribute( DDS_Dictionary::KeyWord( "UNIT_SYSTEM_LABEL" ) );

      if ( aName.IsEmpty() )
        continue;

      if ( !myUnitSystem.IsBound( aName ) )
        myUnitSystem.Bind( aName, aLabel );


    }
  }

  if ( !myUnitSystem.IsBound( UNIT_SYSTEM_SI ) )
  {
    printf( "Warning: Mandatory unit system SI not defined in component: \"%s\". Added automaticaly", aCompName.ToCString() );
    myUnitSystem.Bind( UNIT_SYSTEM_SI, TCollection_ExtendedString( "System international" ) );
  }

  TColStd_SequenceOfAsciiString unitSystems;
  GetUnitSystems( unitSystems );

  LDOM_NodeList aData = theComponentData.getElementsByTagName( DDS_Dictionary::KeyWord( "DATUM" ) );
  if ( !aData.getLength() )
    return;

  for ( int ii = 0; ii < aData.getLength(); ii++ )
  {
    //LDOM_Element aQuantity = (const LDOM_Element&)aData.item( i );
    LDOM_Node aNode = aData.item( ii );
    const LDOM_Element& anElem = (const LDOM_Element&) aNode;
    LDOM_Element aQuantity(anElem);

    // 1. Attributes (id,label,units?,format?,required?)
    TCollection_AsciiString anID = aQuantity.getAttribute( DDS_Dictionary::KeyWord( "DATUM_ID" ) );
    Handle(DDS_DicItem) aDicItem = CreateItem();

    aDicItem->myComponent = this;
    aDicItem->FillDataMap( anID, aQuantity, theComponentData, theDocElement, unitSystems );
    myDataMap.Add( anID, aDicItem );
    
    bool exist = false;
    for( int i=1, n=myKeys.Length(); i<=n && !exist; i++ )
      if( myKeys.Value( i )==anID )
      {
        std::cout << "Doubled key:" << anID << std::endl;
        exist = true;
      }
    if( !exist )
      myKeys.Append( anID );
  }
}

/*!
  \brief Get the dictionary item with specified identifier \a theID.

  If dictionary item is not found, null handle is returned.

  \param theID item identifier
  \return dictionary item
*/
Handle(DDS_DicItem) DDS_DicGroup::GetDicItem( const TCollection_AsciiString& theID ) const
{
  Handle(DDS_DicItem) aDicItem;
  // get dictionary item by id
  if ( myDataMap.Contains( theID ) )
    aDicItem = myDataMap.FindFromKey( theID );

  return aDicItem;
}

/*!
  \brief Return all keys of the group
  \param seq - string container to be filled with keys
*/
void DDS_DicGroup::GetKeys( TColStd_SequenceOfAsciiString& seq ) const
{
  seq = myKeys;
}

/*!
  \brief Instantiate new dictionary item, used for customization of
         data dictionary items.
  \return New dictionary item instance.
*/
Handle(DDS_DicItem) DDS_DicGroup::CreateItem() const
{
  return new DDS_DicItem();
}

/*!
  \brief Bind dictionary item to ID
  \return true if the item has been succesfully bound.
          false if there is an item with same id.
*/
bool DDS_DicGroup::AddDicItem( const TCollection_AsciiString& theID,
                                           const Handle(DDS_DicItem)& theDicItem )
{
  if ( myDataMap.Contains( theID ) )
    return false;

  myDataMap.Add( theID, theDicItem );
  myKeys.Append( theID );

  return true;
}

/*!
  \brief Check if there is item bounded by id.
  \return true if there is an item bound in map with given id.
*/
bool DDS_DicGroup::HasDicItem( const TCollection_AsciiString& theID ) const 
{
  return myDataMap.Contains( theID );
}

/*!
  \brief Clear dictionary items map
*/
void DDS_DicGroup::RemoveAllDicItems() 
{
  myDataMap.Clear();
  myKeys.Clear();
}

/*!
  \brief Returns a reference to a map. Can be used to iterate through
         dictionary items.
*/
const DDS_IndexedDataMapOfDicItems& DDS_DicGroup::GetItemMap() const
{
  return myDataMap;
}

/*!
  \brief Add new unit system and bind its label name.
  \return false if there is other system bound by this key.
*/
bool DDS_DicGroup::AddUnitSystem( const TCollection_AsciiString& theSystemKey,
                                              const TCollection_ExtendedString& theSystemLabel )
{
  return myUnitSystem.Bind( theSystemKey, theSystemLabel );
}
