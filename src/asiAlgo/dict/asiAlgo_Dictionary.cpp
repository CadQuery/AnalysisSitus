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
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_DictionaryGroup.h>

// SUIT includes
#include <DDS_KeyWords.h>
#include <LDOM_Element.hxx>

// ----------------------------------------------------------------------------
//                      Dictionary Accessor
// ----------------------------------------------------------------------------

//! Load dictionary from XML file definition.
//! \param theFile [in] the XML file.
bool asiAlgo_Dictionary::Load(const TCollection_AsciiString& theFile)
{
  asiAlgo_DictionaryImpl::Init();
  return asiAlgo_DictionaryImpl::Load(theFile);
}

//! Get dictionary item by its id.
//! \param theId [in] dictionary item id.
//! \return handle to a dictionary item or a null handler if item is not found.
Handle(asiAlgo_DictionaryItem) 
  asiAlgo_Dictionary::GetDictionaryItem(const TCollection_AsciiString& theId)
{
  return Handle(asiAlgo_DictionaryItem)::
    DownCast(asiAlgo_DictionaryImpl::Get()->GetDicItem(theId));
}

//! Get all dictionary item ids.
//! \param theSeqOfIds [out] sequence of item ids.
void asiAlgo_Dictionary::GetItemIds(TColStd_SequenceOfAsciiString& theSeqOfIds)
{
  asiAlgo_DictionaryImpl::Get()->GetKeys(
    asiAlgo_DictionaryGroup::ComponentName(), theSeqOfIds);
}

//! Get list of defined unit dimensions.
//! \param[out] dimensions list of dimensions.
void asiAlgo_Dictionary::GetDimensions(asiAlgo_DictionaryDimensions& dimensions)
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return;

  anImpl->GetDimensions(dimensions);
}

//! Get unit dimension by name identifier.
//! \param theName [in] dimension name.
//! \return handle on a dimension or null handle if the dimension is not
//!         found.
Handle(asiAlgo_DictionaryDimension) 
  asiAlgo_Dictionary::GetDimension(const TCollection_AsciiString& theName)
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return NULL;

  return anImpl->GetDimension(theName);
}

//! Get local unit system.
//! \return a copy of local unit system instance.
asiAlgo_DictionaryUnitSystem asiAlgo_Dictionary::GetLocalUnitSystem()
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return asiAlgo_DictionaryUnitSystem();

  return anImpl->GetLocalUnitSystem();
}

//! Set local unit system.
//! \param theSystem [in] the new local unit system.
void asiAlgo_Dictionary::SetLocalUnitSystem(const asiAlgo_DictionaryUnitSystem& theSystem)
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return;

  anImpl->SetLocalUnitSystem(theSystem);
}

//! Set notifier for providing log messages.
//! \param theNotifier [in] the notifier instance.
void asiAlgo_Dictionary::SetNotifier(const Handle(ActAPI_IProgressNotifier)& theNotifier)
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return;

  anImpl->SetNotifier(theNotifier);
}

//! Get notifier.
//! \return notifier instance.
Handle(ActAPI_IProgressNotifier) asiAlgo_Dictionary::GetNotifier()
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return NULL;

  return anImpl->GetNotifier();
}

//! Set flag indicating whether the unit conversion errors should be
//! logged or not.
//! \param theIsLogged [in] log enabling flag.
void asiAlgo_Dictionary::SetConversionErrorLog(const bool theIsLogged)
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return;

  anImpl->SetConversionErrorLog(theIsLogged);
}

//! Get boolean flag indicating whether the unit conversion errors should
//! be logged or not.
bool asiAlgo_Dictionary::IsConversionErrorLog()
{
  Handle(asiAlgo_DictionaryImpl) anImpl = getImpl();
  if ( anImpl.IsNull() )
    return false;

  return anImpl->IsConversionErrorLog();
}

//! Get dictionary implementation instance.
//! \return handle on dictionary implementation instance.
Handle(asiAlgo_DictionaryImpl) asiAlgo_Dictionary::getImpl()
{
  asiAlgo_DictionaryImpl::Init();
  return Handle(asiAlgo_DictionaryImpl)::DownCast(asiAlgo_DictionaryImpl::Get());
}

// ----------------------------------------------------------------------------
//                      Dictionary Implementation
// ----------------------------------------------------------------------------


