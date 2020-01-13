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

// asiAlgo includes
#include <asiAlgo_DictionaryGroup.h>
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_DictionaryItem.h>

// OCCT includes
#include <LDOM_Node.hxx>
#include <LDOM_Text.hxx>
#include <LDOM_Element.hxx>
#include <LDOM_NodeList.hxx>

//-----------------------------------------------------------------------------

void asiAlgo_DictionaryGroup::FillDataMap(const LDOM_Element& groupElt,
                                          const LDOM_Element& dicElt)
{
  // Check for datums existence.
  LDOM_NodeList datums =
    dicElt.getElementsByTagName( DDS_Dictionary::KeyWord( "DATUM" ) );
  //
  if ( !datums.getLength() )
    return;

  // Read value lists.
  LDOM_NodeList lists =
    dicElt.getElementsByTagName( DDS_Dictionary::KeyWord( "VALUE_LIST" ) );

  // Load available lists.
  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HArray1OfInteger)> listSeqIds;
  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HArray1OfExtendedString)> listSeqVals;
  //
  for ( int id = 0; id < lists.getLength(); ++id )
  {
    LDOM_Node     listNode = lists.item(id);
    LDOM_Element& listElt  = (LDOM_Element&) listNode;

    // Read list identifier.
    TCollection_AsciiString listID =
      listElt.getAttribute( DDS_Dictionary::KeyWord( "VALUE_LIST_ID" ) );

    NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> valueMap;

    // Get all list values.
    LDOM_NodeList listValueItems =
      listElt.getElementsByTagName(DDS_Dictionary::KeyWord( "VALUE_LIST_VALUE" ));
    //
    Handle(TColStd_HArray1OfInteger)        seqOfIDs;
    Handle(TColStd_HArray1OfExtendedString) seqOfVals;
    //
    int listLength = listValueItems.getLength();
    if ( listLength > 0 )
    {
      seqOfIDs  = new TColStd_HArray1OfInteger(1, listLength);
      seqOfVals = new TColStd_HArray1OfExtendedString(1, listLength);
    }

    for ( int valID = 1; valID <= listLength; valID++ )
    {
      LDOM_Node     listItemNode = listValueItems.item(valID - 1);
      LDOM_Element& listItemElt  = (LDOM_Element&) listItemNode;

      // Read value ID.
      TCollection_AsciiString listValueID =
        listItemElt.getAttribute(DDS_Dictionary::KeyWord( "VALUE_LIST_VALUE_ID" ));
      //
      if ( listValueID.IsIntegerValue() )
      {
        //  Read the text in the element "value".
        LDOM_Node        node = listItemElt.getFirstChild();
        const LDOM_Text& text = (const LDOM_Text&) node;

        LDOM_Text listItemTxt(text);
        //
        if ( !listItemTxt.isNull() )
        {
          // Add ID and text value to sequence.
          seqOfIDs->ChangeValue(valID)  = listValueID.IntegerValue();
          seqOfVals->ChangeValue(valID) = listItemTxt.getData();
        }
      }
    }

    listSeqIds.Bind(listID, seqOfIDs);
    listSeqVals.Bind(listID, seqOfVals);
  }

  // Read dimensions.
  m_DimensionMap.Clear();
  //
  LDOM_Element dimElt =
    dicElt.GetChildByTagName(DDS_Dictionary::KeyWord( "DIMENSION" ));
  //
  while ( !dimElt.isNull() )
  {
    TCollection_AsciiString dimId =
      dimElt.getAttribute(DDS_Dictionary::KeyWord( "DIMENSION_NAME" ));

    if ( dimId.IsEmpty() )
      continue;

    Handle(asiAlgo_DictionaryDimension)
      dimension = new asiAlgo_DictionaryDimension(dimId);

    dimension->FillDataMap(dimElt);

    m_DimensionMap.Add(dimId, dimension);

    dimElt = dimElt.GetSiblingByTagName();
  }

  // Read datum items.
  for ( int id = 0; id < datums.getLength(); ++id )
  {
    LDOM_Node     datumNode = datums.item(id);
    LDOM_Element& datumElt  = (LDOM_Element&) datumNode;

    // read id and parse datum item
    TCollection_AsciiString itemID =
      datumElt.getAttribute( DDS_Dictionary::KeyWord( "DATUM_ID" ) );

    Handle(asiAlgo_DictionaryItem) datum = new asiAlgo_DictionaryItem(itemID, this);
    //
    if ( this->AddDicItem(itemID, datum) )
    {
      // Fill dictionary attributes.
      datum->FillDataMap( itemID, datumElt, groupElt,
                          dicElt, TColStd_SequenceOfAsciiString() );

      // fill dictionary item's list.
      TCollection_AsciiString listRefID = datum->GetListRefID();
      //
      if ( listSeqIds.IsBound(listRefID) )
      {
        Handle(TColStd_HArray1OfInteger)        seqOfIDs  = listSeqIds.Find(listRefID);
        Handle(TColStd_HArray1OfExtendedString) seqOfVals = listSeqVals.Find(listRefID);

        datum->SetListOfValues(seqOfVals, seqOfIDs);

        TCollection_AsciiString defStr = datum->GetDefaultValue();
        //
        if ( defStr == "" && seqOfIDs->Length() > 0 )
          datum->SetDefaultValue( seqOfVals->Value( seqOfVals->Lower() ) );
      }
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_DictionaryGroup::GetDimensions(asiAlgo_DictionaryDimensions& dimensions) const
{
  for ( DimensionMap::Iterator mit(m_DimensionMap); mit.More(); mit.Next() )
    dimensions.push_back( mit.Value() );
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_DictionaryDimension) 
  asiAlgo_DictionaryGroup::GetDimension(const TCollection_AsciiString& theName) const
{
  if ( !m_DimensionMap.Contains(theName) )
    return Handle(asiAlgo_DictionaryDimension)();

  return m_DimensionMap.FindFromKey(theName);
}
