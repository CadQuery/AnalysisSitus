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
#include <asiUI_ParamEditorImpl.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_ParameterEditor.h>

// asiData includes
#include <asiData_ParameterFlags.h>

// asiAlgo includes
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_DictionaryItem.h>

// ActData includes
#include <ActData_Common.h>
#include <ActData_AsciiStringParameter.h>
#include <ActData_RealParameter.h>
#include <ActData_BoolParameter.h>
#include <ActData_IntParameter.h>
#include <ActData_GroupParameter.h>
#include <ActData_BaseNode.h>

// OCCT includes
#include <TCollection_HExtendedString.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QVariant>
#pragma warning(pop)

//! Fill list of parameters in parameter editor.
void asiUI_ParamEditorImpl::SetParameters(const Handle(ActAPI_HSparseParameterList)& theParameterList)
{
  m_paramList.Nullify();

  if ( m_pEditor == 0 )
    return;

  // if input list is empty, clear the parameter
  if ( theParameterList.IsNull() )
  {
    m_pEditor->ClearParameters();
    return;
  }

  QList<asiUI_ParameterEditorData> aParamList;
  ActAPI_HSparseParameterList::ConstIterator anIt( *theParameterList.operator->() );
  for ( ; anIt.More(); anIt.Next() )
  {
    const Handle(ActAPI_IUserParameter)& P = anIt.Value();

    const bool isVisible  = P->HasUserFlags(ParameterFlag_IsVisible);
    const bool isReadOnly = P->HasUserFlags(ParameterFlag_IsReadOnly);

    if ( P->GetName().Length() == 0 || !isVisible )
      continue;

    int aNodalId = (int) anIt.Key();

    if ( P->IsKind(STANDARD_TYPE(ActData_GroupParameter)) )
    {
      aParamList.append( asiUI_ParameterEditorData::GroupParameter( aNodalId, asiUI_Common::ToQString( P->GetName() ) ) );
      continue;
    }

    Handle(ActAPI_INode) aNode = P->GetNode();

    aParamList.append( asiUI_ParameterEditorData::ValueParameter( aNodalId,
                       asiUI_Common::ToQString( P->GetName() ),
                       ConvertValue(P),
                       asiUI_Common::ToQString( P->GetSemanticId() ),
                       aNode->IsEvaluable(aNodalId) == true,
                       aNode->HasConnectedEvaluator(aNodalId) == true,
                       !isReadOnly) );
  }

  m_paramList = new ActAPI_HSparseParameterList(theParameterList->Extent());
  m_paramList->Assign(*theParameterList.operator->());
  m_pEditor->SetParameters(aParamList);
}

//! Update displayed data of the parameters shown in the parameter editor.
void asiUI_ParamEditorImpl::UpdateParameters()
{
  if ( m_pEditor == 0 || m_paramList.IsNull() )
  {
    return;
  }

  QList<asiUI_ParameterEditorData> aParamList;
  ActAPI_HSparseParameterList::ConstIterator anIt( *( m_paramList.operator->() ) );
  for ( ; anIt.More(); anIt.Next() )
  {
    const Handle(ActAPI_IUserParameter)& aParam = anIt.Value();

    bool isVisible  = aParam->HasUserFlags(ParameterFlag_IsVisible);
    bool isReadOnly = aParam->HasUserFlags(ParameterFlag_IsReadOnly);

    if ( aParam->GetName().Length() == 0 || !isVisible )
    {
      continue;
    }

    int aNodalId = (int) anIt.Key();

    if ( aParam->IsKind(STANDARD_TYPE(ActData_GroupParameter)) )
    {
      aParamList.append( asiUI_ParameterEditorData::GroupParameter( aNodalId, asiUI_Common::ToQString( aParam->GetName() ) ) );
      continue;
    }

    Handle(ActAPI_INode) aNode = aParam->GetNode();

    aParamList.append( asiUI_ParameterEditorData::ValueParameter( aNodalId,
                       asiUI_Common::ToQString( aParam->GetName() ),
                       ConvertValue(aParam),
                       asiUI_Common::ToQString( aParam->GetSemanticId() ),
                       aNode->IsEvaluable(aNodalId) == true,
                       aNode->HasConnectedEvaluator(aNodalId) == true,
                       !isReadOnly) );
  }

  m_pEditor->UpdateParameters(aParamList);
}