//! Constructor. 
//! Set keywords used in data dictionary.
asiAlgo_DictionaryImpl::asiAlgo_DictionaryImpl() 
: DDS_Dictionary(),
  m_bLogUnitErr(false)
{
  Handle(DDS_KeyWords) aKeyWords = DDS_KeyWords::Get();

  aKeyWords->SetKeyWord( "ID",                "id" );
  aKeyWords->SetKeyWord( "DATUM" ,            "datum" );
  aKeyWords->SetKeyWord( "DATUM_ID" ,         "id" );
  aKeyWords->SetKeyWord( "DATUM_LABEL",       "label" );
  aKeyWords->SetKeyWord( "DATUM_FORMAT",      "format" );

  aKeyWords->SetKeyWord( "DATUM_DOMAIN",        "domain" );
  aKeyWords->SetKeyWord( "DOMAIN_TYPE",         "type" );
  aKeyWords->SetKeyWord( "DOMAIN_MIN",          "min" );
  aKeyWords->SetKeyWord( "DOMAIN_MAX",          "max" );
  aKeyWords->SetKeyWord( "DOMAIN_LIST",         "list" );
  aKeyWords->SetKeyWord( "DOMAIN_DEFAULT",      "default" );
  aKeyWords->SetKeyWord( "DOMAIN_TYPE_FLOAT",   "Float" );
  aKeyWords->SetKeyWord( "DOMAIN_TYPE_STRING",  "String" );
  aKeyWords->SetKeyWord( "DOMAIN_TYPE_INTEGER", "Integer" );
  aKeyWords->SetKeyWord( "DOMAIN_TYPE_LIST",    "List" );
  aKeyWords->SetKeyWord( "DOMAIN_TYPE_BOOL",    "Bool" );

  aKeyWords->SetKeyWord( "DATUM_DIMENSION",     "dimensions" );
  aKeyWords->SetKeyWord( "DIMENSION",           "dimension" );
  aKeyWords->SetKeyWord( "DIMENSION_NAME",      "name" );
  aKeyWords->SetKeyWord( "DIMENSION_VALUE",     "value" );
  aKeyWords->SetKeyWord( "DIMENSION_EXTRA",     "extra" );

  aKeyWords->SetKeyWord( "DATUM_UI",          "ui" );
  aKeyWords->SetKeyWord( "UI_WIDGET",         "widget" );
  aKeyWords->SetKeyWord( "UI_PARAMS",         "widgetParams" );
  aKeyWords->SetKeyWord( "UI_PARAM_TAG",      "widgetParam" );

  aKeyWords->SetKeyWord( "DATUM_DESCRIPTION", "description" );

  aKeyWords->SetKeyWord( "VALUE_LIST" ,         "valueList" );
  aKeyWords->SetKeyWord( "VALUE_LIST_ID" ,      "id" );
  aKeyWords->SetKeyWord( "VALUE_LIST_NAME" ,    "name" );
  aKeyWords->SetKeyWord( "VALUE_LIST_VALUE",    "value" );
  aKeyWords->SetKeyWord( "VALUE_LIST_VALUE_ID", "id" );

  aKeyWords->SetKeyWord( "UNIT",                "unit" );
  aKeyWords->SetKeyWord( "UNIT_NAME",           "name" );
  aKeyWords->SetKeyWord( "UNIT_SI",             "si" );
  aKeyWords->SetKeyWord( "UNIT_K",              "k" );
  aKeyWords->SetKeyWord( "UNIT_B",              "b" );

  // parsing characters
  aKeyWords->SetKeyWord( "SEPARATOR_CHARACTER", "|" );
  aKeyWords->SetKeyWord( "ASSIGN_CHARACTER",    "=" );
}

//! Fill dictionary internal data.
//! \param theDicElt [in] dictionary entry in XML definiton file.
void asiAlgo_DictionaryImpl::FillDataMap(const LDOM_Element& theDicElt)
{
  Handle(asiAlgo_DictionaryGroup) aGroup = new asiAlgo_DictionaryGroup();

  // the dictionary root and group root is the same elements
  aGroup->FillDataMap(theDicElt, theDicElt);

  // attach loaded definitions
  if ( myGroupMap.Contains( aGroup->GetName() ) )
    myGroupMap.ChangeFromKey( aGroup->GetName() ) = aGroup;
  else
    myGroupMap.Add(aGroup->GetName(), aGroup);
}

