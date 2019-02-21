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

#ifndef asiUI_ParamEditorImpl_HeaderFile
#define asiUI_ParamEditorImpl_HeaderFile

// asiUI includes
#include <asiUI.h>

// asiUI includes
#include <asiUI_IParamEditor.h>

// OCCT includes
#include <NCollection_IndexedDataMap.hxx>

// Active Data includes
#include <ActAPI_IParameter.h>

// Qt includes
#pragma warning(push, 0)
#include <QStringList>
#include <QVariantList>
#pragma warning(pop)

// Forward declaration
class asiUI_ParameterEditor;

//-----------------------------------------------------------------------------

//! Implementation of IParamEditor interface.
class asiUI_ParamEditorImpl : public asiUI_IParamEditor
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_ParamEditorImpl, asiUI_IParamEditor)

public:

  //! Ctor.
  asiUI_ParamEditorImpl(asiUI_ParameterEditor* pEditor) : asiUI_IParamEditor()
  {
    m_pEditor = pEditor;
  }

  //! \return editor widget.
  asiUI_ParameterEditor* GetWidget() const
  {
    return m_pEditor;
  }

public:

  asiUI_EXPORT virtual void
    SetParameters(const Handle(ActAPI_SparseParameterList)& params);

  asiUI_EXPORT virtual void
    UpdateParameters();

  asiUI_EXPORT virtual bool
    IsEnabled() const;

  asiUI_EXPORT virtual void
    SetEnabled(const bool isEnabled);

  asiUI_EXPORT virtual bool
    IsReadOnly() const;

  asiUI_EXPORT virtual void
    SetReadOnly(const bool isReadOnly);

  asiUI_EXPORT void
    SetComboValues(const Handle(ActAPI_IUserParameter)& parameter,
                   const QStringList&                   strings,
                   const QList<int>&                    ids,
                   const QVariantList&                  valueByIds = QVariantList());

private:

  QVariant ConvertValue(const Handle(ActAPI_IUserParameter)& parameter) const;

private:

  Handle(ActAPI_SparseParameterList) m_paramList; //!< Internal cache.
  asiUI_ParameterEditor*             m_pEditor;   //!< Encapsulated widget.
};

#endif