//! Override list of values for combo box parameter. The parameter must be defined in the parameter
//! editor. The values are cleared once the parameter list is changed.
//! Exception is raised if the parameter is not combo or is not defined in parameter editor.
//! The passed collections should have the same size.
//! \param theParameterId [in] the id of parameter.
//! \param theStrings [in] the strings to display in combo-box.
//! \param theIds [in] the item ids or values to use if theValueMap is not defined.
//! \param theValueByIds [in] maps ids to real values, which will be stored in parameter.
//!        if empty, then the ids are used instead.
void asiUI_ParamEditorImpl::SetComboValues( const Handle(ActAPI_IUserParameter)& theParameter,
                                            const QStringList& theStrings,
                                            const QList<int>& theIds,
                                            const QVariantList& theValueByIds )
{
  QList<asiUI_ParameterEditorData> aParamList;
  ActAPI_HSparseParameterList::ConstIterator anIt( *m_paramList.operator->() );
  for ( ; anIt.More(); anIt.Next() )
  {
    if ( !ActAPI_IDataCursor::IsEqual( anIt.Value(), theParameter ) )
    {
      continue;
    }

    m_pEditor->SetComboValues( (int) anIt.Key(), theStrings, theIds, theValueByIds );

    UpdateParameters();

    return;
  }

  Standard_ProgramError::Raise("Error: parameter is not defined in parameter editor.");
}

//! Checks whether the user interaction with the parameter editor is
//! enabled.
//! \return boolean value that indicates whether the parameter editor is
//!         enabled for interaction or not.
bool asiUI_ParamEditorImpl::IsEnabled() const
{
  return m_pEditor->isEnabled() ? true : false;
}

//! Enable/disable user interaction with parameter editor.
//! \param theIsEnabled [in] indicates whether the parameter editor
//!        should be enabled or disabled.
void asiUI_ParamEditorImpl::SetEnabled(const bool theIsEnabled)
{
  m_pEditor->setEnabled(theIsEnabled == true);
}

//! Checks whether the parameter editor is in "read only" state.
//! \return boolean value indicating the parameter editor is in
//!         "read only" state or not.
bool asiUI_ParamEditorImpl::IsReadOnly() const
{
  return m_pEditor->IsReadOnly() ? true : false;
}

//! Enable/disable "read only" state for the parameter editor.
//! \param theIsReadOnly [in] specifies whether the parameter
//!        editor state should be set to "read only" or not.
void asiUI_ParamEditorImpl::SetReadOnly(const bool theIsReadOnly)
{
  m_pEditor->SetReadOnly(theIsReadOnly == true);
}

//! Convert value of parameter for parameter editor control.
//! \param theParameter [in] the parameter to convert.
QVariant asiUI_ParamEditorImpl::ConvertValue(const Handle(ActAPI_IUserParameter)& theParameter) const
{
  if ( theParameter->IsKind( STANDARD_TYPE(ActData_ReferenceParameter) ) )
  {
    Handle(asiAlgo_DictionaryItem) anItem = asiAlgo_Dictionary::GetDictionaryItem( theParameter->GetSemanticId() );
    if ( !anItem.IsNull() && anItem->GetWidgetType() == asiAlgo_DictionaryItem::WT_ComboBox )
    {
      Handle(ActData_ReferenceParameter) aReference = Handle(ActData_ReferenceParameter)::DownCast(theParameter);
      Handle(ActAPI_IDataCursor) aDataCursor = aReference->GetTarget();
      return aDataCursor.IsNull() ? QVariant("") : AsciiStr2QStr( aDataCursor->GetId() );
    }
  }

  return asiUI_ParametersTool::GetParameterValue(theParameter);
}

// ----------------------------------------------------------------------------
//                      Parameter tool
// ----------------------------------------------------------------------------

