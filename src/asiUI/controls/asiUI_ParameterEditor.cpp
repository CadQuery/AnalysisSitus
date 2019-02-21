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
#include <asiUI_ParameterEditor.h>
#include <asiUI_ParameterEditorView.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QHBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_ParameterEditor::asiUI_ParameterEditor(QWidget* parent) : QWidget(parent)
{
  m_pView = new asiUI_ParameterEditorView(parent);
  
  QHBoxLayout* layout = new QHBoxLayout;
  layout->setMargin(0);
  layout->addWidget(m_pView);
  //
  this->setLayout(layout);

  connect( m_pView, SIGNAL( ParamEditorActivated(const int) ),
                    SIGNAL( ParamEditorActivated(const int) ) );
  //
  connect( m_pView, SIGNAL( ParamEditorClosed(const int, const bool) ),
                    SIGNAL( ParamEditorClosed(const int, const bool) ) );
  //
  connect( m_pView, SIGNAL( ParamModified(const int, const QVariant&) ),
                    SLOT(   onParamModified(const int, const QVariant&) ) );
  //
  connect( m_pView, SIGNAL( ParamComputedChanged(const int, const bool) ),
                    SIGNAL( ParamComputedChanged(const int, const bool) ) );
  //
  connect( m_pView, SIGNAL( CustomSelectorTriggered(const int) ),
                    SIGNAL( CustomSelectorTriggered(const int) ) );
}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditor::SetParameters(const asiUI_ParamDataList& params)
{
  m_comboValueMapping.clear();

  m_pView->SetParameters(params);
}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditor::UpdateParameters(const asiUI_ParamDataList& params)
{
  // provide mapping substitution
  if ( m_comboValueMapping.isEmpty() )
  {
    m_pView->UpdateParameters(params);
    return;
  }

  asiUI_ParamDataList changeValues;
  asiUI_ParamDataList::const_iterator inputIt = params.constBegin();
  //
  for ( ; inputIt != params.constEnd(); ++inputIt )
  {
    const asiUI_ParameterEditorData& inputData = *inputIt;

    changeValues.append(inputData);

    int paramId = inputData.GetNodeID();

    if ( !m_comboValueMapping.contains(paramId) )
      continue;

    // provide value mapping to combo-box key
    asiUI_ParameterEditorData& changeData = changeValues.last();

    QVariant inputValue = changeData.GetValue();

    changeValues.last().ChangeValue() = m_comboValueMapping[paramId].key(inputValue, -1);
  }

  m_pView->UpdateParameters(changeValues);
}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditor::ClearParameters()
{
  m_pView->ClearParameters();
}

//-----------------------------------------------------------------------------

bool asiUI_ParameterEditor::IsReadOnly() const
{
  return m_pView->IsReadOnly();
}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditor::SetReadOnly(const bool theIsReadOnly)
{
  m_pView->SetReadOnly(theIsReadOnly);
}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditor::SetComboValues(const int           pid,
                                           const QStringList&  strings,
                                           const QList<int>&   ids,
                                           const QVariantList& valueByIds)
{
  if ( !valueByIds.isEmpty() && valueByIds.length() != ids.length() )
  {
    Standard_ProgramError::Raise("Empty or incompatible collections.");
  }

  m_pView->SetComboValues(pid, strings, ids);

  m_comboValueMapping.remove(pid);

  if ( !valueByIds.isEmpty() )
  {
    IdsValueMap valueMap;
    QList<int>::const_iterator idsIt = ids.constBegin();
    QVariantList::const_iterator valueIt = valueByIds.constBegin();
    for ( ; idsIt != ids.constEnd(); ++idsIt, ++valueIt )
      valueMap.insert(*idsIt, *valueIt);

    m_comboValueMapping.insert(pid, valueMap);
  }
}

//-----------------------------------------------------------------------------

//! Slot to catch value modification signals. Maps combo-box indexes to
//! associated QVariant values.
//! \param theParamID [in] parameter identifier within the node.
//! \param theValue [in] new value.
void asiUI_ParameterEditor::onParamModified(const int theParamID, const QVariant& theValue)
{
  QVariant aEmitValue = m_comboValueMapping.contains(theParamID)
                      ? m_comboValueMapping[theParamID].value( theValue.toInt(), QVariant() )
                      : theValue;

  emit ParamModified(theParamID, aEmitValue);
}
