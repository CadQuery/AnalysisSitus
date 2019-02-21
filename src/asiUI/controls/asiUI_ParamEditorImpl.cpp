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

// asiUI includes
#include <asiUI_ParamEditorImpl.h>
#include <asiUI_ParameterEditor.h>
#include <asiUI_ParameterFlags.h>
#include <asiUI_Tools.h>

// ActData includes
#include <ActData_Common.h>
#include <ActData_AsciiStringParameter.h>
#include <ActData_RealParameter.h>
#include <ActData_BoolParameter.h>
#include <ActData_IntParameter.h>
#include <ActData_GroupParameter.h>
#include <ActData_BaseNode.h>

// asiAlgo includes
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_DictionaryItem.h>

// asiUI includes
#include <ActAPI_IParameter.h>

// OCC includes
#include <TCollection/TCollection_HExtendedString.hxx>

// Qt includes
#include <QVariant>


//! Fill list of parameters in parameter editor.
void asiUI_ParamEditorImpl::SetParameters(const Handle(ActAPI_SparseParameterList)& theParameterList)
{
  m_ParamList = Handle(ActAPI_SparseParameterList)();

  if ( m_pEditor == 0 )
  {
    return;
  }

  // if input list is empty, clear the parameter
  if ( theParameterList.IsNull() )
  {
    m_pEditor->ClearParameters();
    return;
  }

  QList<asiUI_ParameterEditorData> aParamList;
  ActAPI_SparseParameterList::ConstIterator anIt( *theParameterList.operator->() );
  for ( ; anIt.More(); anIt.Next() )
  {
    const Handle(ActAPI_IParameter)& aParam = anIt.Value();

    bool isVisible  = aParam->HasUserFlags(ParameterFlag_IsVisibleInEditor);
    bool isReadOnly = aParam->HasUserFlags(ParameterFlag_IsReadOnlyInEditor);

    if ( aParam->GetName().Length() == 0 || !isVisible )
    {
      continue;
    }

    int aNodalId = (int) anIt.Key();

    if ( aParam->IsKind(STANDARD_TYPE(ActData_GroupParameter)) )
    {
      aParamList.append( asiUI_ParameterEditorData::GroupParameter( aNodalId, asiUI_Tools::ToQString( aParam->GetName() ) ) );
      continue;
    }

    Handle(ActAPI_INode) aNode = aParam->GetNode();

    aParamList.append( asiUI_ParameterEditorData::ValueParameter( aNodalId,
                       asiUI_Tools::ToQString( aParam->GetName() ),
                       ConvertValue(aParam),
                       asiUI_Tools::ToQString( aParam->GetSemanticId() ),
                       aNode->IsEvaluable(aNodalId) == true,
                       aNode->HasConnectedEvaluator(aNodalId) == true,
                       !isReadOnly) );
  }

  m_ParamList = new ActAPI_SparseParameterList(theParameterList->Extent());
  m_ParamList->Assign(*theParameterList.operator->());
  m_pEditor->SetParameters(aParamList);
}

//! Update displayed data of the parameters shown in the parameter editor.
void asiUI_ParamEditorImpl::UpdateParameters()
{
  if ( m_pEditor == 0 || m_ParamList.IsNull() )
  {
    return;
  }

  QList<asiUI_ParameterEditorData> aParamList;
  ActAPI_SparseParameterList::ConstIterator anIt( *( m_ParamList.operator->() ) );
  for ( ; anIt.More(); anIt.Next() )
  {
    const Handle(ActAPI_IParameter)& aParam = anIt.Value();

    bool isVisible  = aParam->HasUserFlags(ParameterFlag_IsVisibleInEditor);
    bool isReadOnly = aParam->HasUserFlags(ParameterFlag_IsReadOnlyInEditor);

    if ( aParam->GetName().Length() == 0 || !isVisible )
    {
      continue;
    }

    int aNodalId = (int) anIt.Key();

    if ( aParam->IsKind(STANDARD_TYPE(ActData_GroupParameter)) )
    {
      aParamList.append( asiUI_ParameterEditorData::GroupParameter( aNodalId, asiUI_Tools::ToQString( aParam->GetName() ) ) );
      continue;
    }

    Handle(ActAPI_INode) aNode = aParam->GetNode();

    aParamList.append( asiUI_ParameterEditorData::ValueParameter( aNodalId,
                       asiUI_Tools::ToQString( aParam->GetName() ),
                       ConvertValue(aParam),
                       asiUI_Tools::ToQString( aParam->GetSemanticId() ),
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
void asiUI_ParamEditorImpl::SetComboValues( const Handle(ActAPI_IParameter)& theParameter,
                                             const QStringList& theStrings,
                                             const QIntList& theIds,
                                             const QVariantList& theValueByIds )
{
  QList<asiUI_ParameterEditorData> aParamList;
  ActAPI_SparseParameterList::ConstIterator anIt( *m_ParamList.operator->() );
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
QVariant asiUI_ParamEditorImpl::ConvertValue(const Handle(ActAPI_IParameter)& theParameter) const
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