//! Convert parameter's value to QVariant type.
//! \param theParameter [in] parameter to get the value.
//! \return converted value or invalid qvariant if the value can't be converted.
QVariant asiUI_ParametersTool::GetParameterValue(const Handle(ActAPI_IUserParameter)& theParameter)
{  
  if ( !theParameter->IsValidData() )
  {
    return QVariant();
  }

  // TODO: use switch - case with ParmeterType enumeration instead of
  //       if - else check.
  if ( theParameter->IsKind( STANDARD_TYPE(ActData_NameParameter) ) )
  {
    // convert name parameter
    return ExtStr2QStr(
      ActData_ParameterFactory::AsName(theParameter)->GetValue() );
  }
  else if ( theParameter->IsKind( STANDARD_TYPE(ActData_AsciiStringParameter) ) )
  {
    // convert ascii string parameter
    return ExtStr2QStr(
      ActData_ParameterFactory::AsAsciiString(theParameter)->GetValue() );
  }
  else if ( theParameter->IsKind( STANDARD_TYPE(ActData_IntParameter) ) )
  {
    // convert integer type parameter
    return ActData_ParameterFactory::AsInt(theParameter)->GetValue();
  }
  else if ( theParameter->IsKind( STANDARD_TYPE(ActData_RealParameter) ) )
  {
    // convert double type parameter
    return ActData_ParameterFactory::AsReal(theParameter)->GetValue();
  }
  else if ( theParameter->IsKind( STANDARD_TYPE(ActData_BoolParameter) ) )
  {
    // convert boolean type parameter
    return ActData_ParameterFactory::AsBool(theParameter)->GetValue();
  }
  else if ( theParameter->IsKind( STANDARD_TYPE(ActData_ReferenceParameter) ) )
  {
    // convert reference name parameter
    Handle(ActAPI_IDataCursor) aTarget =
      ActData_ParameterFactory::AsReference(theParameter)->GetTarget();

    // null reference - return empty name
    if ( aTarget.IsNull() )
    {
      return QVariant( "" );
    }

    if ( aTarget->IsKind( STANDARD_TYPE(ActAPI_IUserParameter) ) )
    {
      return ExtStr2QStr(
        Handle(ActAPI_IUserParameter)::DownCast(aTarget)->GetName() );
    }
    else if ( aTarget->IsKind(STANDARD_TYPE(ActAPI_INode)) )
    {
      return ExtStr2QStr(
        Handle(ActAPI_INode)::DownCast(aTarget)->GetName() );
    }
  }
  else if ( theParameter->IsKind( STANDARD_TYPE(ActData_TimeStampParameter) ) )
  {
    Handle(ActAux_TimeStamp) aStamp = 
      ActData_ParameterFactory::AsTimeStamp(theParameter)->GetValue();

    return !aStamp.IsNull() ?
      QVariant(asiUI_Common::ToQDateTime(aStamp)) : QVariant();
  }

  return QVariant("");
}

//! Get list of computed parameter, which values are evaluated by the framework 
//! from the expression.
//! \param theModel [in] model.
//! \return list of computed parameters.
Handle(ActAPI_HParameterList) 
  asiUI_ParametersTool::GetEvaluatedParameters(const Handle(ActAPI_IModel)& theModel)
{
  Handle(ActAPI_HParameterList) aList = new ActAPI_HParameterList();

  // get computed parameters for the nodes recursively
  getComputedParams(theModel->GetRootNode(), aList);

  return aList;
}

//! Get index of the parameter within the node.
//! \param theParameter [in] index of this parameter will be searched by this method.
//! \return index of the parameter or -1 if the index can't be found.
int asiUI_ParametersTool::GetParameterIndex(const Handle(ActAPI_IUserParameter)& theParameter)
{
  Handle(ActAPI_INode) aParamNode = theParameter->GetNode();
  ASSERT_RAISE(!aParamNode.IsNull(), "invalid node for the parameter");

  Handle(ActAPI_IParamIterator) aParamIt = aParamNode->GetParamIterator();
  for ( ; aParamIt->More(); aParamIt->Next() )
  {
    if ( ActAPI_IDataCursor::IsEqual(aParamIt->Value(), theParameter) )
      return aParamIt->Key();
  }

  return -1;
}

//! recursively append computed parameter from the node to the
//! passed as argument parameters list.
//! \param theNode [in] theNode to process.
//! \param theParamList [out] list of computed parameter.
void 
  asiUI_ParametersTool::getComputedParams(const Handle(ActAPI_INode)& theNode,
                                           Handle(ActAPI_HParameterList)& theParamList)
{
  Handle(ActAPI_IParamIterator) aParamIt = theNode->GetParamIterator();
  for ( ; aParamIt->More(); aParamIt->Next() )
  {
    Handle(ActAPI_IUserParameter) aParam = aParamIt->Value();
    
    if ( theNode->HasConnectedEvaluator(aParamIt->Key()) )
      theParamList->Append(aParam);
  }

  // iterate on children recurisvely
  Handle(ActAPI_IChildIterator) aChildIt = theNode->GetChildIterator();
  for ( ; aChildIt->More(); aChildIt->Next() )
  {
    getComputedParams(aChildIt->Value(), theParamList);
  }
}
