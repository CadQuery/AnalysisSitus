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

#ifndef asiUI_DatumViewModel_HeaderFile
#define asiUI_DatumViewModel_HeaderFile

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_HeaderViewAPI.h>

// Qt includes
#pragma warning(push, 0)
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QStandardItem>
#pragma warning(pop)

#pragma warning(disable: 4251) // "Class XXX needs to have dll-interface..."

//! Custom model for datum view hierarchy group.
//! Supports as many features provided by datum view interfaces as possible.
//! By default it supports QStandardItem as data source, but usually is populate with
//! general use asiUI_DatumViewItem classes, which provides automatic display value formatting
//! and local-to-display units value conversion.
class asiUI_EXPORT asiUI_DatumViewModel : public QStandardItemModel, public asiUI_HeaderViewDataApi
{
  Q_OBJECT

public:

  asiUI_DatumViewModel(QObject* theParent = 0);

  virtual ~asiUI_DatumViewModel();

public:

  //! Enumerates item usage domain.
  enum Domain
  {
    Domain_Undefined,
    Domain_View,
    Domain_HeaderSections,
    Domain_HeaderBand
  };

  Domain ItemDomain(const QStandardItem* theItem);

  Qt::Orientation Header(const QStandardItem* theItem) const;

  asiUI_HeaderIndex HeaderIndex(const QStandardItem* theItem) const;

  void SetHeaderItemPrototype(const QStandardItem* theItem);

public: //!< \name Implementation of header data interface methods

  virtual int
    BandRowsCount(const Qt::Orientation theHeader) const;

  virtual int
    SectionCount(const Qt::Orientation theHeader,
                 const asiUI_HeaderIndex& theParent) const;

  virtual asiUI_HeaderIndex
    Section(const Qt::Orientation theHeader,
            const asiUI_HeaderIndex& theParent,
            const int theChildIdx) const;

  virtual asiUI_HeaderIndex
    ParentSection(const Qt::Orientation theHeader,
                  const asiUI_HeaderIndex& theIndex) const;

  virtual void
    SetHeaderItemData(const Qt::Orientation theHeader,
                      const asiUI_HeaderIndex& theIndex,
                      const QVariant& theValue,
                      const int theRole);

  virtual QVariant
    HeaderItemData(const Qt::Orientation theHeader,
                   const asiUI_HeaderIndex& theIndex,
                   const int theRole) const;

  virtual void
    SetHeaderItem(const Qt::Orientation theHeader,
                  const asiUI_HeaderIndex& theIndex,
                  QStandardItem* theItem);

  virtual QStandardItem*
    HeaderItem(const Qt::Orientation theHeader,
               const asiUI_HeaderIndex& theIndex) const;

public: //!< \name Hierarchy & Band header manipulations

  void AddHeaderGroup(const asiUI_HeaderIndex& theGroup, const Qt::Orientation theHeader);

  void RemoveHeaderGroup(const asiUI_HeaderIndex& theGroup, const Qt::Orientation theHeader);

  void SetBandRowCount(const Qt::Orientation theHeader, const int theCount);

  void ResetHierarchyHeader(const Qt::Orientation theHeader);

  void ResetBandHeader(const Qt::Orientation theHeader);

protected slots:

  void OnRowsInserted(const QModelIndex& theParent, int theFirst, int theLast);
  void OnRowsRemoved(const QModelIndex& theParent, int theFirst, int theLast);
  void OnColumnsInserted(const QModelIndex& theParent, int theFirst, int theLast);
  void OnColumnsRemoved(const QModelIndex& theParent, int theFirst, int theLast);

// arranging section ranges in tree-like structure:
private:

  typedef QMap<asiUI_HeaderIndex, QStandardItem*> SectionItemMap;
  enum SectionRangeRelation
  {
    SectionRangeRelation_Overlaps,
    SectionRangeRelation_Comprises,
    SectionRangeRelation_Enters,
    SectionRangeRelation_None
  };

