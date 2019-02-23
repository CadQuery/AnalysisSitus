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

#ifndef asiUI_ParameterEditor_HeaderFile
#define asiUI_ParameterEditor_HeaderFile

// asiUI includes
#include <asiUI_WidgetFactory.h>

// Active Data includes
#include <ActAPI_IParameter.h>

// Salome includes
#pragma warning(push, 0)
#include <SUIT_PopupClient.h>
#include <Qtx.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QVariantList>
#include <QPointer>
#include <QStringList>
#include <QWidget>
#pragma warning(pop)

class asiUI_ParameterEditorView;
struct asiUI_ParameterEditorData;

typedef QList<asiUI_ParameterEditorData> asiUI_ParamDataList;

//-----------------------------------------------------------------------------

//! Widget to handle Data Model parameters. This widget is encapsulated into
//! the implementation of asiUI_IParamEditor interface.
//!
//! \sa asiUI_IParamEditor
class asiUI_ParameterEditor : public QWidget, public SUIT_PopupClient
{
  Q_OBJECT

public:

  //! Ctor.
  //! \param[in] factory factory for datums.
  //! \param[in] parent  parent widget for this editor.
  asiUI_ParameterEditor(const Handle(asiUI_WidgetFactory)& factory,
                        QWidget*                           parent = NULL);

public:

  //! Sets he list of parameter types and values, the items following after separator
  //! will be included in its group.
  //! \param[in] parameters list of parameters to set.
  void SetParameters(const asiUI_ParamDataList& params);

  //! Updates the current list of parameters. The current parameter list will be 
  //! synchronized with the passed list of parameters; the selection will not
  //! change.
  //! \param[in] params list of data parameters.
  void UpdateParameters(const asiUI_ParamDataList& params);

  //! Clears the parameter editor.
  void ClearParameters();

  //! \return true if the editor is in a read-only state.
  bool IsReadOnly() const;

  //! Sets read-only state for the parameter editor.
  //! \param[in] isReadOnly on/off.
  void SetReadOnly(const bool isReadOnly);

  //! Overrides the list of values for a combo-box parameter. The parameter must
  //! be defined in the parameter editor. The values are cleared once the
  //! list is changed. Exception is raised if the parameter is not combo or
  //! is not defined in the parameter editor. The passed collections should have
  //! identical sizes.
  //!
  //! \param[in] pid        local id of a parameter in its owning Data Node.
  //! \param[in] strings    string values to display in a combo-box.
  //! \param[in] ids        item ids or values to use if `valueByIds` is
  //!                       not defined.
  //! \param[in] valueByIds maps ids to real values which will be stored.
  //!                       If this list if empty, then the `ids` are
  //!                       used instead.
  void SetComboValues(const int           pid,
                      const QStringList&  strings,
                      const QList<int>&   ids,
                      const QVariantList& valueByIds = QVariantList());

signals:

  //! signal emitted when editor activates for editing
  //! the parameter.
  //! \param theParamID [in] parameter identifier within the node.
  void ParamEditorActivated(const int theParamID);

  //! signal emitted when parameter editor closes.
  //! \param theParamID [in] parameter identifier within the node.
  //! \param theIsCommit [in] boolean value indicating whether the
  //!        editor closes with commiting or rejecting the data.
  void ParamEditorClosed(const int theParamID, const bool theIsCommited);

  //! signal emitted when user changed value field of the parameter.
  //! \param theParamID [in] parameter identifier within the node.
  //! \param theValue [in] new value.
  void ParamModified(const int theParamID, const QVariant& theValue);

  //! signal emitted when user changes "computed" state for the parameter
  //! by click corresponding check-box.
  //! \param theParamID [in] parameter identifier within the node.
  //! \param theIsComputed [in] boolean value indicating wheter the parameter
  //!        should be evaluated by its expression or not.
  void ParamComputedChanged(const int theParamID, const bool theIsComputed);