//! Get list of defined dimensions for all component groups.
//! \param[out] dimensions list of dimensions.
void asiAlgo_DictionaryImpl::GetDimensions(asiAlgo_DictionaryDimensions& dimensions) const
{
  for ( int idx = 1; idx <= myGroupMap.Extent(); ++idx )
  {
    const Handle(asiAlgo_DictionaryGroup)&
      group = Handle(asiAlgo_DictionaryGroup)::DownCast( myGroupMap.FindFromIndex(idx) );

    if ( !group.IsNull() )
    {
      // Collect dimensions from the current group into the output list.
      group->GetDimensions(dimensions);
    }
  }
}

//! Get list of defined dimensions of a component group.
//! \param[in]  component  component group.
//! \param[out] dimensions list of dimensions.
void asiAlgo_DictionaryImpl::GetDimensions(const TCollection_AsciiString& component,
                                           asiAlgo_DictionaryDimensions&  dimensions) const
{
  if ( !myGroupMap.Contains(component) )
    return;

  const Handle(asiAlgo_DictionaryGroup)&
    group = Handle(asiAlgo_DictionaryGroup)::DownCast( myGroupMap.FindFromKey(component) );

  group->GetDimensions(dimensions);
}

//! Search dimension definition by name in all component groups.
//! \param theName [in] name of dimension to be found and returneds.
//! \return handle to a first found dimension definition specified by name,
//!         or null handle if the dimension is not found.
Handle(asiAlgo_DictionaryDimension)
  asiAlgo_DictionaryImpl::GetDimension(const TCollection_AsciiString& theName) const
{
  Handle(asiAlgo_DictionaryDimension) aDimension;

  int anIdx = 1;
  for ( ; anIdx <= myGroupMap.Extent() && aDimension.IsNull(); anIdx++ )
  {
    Handle(asiAlgo_DictionaryGroup) aGroup = 
      Handle(asiAlgo_DictionaryGroup)::DownCast(myGroupMap.FindFromIndex(anIdx));
    if ( !aGroup.IsNull() )
      aDimension = aGroup->GetDimension(theName);
  }

  return aDimension;
}

//! Get dimension definition by name for a component.
//! \param theName [in] name of dimension to be found and returneds.
//! \param theComponent [in] name of component group.
//! \return handle to a dimension definition specified by name,
//!         or null handle when the dimension is not found.
Handle(asiAlgo_DictionaryDimension)
  asiAlgo_DictionaryImpl::GetDimension(const TCollection_AsciiString& theName,
                                       const TCollection_AsciiString& theComponent) const
{
  if ( !myGroupMap.Contains(theComponent) )
    return Handle(asiAlgo_DictionaryDimension)();

  Handle(asiAlgo_DictionaryGroup) aGroup = 
    Handle(asiAlgo_DictionaryGroup)::DownCast(myGroupMap.FindFromKey(theComponent));

  return aGroup->GetDimension(theName);
}

//! Set notifier for providing log messages.
//! \param theNotifier [in] the notifier instance.
void asiAlgo_DictionaryImpl::SetNotifier(
  const Handle(ActAPI_IProgressNotifier)& theNotifier)
{
  m_Notifier = theNotifier;
}

//! Get notifier.
//! \return notifier instance.
Handle(ActAPI_IProgressNotifier) asiAlgo_DictionaryImpl::GetNotifier() const
{
  return m_Notifier;
}

//! Init data dictionary. This method should be called prior to
//! using the data dictionary, before Get() method is called.
//! It initializes and sets the custom data dictionary instance to be
//! used as a common dictionary.
void asiAlgo_DictionaryImpl::Init()
{
  Handle(DDS_Dictionary) aDic = DDS_Dictionary::Get();

  if ( !aDic->IsKind( STANDARD_TYPE(asiAlgo_DictionaryImpl) ) )
    SetDictionary( new asiAlgo_DictionaryImpl() );
}

//! Get name of system of units used by data dictionary.
TCollection_AsciiString asiAlgo_DictionaryImpl::GetAsiUnitSystem()
{
  return "asiAlgo_Units";
}

TCollection_AsciiString asiAlgo_DictionaryImpl::GetAsiUnitSystemLabel()
{
  return "Analysis Situs Units System";
}

//! Get boolean flag indicating whether the unit conversion errors should
//! be logged or not.
void asiAlgo_DictionaryImpl::SetConversionErrorLog(const bool theIsLogged)
{
  m_bLogUnitErr = theIsLogged;
}

//! Get dictionary implementation instance.
//! \return handle on dictionary implementation instance.
bool asiAlgo_DictionaryImpl::IsConversionErrorLog() const
{
  return m_bLogUnitErr;
}