  void addGroup(const asiUI_HeaderIndex& theGroup,
                QStandardItem* theParent,
                SectionItemMap& theSectionTree);

  SectionRangeRelation
    checkRelation(const asiUI_HeaderIndex& theR1,
                  const asiUI_HeaderIndex& theR2);

  void collectItemGroups(const Qt::Orientation theHeader);

  void subItemGroups(QStandardItem* theItemIt, SectionItemMap& theItemMap, int& theSectionIt);

// private fields:
private:

  QStandardItemModel* m_horSectionModel;
  QStandardItemModel* m_verSectionModel;
  QStandardItemModel* m_horBandModel;
  QStandardItemModel* m_verBandModel;
  SectionItemMap m_horSectionIndexMap;
  SectionItemMap m_verSectionIndexMap;
};

//! \ingroup GUI
//!
//! Implementation of selection model which supports advanced header item selection.
//! For need of copy - paste & asiUI_HeaderView class. The selection of header sections
//! is considered to be synchronized with selection of underlying columns. The selection
//! of maintained by this class on its own.
class asiUI_EXPORT asiUI_DatumViewSelectionModel : public QItemSelectionModel,  public asiUI_HeaderViewSelectionApi
{
  Q_OBJECT

public:

  asiUI_DatumViewSelectionModel(QAbstractItemModel* theModel);

// implementation of interface
public:

  virtual const asiUI_HeaderIndexList&
    HeaderSelection(const Qt::Orientation theHeader,
                    const bool theIntersecting = false) const;

public slots:

  virtual void select(const Qt::Orientation theHeader,
                      const asiUI_HeaderIndex& theIndex,
                      QItemSelectionModel::SelectionFlags theCommand);

  virtual void select(const Qt::Orientation theHeader,
                      const QList<asiUI_HeaderIndex>& theIndexes,
                      QItemSelectionModel::SelectionFlags theCommand);

  virtual void select(const QModelIndex& theIndex,
                      QItemSelectionModel::SelectionFlags theCommand);

  virtual void select(const QItemSelection& theSelection,
                      QItemSelectionModel::SelectionFlags theCommand);

  virtual void clear();

  virtual void reset();

signals:

  void selectionChanged(const Qt::Orientation theHeader,
                        const asiUI_HeaderIndexList& theSelected,
                        const asiUI_HeaderIndexList& theDeselected);

protected:

  void emitSelectionChanged(const Qt::Orientation theHeader,
                            const asiUI_HeaderIndexList& theSelection,
                            const asiUI_HeaderIndexList& theOldSelection);

  asiUI_HeaderIndexSet getTopIndexes(const int theSection, const Qt::Orientation theHeader) const;

protected slots:

  void OnSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);
  void OnSelectionInSections(const Qt::Orientation theHeader, const QSet<int>& theSections);
  void OnRowsInserted(const QModelIndex& theParent, int theFirst, int theLast);
  void OnRowsRemoved(const QModelIndex& theParent, int theFirst, int theLast);
  void OnColumnsInserted(const QModelIndex& theParent, int theFirst, int theLast);
  void OnColumnsRemoved(const QModelIndex& theParent, int theFirst, int theLast);

// cached fields for optimization, updated when selection changes
private:

  //! Pure header indexes selection, without intersecting sections
  asiUI_HeaderIndexList m_onlySelectionHor;
  asiUI_HeaderIndexList m_onlySelectionVer;

  //! Selection with intersecting sections
  asiUI_HeaderIndexList m_interSelectionHor;
  asiUI_HeaderIndexList m_interSelectionVer;

  //! State of title selection & intersection with items, band selection.
  QMap<Qt::Orientation, asiUI_HeaderIndexSet> m_titlesSelection;
  QMap<Qt::Orientation, asiUI_HeaderIndexSet> m_titlesIntersection;
  QMap<Qt::Orientation, asiUI_HeaderIndexSet> m_bandsSelection;
};

#pragma warning(default: 4251) // "Class XXX needs to have dll-interface..."

#endif