  //! signal emitted when user triggers action button of custom selector
  //! datum set for the parameter.
  //! \param theParamID [in] parameter identifier within the node.
  void CustomSelectorTriggered(const int theParamID);

protected slots:

  void onParamModified(const int theParamID, const QVariant& theValue);

private:

  //! Get client type.
  //! \return string with type.
  virtual QString popupClientType() const
  {
    return QString("asiUI_ParameterEditor");
  }

private:

  //! Map combo-box datum control values to QVariant associations
  typedef QMap<int, QVariant>    IdsValueMap;
  typedef QMap<int, IdsValueMap> ParameterValueMapping;
  ParameterValueMapping          m_comboValueMapping;

  //! View.
  QPointer<asiUI_ParameterEditorView> m_pView;
};

//-----------------------------------------------------------------------------
// Parameter data
//-----------------------------------------------------------------------------

//! Wrapper for representing a Data Model parameter in GUI.
struct asiUI_ParameterEditorData
{
  //! group parameter data
  static
    asiUI_ParameterEditorData GroupParameter(int     theNodalID,
                                             QString theName)
  {
    // init group's data
    asiUI_ParameterEditorData aData;
    aData.m_bIsGroup = true;
    aData.m_Name     = theName;
    aData.m_iNodalID = theNodalID;

    // init other data
    aData.m_bIsComputable = false;
    aData.m_bComputed     = false;
    aData.m_bIsEnabled    = true;

    return aData;
  }

  //! value parameter data
  static asiUI_ParameterEditorData 
    ValueParameter(int      theNodalID,
                   QString  theName,
                   QVariant theValue,
                   QString  theDictID,
                   bool     theIsComputable,
                   bool     theIsComputed,
                   bool     theIsEnabled)
  {
    // init parameter's data
    asiUI_ParameterEditorData aData;
    aData.m_bIsGroup      = false;
    aData.m_Name          = theName;
    aData.m_iNodalID      = theNodalID;
    aData.m_bComputed     = theIsComputed;
    aData.m_bIsComputable = theIsComputable;
    aData.m_Value         = theValue;
    aData.m_DictID        = theDictID;
    aData.m_bIsEnabled    = theIsEnabled;

    return aData;
  }


  //! Get parameter identifier used
  //! to locate parameter for node.
  int GetNodeID() const
  {
    return m_iNodalID;
  }

  //! Get parameter identifier for 
  //! data dictionary.
  QString GetDictID() const
  {
    return m_DictID;
  }

  //! Get parameter name
  QString GetName() const
  {
    return m_Name;
  }

  //! Get variant value
  QVariant GetValue() const
  {
    return m_Value;
  }

  QVariant& ChangeValue()
  {
    return m_Value;
  }

  //! Check whether the parameter is
  //! group separator.
  //! \return Boolean value indicating whether
  //!         the parameter is group separator.
  bool IsGroup() const
  {
    return m_bIsGroup;
  }

  //! Get computed state.
  bool GetComputed() const
  {
    return m_bComputed;
  }

  //! Indicates if it is possible to specify the parameter
  //! through the evaluation string.
  //! \return true if parameter's value can be specified
  //!         through the evaluation string.
  bool IsComputable() const
  {
    return m_bIsComputable;
  }

  //! Get enabled state.
  bool IsEnabled() const
  {
    return m_bIsEnabled;
  }

private:

  //! parameter identifier for location in node.
  int m_iNodalID;

  //! parameter data type for location in data dictionary.
  QString m_DictID;

  //! parameter name
  QString m_Name;

  //! parameter's value
  QVariant m_Value;

  //! is parameter computable.
  bool m_bComputed;

  //! is it evaluated, or entered manually.
  bool m_bIsComputable;

  //! is it a group separator.
  bool m_bIsGroup;

  //! is it enabled.
  bool m_bIsEnabled;
};

#endif
