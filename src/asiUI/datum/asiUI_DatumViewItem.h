//-----------------------------------------------------------------------------
// Created on: 22 February 2019
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

#ifndef asiUI_DatumViewItem_HeaderFile
#define asiUI_DatumViewItem_HeaderFile

// asiUI includes
#include <asiUI.h>
#include <asiUI_ItemRoles.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>

// QT includes
#pragma warning(push, 0)
#include <QStandardItem>
#include <QPointer>
#pragma warning(pop)

class asiUI_Datum;

//! The custom item for datum views.
//! Makes data display and editor contents to be the same.
//! The item contains three values:
//! - displayed and formatted string, accessible via Qt::DisplayRole.
//! - value in local unit system, accessible via Qt::EditRole.
//! - value in SI unit system, accessible via asiUI_DatumTableItem::SIRole
class asiUI_EXPORT asiUI_DatumViewItem : public QStandardItem
{
public:

  //! Enumerates supported options for formatting.
  enum Formatting
  {
    Formatting_Short,
    Formatting_Long
  };

public:

  //! Enumerates "built-in" patterns for composing display
  //! string produced by combining value with any of the
  //! datum properties: e.g. "%label %value [%units]"
  enum Pattern
  {
    Pattern_LabelOnly,
    Pattern_ValueOnly,
    Pattern_UnitsOnly,
    Pattern_LabelAndValue,
    Pattern_LabelAndUnits,
    Pattern_All
  };

  static QString DefaultPattern(const Pattern thePattern);

public:

  asiUI_DatumViewItem();

// data accessors
public:

  void SetDisplayData(const QVariant& theValue);

  QVariant GetDisplayData() const;

  void SetDisplayPattern(const QString& thePattern);

  QString GetDisplayPattern() const;

  void SetEditData(const QVariant& theValue);

  QVariant GetEditData() const;

  void SetModelData(const QVariant& theValue);

  QVariant GetModelData() const;

public:

  virtual bool CheckMinMax() const;

  virtual void Update();

  virtual QStandardItem* clone() const;

  virtual QVariant data(int role = Qt::UserRole + 1) const;

  virtual void setData(const QVariant &theValue, int theRole = Qt::UserRole + 1);

protected:

  virtual QVariant fromModel(const QVariant& theValue, const Handle(asiAlgo_DictionaryItem)& theDictItem) const;

  virtual QVariant toModel(const QVariant& theValue, const Handle(asiAlgo_DictionaryItem)& theDictItem) const;

  virtual QString format(const QVariant& theValue,
                         const Formatting theFormat,
                         const Handle(asiAlgo_DictionaryItem)& theDictItem) const;

  QVariant listItemById(const int theId, const Handle(asiAlgo_DictionaryItem)& theDictItem) const;

  int listIdByItem(const QVariant& theValue, const Handle(asiAlgo_DictionaryItem)& theDictItem) const;

  QString fromPattern(const QString& thePattern,
                      const QString& theValue,
                      const QString& theLabel,
                      const QString& theUnits) const;

protected:

  Handle(asiAlgo_DictionaryItem) dictionaryItem() const;

  QVariant m_ModelValue;
  QVariant m_LocalValue;
  QVariant m_ShortFmtValue;
  QVariant m_LongFmtValue;
  QString  m_Pattern;
};

#endif
