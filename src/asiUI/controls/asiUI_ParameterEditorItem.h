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

#ifndef asiUI_ParameterEditorItem_HeaderFile
#define asiUI_ParameterEditorItem_HeaderFile

// Qt includes
#pragma warning(push, 0)
#include <QTreeWidgetItem>
#pragma warning(pop)

#include <asiUI_ParameterEditor.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_Datum.h>
#include <asiUI_ParameterEditorView.h>

// SUIT includes
#include <DDS_DicItem.h>

//-----------------------------------------------------------------------------

//! This item provides customization introduced by the parameter editor view.
//! In case of QColor data model value, the item's cell will be colored in 
//! this color.
class asiUI_ParameterEditorItem : public QTreeWidgetItem
{
public:

  //! Column enumeration.
  enum ColumnID
  {
    Column_Name     = 0,
    Column_Computed = 1,
    Column_Value    = 2
  };

  //! User data role enumeration.
  enum DataRole
  {
    Role_ParamNodeID = Qt::UserRole + 1,
    Role_ParamDictID = Qt::UserRole + 2,
    Role_ShortFmtID  = Qt::UserRole + 3
  };

public:

  asiUI_ParameterEditorItem(QTreeWidgetItem*           theParent,
                            const QString&             theName,
                            const int&                 theNodalId,
                            const Handle(DDS_DicItem)& theDicItem,
                            asiUI_Datum*               theDatum);

  virtual ~asiUI_ParameterEditorItem();

  //! Get item's name
  //! \return name string.
  QString GetName() const
  {
    return data(Column_Name, Qt::EditRole).toString();
  }

  //! Get item's node id.
  //! \return node id string.
  int GetNodeId() const
  {
    return data(Column_Name, Role_ParamNodeID).toInt();
  }

  //! Get item's dictionary id.
  //! \return id string.
  QString GetDicId() const
  {
    return !m_DicItem.IsNull() ? asiUI_Common::ToQString( m_DicItem->GetId() ) : QString();
  }

  void SetName(const QString& theName);

  //! Set item's value.
  //! \param theValue [in] item's value.
  void SetValue(const QVariant& theValue)
  {
    setData(Column_Value, Qt::EditRole, theValue);
  }

  //! Get item's value.
  //! \return value.
  QVariant GetValue() const
  {
    return data(Column_Value, Qt::EditRole);
  }

  //! Get datum of the item.
  //! \return pointer to the datum editor
  asiUI_Datum* GetDatum() const
  {
    return m_pDatum;
  }

  //! Get dictionary item.
  //! \return handle to the item's dictionary item.
  Handle(DDS_DicItem) GetDicItem() const
  {
    return m_DicItem;
  }
  
  //! Set computed state for the item.
  //! \param theIsComputed [in] the computed state.
  void SetComputed(const bool theIsComputed)
  {
    m_bComputed = theIsComputed;
  }

  //! Get computed state.
  //! \return computed state.
  bool GetComputed() const
  {
    return m_bComputed;
  }

  void SetEnabled(const bool isOn)
  {
    if ( isOn )
      this->setFlags(this->flags() | Qt::ItemIsEnabled);
    else
      this->setFlags(this->flags() & ~Qt::ItemIsEnabled);
  }

  bool IsEnabled() const
  {
    return (this->flags() & Qt::ItemIsEnabled) > 0;
  }

  virtual QVariant data(int theColumn, int theRole) const;

  virtual void setData(int theColumn, int theRole, const QVariant &theValue);

  bool IsLabel() const;

  bool IsColorEditor() const;

  bool IsBoolEditor() const;

  bool IsPathEditor() const;

private:

  QString valueString(const QVariant& theValue, const bool theLongFmt);

  QString nameString(const QString& theName);
  
private:

  //! datum editor that is used by item delegate
  //! and for converting the input value to a 
  //! display string.
  QPointer<asiUI_Datum> m_pDatum;
  
  //! data dictionary item.
  Handle(DDS_DicItem) m_DicItem;

  //! parameter's units
  QString m_Units;

  //! is item computed
  bool m_bComputed;

  //! node id.
  int m_iNodalId;

  QString m_DisplayName;   //!< displayed name string
  QString m_ShortFmtValue; //!< short formatted display value string
  QString m_LongFmtValue;  //!< long formatted display value string
};

#endif
