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

// asiUI includes
#include <asiUI_DatumViewModel.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

// ----------------------------------------------------------------------------
//             Extended view model with header items support
// ----------------------------------------------------------------------------

//! Constructor.
//! \param theParent [in] the model owner.
asiUI_DatumViewModel::asiUI_DatumViewModel(QObject* theParent)
: QStandardItemModel(theParent)
{
  m_horSectionModel = new QStandardItemModel(this);
  m_verSectionModel = new QStandardItemModel(this);
  m_horBandModel   = new QStandardItemModel(this);
  m_verBandModel   = new QStandardItemModel(this);

  connect(this, SIGNAL( rowsInserted(const QModelIndex&, int, int) ),
          this, SLOT( OnRowsInserted(const QModelIndex&, int, int) ) );
  connect(this, SIGNAL( rowsRemoved(const QModelIndex&, int, int) ),
          this, SLOT( OnRowsRemoved(const QModelIndex&, int, int) ) );
  connect(this, SIGNAL( columnsInserted(const QModelIndex&, int, int) ),
          this, SLOT( OnColumnsInserted(const QModelIndex&, int, int) ) );
  connect(this, SIGNAL( columnsRemoved(const QModelIndex&, int, int) ),
          this, SLOT( OnColumnsRemoved(const QModelIndex&, int, int) ) );
}

//! Destructor.
//! Release allocated for generic items.
asiUI_DatumViewModel::~asiUI_DatumViewModel()
{
}

//! Sets prototype for the header items.
void asiUI_DatumViewModel::SetHeaderItemPrototype(const QStandardItem* thePrototype)
{
  m_horBandModel->setItemPrototype( thePrototype->clone() );
  m_verBandModel->setItemPrototype( thePrototype->clone() );
  m_horSectionModel->setItemPrototype( thePrototype->clone() );
  m_verSectionModel->setItemPrototype( thePrototype->clone() );
}

//! Provides information on the items usage.
//! \param theItem [in] the item.
//! \return items domain within the item model.
asiUI_DatumViewModel::Domain asiUI_DatumViewModel::ItemDomain(const QStandardItem* theItem)
{
  QAbstractItemModel* anItemModel = theItem->model();

  if ( anItemModel == m_horSectionModel || anItemModel == m_verSectionModel )
  {
    return Domain_HeaderSections;
  }

  if ( anItemModel == m_horBandModel || anItemModel == m_verBandModel )
  {
    return Domain_HeaderBand;
  }

  if ( anItemModel == this )
  {
    return Domain_View;
  }

  return Domain_Undefined;
}

//! Query the header for which the item belong to.
//! \param theItem [in] the item.
//! \return header for which the item is belongs to.
Qt::Orientation asiUI_DatumViewModel::Header(const QStandardItem* theItem) const
{
  QAbstractItemModel* anItemModel = theItem->model();

  if ( anItemModel == m_horSectionModel || anItemModel == m_horBandModel )
  {
    return Qt::Horizontal;
  }

  return Qt::Vertical;
}

//! Return header model index for the passed item.
//! \param theItem [in] the item to check.
//! \return header model index.
asiUI_HeaderIndex asiUI_DatumViewModel::HeaderIndex(const QStandardItem* theItem) const
{
  QAbstractItemModel* anItemModel = theItem->model();

  if ( anItemModel == m_horSectionModel )
  {
    return m_horSectionIndexMap.key( const_cast<QStandardItem*>(theItem) );
  }

  if ( anItemModel == m_verSectionModel )
  {
    return m_verSectionIndexMap.key( const_cast<QStandardItem*>(theItem) );
  }

  if ( anItemModel == m_horBandModel || anItemModel == m_verBandModel )
  {
    return asiUI_HeaderIndex::Band( theItem->column(), theItem->row() );
  }

  return asiUI_HeaderIndex();
}

//! Returns number of header model band rows for the specified header.
//! \param theHeader [in] the header.
int asiUI_DatumViewModel::BandRowsCount(const Qt::Orientation theHeader) const
{
  QStandardItemModel* aBandModel =
    theHeader == Qt::Horizontal
      ? m_horBandModel
      : m_verBandModel;

  return aBandModel->rowCount();
}

//! Returns number of subsidiary sections under the passed one.
//! \param theHeader [in] the header.
//! \param theParent [in] the parent.
//! \return number of children section ranges.
int asiUI_DatumViewModel::SectionCount(const Qt::Orientation theHeader,
                                        const asiUI_HeaderIndex& theParent) const
{
  const SectionItemMap& aSectionItemMap = 
    theHeader == Qt::Horizontal
      ? m_horSectionIndexMap
      : m_verSectionIndexMap;

  QStandardItem* aParentItem = aSectionItemMap.value(theParent);
  if ( !aParentItem )
  {
    return 0;
  }

  return aParentItem->columnCount();
}

//! Returns subsidiary section by the specified parent and index.
//! \param theHeader [in] the header.
//! \param theParent [in] the parent.
//! \param theChildIdx [in] the index of the child section in the parent.
//! \return number of children section ranges.
asiUI_HeaderIndex asiUI_DatumViewModel::Section(const Qt::Orientation theHeader,
                                                  const asiUI_HeaderIndex& theParent,
                                                  const int theChildIdx) const
{
  const SectionItemMap& aSectionItemMap = 
    theHeader == Qt::Horizontal
      ? m_horSectionIndexMap
      : m_verSectionIndexMap;

  QStandardItem* aParentItem = aSectionItemMap.value(theParent);
  if ( !aParentItem )
  {
    return asiUI_HeaderIndex();
  }

  return aSectionItemMap.key( aParentItem->child(0, theChildIdx), asiUI_HeaderIndex() );
}

//! Returns parent section of the passed section by its index.
//! \param theHeader [in] the header.
//! \param theIndex [in] the section index.
//! \return parent section index.
asiUI_HeaderIndex asiUI_DatumViewModel::ParentSection(const Qt::Orientation theHeader,
                                                        const asiUI_HeaderIndex& theIndex) const
{
  const SectionItemMap& aSectionItemMap = 
    theHeader == Qt::Horizontal
      ? m_horSectionIndexMap
      : m_verSectionIndexMap;

  QStandardItem* anItem = aSectionItemMap.value(theIndex);
  if ( !anItem )
  {
    return asiUI_HeaderIndex();
  }

  QStandardItem* aParent = anItem->parent();
  if ( !aParent )
  {
    return asiUI_HeaderIndex();
  }

  return aSectionItemMap.key( aParent, asiUI_HeaderIndex() );
}

//! Set data for the header view item. The model supports both hierarchy sections and band header items.
//! The data of bottom level section are initially streamed from datum table model (generated titles, generated
//! labels), but can be overridden with SetHeaderData method.
//! \param theHeader [in] the header.
//! \param theIndex [in] the header view item index.
//! \param theValue [in] the value to set.
//! \param theRole [in] the role of the value.
void asiUI_DatumViewModel::SetHeaderItemData(const Qt::Orientation theHeader,
                                              const asiUI_HeaderIndex& theIndex,
                                              const QVariant& theValue,
                                              const int theRole)
{
  if ( !theIndex.IsValid() )
  {
    return;
  }

  if ( theIndex.IsSection() )
  {
    if ( theIndex.IsUnitarySection() )
    {
      setHeaderData( theIndex.GetSection(), theHeader, theValue, theRole );
      return;
    }

    SectionItemMap& aSectionItemMap = 
      theHeader == Qt::Horizontal
        ? m_horSectionIndexMap
        : m_verSectionIndexMap;
    
    QStandardItem* anItem = aSectionItemMap.value(theIndex);
    if ( !anItem )
    {
      return;
    }

    anItem->setData(theValue, theRole);

    emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );

    return;
  }

  if ( theIndex.IsBand() )
  {
    QStandardItemModel* aBandItemModel =
      theHeader == Qt::Horizontal
        ? m_horBandModel
        : m_verBandModel;

    QModelIndex anIndex = aBandItemModel->index( theIndex.GetBandLine(), theIndex.GetSection() );

    aBandItemModel->setData(anIndex, theValue, theRole);

    emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );
  }
}

//! Queries data for the header view item. The model supports both hierarchy sections and band header items.
//! The data of bottom level section are initially streamed from datum table model (generated titles, generated
//! labels), but can be overridden with SetHeaderData method.
//! \param theHeader [in] the header.
//! \param theIndex [in] the header view item index.
//! \param theRole [in] the role of the value.
//! \return the queried value.
QVariant asiUI_DatumViewModel::HeaderItemData(const Qt::Orientation theHeader,
                                               const asiUI_HeaderIndex& theIndex,
                                               const int theRole) const
{
  if ( !theIndex.IsValid() )
  {
    return QVariant();
  }

  if ( theIndex.IsSection() )
  {
    const SectionItemMap& aSectionItemMap = 
      theHeader == Qt::Horizontal
        ? m_horSectionIndexMap
        : m_verSectionIndexMap;
    
    QStandardItem* anItem = aSectionItemMap.value(theIndex);
    if ( !anItem )
    {
      return QVariant();
    }

    QVariant anItemData = anItem->data(theRole);

    // the header data might be redefined for the section index if not,
    // the data is streamed from datum table (generated titles, generated labels)
    if ( ( anItemData.isNull() || !anItemData.isValid() ) && theIndex.IsUnitarySection() )
    {
      return headerData( theIndex.GetSection(), theHeader, theRole );
    }

    return anItemData;
  }

  if ( theIndex.IsBand() )
  {
    const QStandardItemModel* aBandItemModel =
      theHeader == Qt::Horizontal
        ? m_horBandModel
        : m_verBandModel;

    QModelIndex anIndex = aBandItemModel->index( theIndex.GetBandLine(), theIndex.GetSection() );

    return aBandItemModel->data(anIndex, theRole);
  }

  return QVariant();
}

//! Sets data object for the header view item. The model supports both hierarchy
//! sections and band header items.
//! \param theHeader [in] the header.
//! \param theIndex [in] the header view item index.
//! \param theItem [in] the item to set.
void asiUI_DatumViewModel::SetHeaderItem(const Qt::Orientation theHeader,
                                          const asiUI_HeaderIndex& theIndex,
                                          QStandardItem* theItem)
{
  if ( !theIndex.IsValid() )
  {
    return;
  }

  if ( theIndex.IsSection() )
  {
    if ( theIndex.IsUnitarySection() )
    {
      switch ( theHeader )
      {
        case Qt::Horizontal : setHorizontalHeaderItem( theIndex.GetSection(), theItem );
        case Qt::Vertical   : setVerticalHeaderItem  ( theIndex.GetSection(), theItem );
        default:
          break;
      }
      return;
    }

    SectionItemMap& aSectionItemMap = 
      theHeader == Qt::Horizontal
        ? m_horSectionIndexMap
        : m_verSectionIndexMap;
    
    QStandardItem* anItem = aSectionItemMap.value(theIndex);
    if ( !anItem )
    {
      return;
    }

    QStandardItemModel* aSectionItemModel =
      theHeader == Qt::Horizontal
        ? m_horSectionModel
        : m_verSectionModel;

    int aRow = anItem->row();
    int aCol = anItem->column();

    if ( !anItem->parent() )
    {
      aSectionItemModel->setItem(aRow, aCol, theItem);

      emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );

      return;
    }

    anItem->parent()->setChild(aRow, aCol, theItem);

    emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );

    return;
  }

  if ( theIndex.IsBand() )
  {
    QStandardItemModel* aBandItemModel =
      theHeader == Qt::Horizontal
        ? m_horBandModel
        : m_verBandModel;

    aBandItemModel->setItem( theIndex.GetBandLine(), theIndex.GetSection(), theItem );

    emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );
  }
}

//! Access data object for the header view item.
//! \param theHeader [in] the header.
//! \param theIndex [in] the header view item index.
//! \return the accessed data object.
QStandardItem* asiUI_DatumViewModel::HeaderItem(const Qt::Orientation theHeader,
                                                 const asiUI_HeaderIndex& theIndex) const
{
  if ( !theIndex.IsValid() )
  {
    return NULL;
  }

  if ( theIndex.IsSection() )
  {
    if ( theIndex.IsUnitarySection() )
    {
      switch ( theHeader )
      {
        case Qt::Horizontal : return horizontalHeaderItem( theIndex.GetSection() );
        case Qt::Vertical   : return verticalHeaderItem  ( theIndex.GetSection() );
        default:
          break;
      }
    }

    const SectionItemMap& aSectionItemMap = 
      theHeader == Qt::Horizontal
        ? m_horSectionIndexMap
        : m_verSectionIndexMap;
    
    return aSectionItemMap.value(theIndex, NULL);
  }

  if ( theIndex.IsBand() )
  {
    QStandardItemModel* aBandItemModel =
      theHeader == Qt::Horizontal
        ? m_horBandModel
        : m_verBandModel;

    return aBandItemModel->item( theIndex.GetBandLine(), theIndex.GetSection() );
  }

  return NULL;
}

//! Adds new header group definition. Raises program error if the group is
//! overlaps with other groups and, thus, can not be displayed and added.
//! \param theGroup [in] the section group definition.
//! \param theHeader [in] the header.
void asiUI_DatumViewModel::AddHeaderGroup(const asiUI_HeaderIndex& theGroup,
                                           const Qt::Orientation theHeader)
{
  // the group is already defined
  SectionItemMap& aSectionItemMap = 
    theHeader == Qt::Horizontal
      ? m_horSectionIndexMap
      : m_verSectionIndexMap;

  if ( aSectionItemMap.contains(theGroup) )
  {
    return;
  }

  // check range indexes
  int aSectionCount = theHeader == Qt::Horizontal ? columnCount() : rowCount();

  if ( theGroup.GetFirstSection() < 0 || theGroup.GetLastSection() >= aSectionCount )
  {
    Standard_ProgramError::Raise("invalid section range");
  }

  QStandardItemModel* aSectionItemModel =
    theHeader == Qt::Horizontal
      ? m_horSectionModel
      : m_verSectionModel;

  addGroup( theGroup, aSectionItemModel->invisibleRootItem(), aSectionItemMap );

  collectItemGroups(theHeader);

  emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );
}

//! Removes header group definition.
//! \param theGroup [in] the section group definition.
//! \param theHeader [in] the header orientation.
void asiUI_DatumViewModel::RemoveHeaderGroup(const asiUI_HeaderIndex& theGroup,
                                              const Qt::Orientation theHeader)
{
  SectionItemMap& aSectionItemMap = 
    theHeader == Qt::Horizontal
      ? m_horSectionIndexMap
      : m_verSectionIndexMap;

  if ( !aSectionItemMap.contains(theGroup) )
  {
    return;
  }

  QStandardItem* anItem = aSectionItemMap.value(theGroup);

  QStandardItem* aParent;

  do
  {
    aParent = anItem->parent();
    
    if ( !aParent )
    {
      return;
    }
    
    aParent->removeColumn( anItem->column() );

    anItem = aParent;

  } while ( aParent->columnCount() == 0 );

  collectItemGroups(theHeader);

  emit headerDataChanged( theHeader, 0, theHeader == Qt::Horizontal ? columnCount() - 1 : rowCount() - 1 );
}

//! Sets number of fixed header rows under the passed section.
//! \param theOrientation [in] the header orientation.
//! \param theCount [in] the section count.
//! \return parent section range for the passed one.
void asiUI_DatumViewModel::SetBandRowCount(const Qt::Orientation theOrientation, const int theCount)
{
  QStandardItemModel* aBandModel = theOrientation == Qt::Horizontal
    ? m_horBandModel
    : m_verBandModel;

  aBandModel->setRowCount(theCount);
}

//! Resets hierarchy header structure.
//! \param theHeader [in] the header to reset.
void asiUI_DatumViewModel::ResetHierarchyHeader(const Qt::Orientation theHeader)
{
  SectionItemMap& aSectionItemMap = 
    theHeader == Qt::Horizontal
      ? m_horSectionIndexMap
      : m_verSectionIndexMap;

  aSectionItemMap.clear();

  QStandardItemModel* aHeaderModel =
    theHeader == Qt::Horizontal
    ? m_horSectionModel
    : m_verSectionModel;

  int aHeaderCols = theHeader == Qt::Horizontal ? columnCount() : rowCount();
  aHeaderModel->clear();
  aHeaderModel->setRowCount(1);
  aHeaderModel->setColumnCount(aHeaderCols);
  for ( int aCol = 0; aCol < aHeaderCols; aCol++ )
  {
    aHeaderModel->setItem( 0, aCol, new QStandardItem() );
  }
  collectItemGroups(theHeader);

  int aSectionNb = theHeader == Qt::Horizontal ? columnCount() : rowCount();
  if ( aSectionNb > 0 )
  {
    emit headerDataChanged(theHeader, 0, aSectionNb - 1);
  }
}

//! Resets band header structure.
//! \param theHeader [in] the header to reset.
void asiUI_DatumViewModel::ResetBandHeader(const Qt::Orientation theHeader)
{
  QStandardItemModel* aBandModel = theHeader == Qt::Horizontal
    ? m_horBandModel
    : m_verBandModel;

  int aHeaderCols = theHeader == Qt::Horizontal ? columnCount() : rowCount();
  aBandModel->clear();
  aBandModel->setRowCount(0);
  aBandModel->setColumnCount(aHeaderCols);
  for ( int aCol = 0; aCol < aHeaderCols; aCol++ )
  {
    aBandModel->setItem( 0, aCol, new QStandardItem() );
  }

  int aSectionNb = theHeader == Qt::Horizontal ? columnCount() : rowCount();
  if ( aSectionNb > 0 )
  {
    emit headerDataChanged(theHeader, 0, aSectionNb - 1);
  }
}

//! Synchronize header data state when the new rows are inserted to the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewModel::OnRowsInserted(const QModelIndex& theParent, int theFirst, int theLast)
{
  asiUI_NotUsed(theParent);

  m_verBandModel->insertColumns(theFirst, theLast - theFirst + 1);

  QStandardItem* aAppendItem = m_verSectionModel->invisibleRootItem();

  if ( theFirst != 0 )
  {
    QStandardItem* anItem = m_verSectionIndexMap.value( asiUI_HeaderIndex::UnitarySection(theFirst), NULL );

    if ( anItem && anItem->parent() )
    {
      aAppendItem = anItem->parent();
    }
  }

  aAppendItem->insertColumns( theFirst, theLast - theFirst + 1 );

  for ( int it = theFirst; it <= theLast; ++it )
  {
    aAppendItem->setChild( 0, it, m_verSectionModel->itemPrototype()->clone() );
  }

  collectItemGroups(Qt::Vertical);
}

//! Synchronize header data state when the rows are removed from the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewModel::OnRowsRemoved(const QModelIndex& theParent, int theFirst, int theLast)
{
  asiUI_NotUsed(theParent);

  m_verBandModel->removeRows(theFirst, theLast - theFirst + 1);

  QList<QStandardItem*> anItems;

  for ( int it = theFirst; it <= theLast; ++it )
  {
    anItems.append( m_verSectionIndexMap.value( asiUI_HeaderIndex::UnitarySection(it), NULL ) );
  }

  QList<QStandardItem*>::iterator anIt = anItems.begin();

  for ( ; anIt != anItems.end(); ++anIt )
  {
    QStandardItem* anItem = *anIt;

    if ( !anItem || !anItem->parent() )
    {
      continue;
    }
    
    while ( anItem->columnCount() == 0 )
    {
      QStandardItem* aParent = anItem->parent();

      aParent->removeColumn( anItem->column() );
      
      anItem = aParent;
      
      if ( !anItem->parent() )
      {
        break;
      }
    }
  }
  
  collectItemGroups(Qt::Vertical);
}

//! Synchronize header data state when the new columns are inserted to the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewModel::OnColumnsInserted(const QModelIndex& theParent, int theFirst, int theLast)
{
  asiUI_NotUsed(theParent);

  m_horBandModel->insertColumns(theFirst, theLast - theFirst + 1);

  QStandardItem* aAppendItem = m_horSectionModel->invisibleRootItem();

  if ( theFirst != 0 )
  {
    QStandardItem* anItem = m_horSectionIndexMap.value( asiUI_HeaderIndex::UnitarySection(theFirst), NULL );

    if ( anItem && anItem->parent() )
    {
      aAppendItem = anItem->parent();
    }
  }
  
  aAppendItem->insertColumns( theFirst, theLast - theFirst + 1 );
  
  for ( int it = theFirst; it <= theLast; ++it )
  {
    aAppendItem->setChild( 0, it, m_horSectionModel->itemPrototype()->clone() );
  }
  
  collectItemGroups(Qt::Horizontal);
}

//! Synchronize header data state when the columns are removed from the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewModel::OnColumnsRemoved(const QModelIndex& theParent, int theFirst, int theLast)
{
  asiUI_NotUsed(theParent);

  m_horBandModel->removeRows(theFirst, theLast - theFirst + 1);

  QList<QStandardItem*> anItems;

  for ( int it = theFirst; it <= theLast; ++it )
  {
    anItems.append( m_horSectionIndexMap.value( asiUI_HeaderIndex::UnitarySection(it), NULL ) );
  }

  QList<QStandardItem*>::iterator anIt = anItems.begin();

  for ( ; anIt != anItems.end(); ++anIt )
  {
    QStandardItem* anItem = *anIt;

    if ( !anItem || !anItem->parent() )
    {
      continue;
    }
    
    while ( anItem->columnCount() == 0 )
    {
      QStandardItem* aParent = anItem->parent();

      aParent->removeColumn( anItem->column() );
      
      anItem = aParent;
      
      if ( !anItem->parent() )
      {
        break;
      }
    }
  }

  collectItemGroups(Qt::Horizontal);
}

//! Checks relation between two header section ranges and returns the status.
//! \param theR1 [in] the primary range.
//! \param theR2 [in] the secondary range.
//! \return relation status.
asiUI_DatumViewModel::SectionRangeRelation
  asiUI_DatumViewModel::checkRelation(const asiUI_HeaderIndex& theG1,
                                       const asiUI_HeaderIndex& theG2)
{
  if ( theG1.GetFirstSection() <= theG2.GetFirstSection()
    && theG1.GetLastSection()  >= theG2.GetLastSection() )
  {
    return SectionRangeRelation_Comprises;
  }

  if ( theG1.GetFirstSection() >= theG2.GetFirstSection()
    && theG1.GetLastSection()  <= theG2.GetLastSection() )
  {
    return SectionRangeRelation_Enters;
  }

  if ( theG1.GetFirstSection() < theG2.GetFirstSection()
    && theG1.GetLastSection()  < theG2.GetLastSection() 
    && theG1.GetLastSection()  > theG2.GetFirstSection() )
  {
    return SectionRangeRelation_Overlaps;
  }

  if ( theG1.GetFirstSection() > theG2.GetFirstSection() 
    && theG1.GetLastSection()  > theG2.GetLastSection() 
    && theG1.GetFirstSection() < theG2.GetLastSection() )
  {
    return SectionRangeRelation_Overlaps;
  }

  return SectionRangeRelation_None;
}

//! Adds section to the tree of ranges (recursive).
//! \param theGroup [in] the group to add.
//! \param theParent [in] the parent range of group.
//! \param theItemMap [in] the item map.
void asiUI_DatumViewModel::addGroup(const asiUI_HeaderIndex& theGroup,
                                     QStandardItem* theParent,
                                     SectionItemMap& theItemMap)
{
  QList<QStandardItem*> aComprised;
  
  for ( int col = 0; col < theParent->columnCount(); ++col )
  {
    QStandardItem* aColumnItem = theParent->child(0, col);
    
    asiUI_HeaderIndex aColumnGroup = theItemMap.key(aColumnItem);

    SectionRangeRelation aStatus = checkRelation(theGroup, aColumnGroup);
    switch (aStatus)
    {
      case SectionRangeRelation_None: break;
      
      case SectionRangeRelation_Comprises:
      {
        aComprised.append(aColumnItem);
        break;
      }
      
      case SectionRangeRelation_Enters:
      {
        addGroup(theGroup, aColumnItem, theItemMap);
        return;
      }

      case SectionRangeRelation_Overlaps:
      {
        Standard_ProgramError::Raise("overlapping ranges");
        return;
      }
    }
  }

  if ( aComprised.isEmpty() )
  {
    QList<QStandardItem*> anAdds;
    anAdds.append( theParent->model()->itemPrototype()->clone() );
    theParent->appendColumn(anAdds);
    return;
  }

  int aInsertAt = aComprised.first()->column();

  QList<QStandardItem*>::Iterator anIt = aComprised.begin();

  for ( ; anIt != aComprised.end(); ++anIt )
  {
    theParent->takeColumn( (*anIt)->column() );
  }

  QList<QStandardItem*> anAdds;
  QStandardItem* anAddItem = theParent->model()->itemPrototype()->clone();
  anAdds.append(anAddItem);
  theParent->insertColumn(aInsertAt, anAdds);

  anIt = aComprised.begin();

  for ( ; anIt != aComprised.end(); ++anIt )
  {
    QList<QStandardItem*> anAdds2;
    anAdds2.append( *anIt );
    anAddItem->appendColumn(anAdds2);
  }
}

//! Collects map of header groups represented by hierarchical header model.
//! The header groups (ranges from-to) are associated with appropriate
//! node items in tree hierarchy model.
//! \param theHeader [in] the header.
void asiUI_DatumViewModel::collectItemGroups(const Qt::Orientation theHeader)
{
  QStandardItemModel* aHeaderModel =
    theHeader == Qt::Horizontal
    ? m_horSectionModel
    : m_verSectionModel;

  SectionItemMap& aSectionItemMap =
    theHeader == Qt::Horizontal
    ? m_horSectionIndexMap
    : m_verSectionIndexMap;

  aSectionItemMap.clear();

  QStandardItem* aRootItem = aHeaderModel->invisibleRootItem();

  if ( aRootItem->columnCount() == 0 )
  {
    return;
  }

  int aSectionIt = 0;

  for ( int col = 0; col < aRootItem->columnCount(); ++col )
  {
    subItemGroups( aRootItem->child(0, col), aSectionItemMap, aSectionIt );
  }

  aSectionItemMap.insert( asiUI_HeaderIndex(), aHeaderModel->invisibleRootItem() );
}

//! Collects map of header groups represented by hierarchical header model.
//! The header groups (ranges from-to) are associated with appropriate
//! node items in tree hierarchy model.
//! \param theItemIt [in] the iterated item.
//! \param theItemMap [in] the map to fill.
//! \param theSectionIt [in] the section increment.
void asiUI_DatumViewModel::subItemGroups(QStandardItem* theItemIt, 
                                          SectionItemMap& theItemMap,
                                          int& theSectionIt)
{
  // the bottom-level child of hierarchy
  if ( theItemIt->columnCount() == 0 )
  {
    theItemMap.insert( asiUI_HeaderIndex::UnitarySection(theSectionIt), theItemIt );
    ++theSectionIt;
    return;
  }

  int aBeginSection = theSectionIt;

  for ( int col = 0; col < theItemIt->columnCount(); ++col )
  {
    subItemGroups( theItemIt->child(0, col), theItemMap, theSectionIt );
  }

  int aEndSection = theSectionIt - 1;

  theItemMap.insert( asiUI_HeaderIndex::GroupSection(aBeginSection, aEndSection), theItemIt );
}

// ----------------------------------------------------------------------------
//                 Extended model with header selection support
// ----------------------------------------------------------------------------

//! Constructor.
//! \param theModel [in] the model.
asiUI_DatumViewSelectionModel::asiUI_DatumViewSelectionModel(QAbstractItemModel* theModel)
: QItemSelectionModel(theModel)
{
  m_bandsSelection.insert( Qt::Horizontal, asiUI_HeaderIndexSet() );
  m_bandsSelection.insert( Qt::Vertical,   asiUI_HeaderIndexSet() );
  m_titlesSelection.insert( Qt::Horizontal, asiUI_HeaderIndexSet() );
  m_titlesSelection.insert( Qt::Vertical,   asiUI_HeaderIndexSet() );
  m_titlesIntersection.insert( Qt::Horizontal, asiUI_HeaderIndexSet() );
  m_titlesIntersection.insert( Qt::Vertical,   asiUI_HeaderIndexSet() );

  connect(theModel, SIGNAL( rowsInserted(const QModelIndex&, int, int) ),
          this,     SLOT( OnRowsInserted(const QModelIndex&, int, int) ) );
  connect(theModel, SIGNAL( rowsRemoved(const QModelIndex&, int, int) ),
          this,     SLOT( OnRowsRemoved(const QModelIndex&, int, int) ) );
  connect(theModel, SIGNAL( columnsInserted(const QModelIndex&, int, int) ),
          this,     SLOT( OnColumnsInserted(const QModelIndex&, int, int) ) );
  connect(theModel, SIGNAL( columnsRemoved(const QModelIndex&, int, int) ),
          this,     SLOT( OnColumnsRemoved(const QModelIndex&, int, int) ) );
  connect(this, SIGNAL( selectionChanged(const QItemSelection&, const QItemSelection&) ),
          this, SLOT( OnSelectionChanged(const QItemSelection&, const QItemSelection&) ) );
}

//! Returns current selection of headers, including bands selection. Optionally returns
//! header title sections which crosses the selection (only a parts of items is selected)
//! in addition to selection of band items.
//! \param theHeader [in] the header.
//! \param theIntersecting [in] the intersecting sections.
//! \return header selection.
const asiUI_HeaderIndexList&
  asiUI_DatumViewSelectionModel::HeaderSelection(const Qt::Orientation theHeader,
                                                  const bool theIntersecting) const
{
  if ( theHeader == Qt::Horizontal )
  {
    return theIntersecting ? m_interSelectionHor : m_onlySelectionHor;
  }

  return theIntersecting ? m_interSelectionVer : m_onlySelectionVer;
}

//! Select single index in the header view.
//! \param theHeader [in] the header.
//! \param theIndex [in] the index to select.
//! \param theCommand [in] the selection command.
void asiUI_DatumViewSelectionModel::select(const Qt::Orientation theHeader,
                                            const asiUI_HeaderIndex& theIndex,
                                            QItemSelectionModel::SelectionFlags theCommand)
{
  asiUI_HeaderIndexList aIndexes;
  aIndexes.append(theIndex);
  this->select(theHeader, aIndexes, theCommand);
}

//! Provide selection of the specified item indexes taking into account the selection command.
//! \param theHeader [in] the header.
//! \param theIndexes [in] the indexes.
//! \param theCommand [in] the selection command.
void asiUI_DatumViewSelectionModel::select(const Qt::Orientation theHeader,
                                            const asiUI_HeaderIndexList& theIndexes,
                                            QItemSelectionModel::SelectionFlags theCommand)
{
  const asiUI_HeaderViewDataApi* aHeaderModelApi = dynamic_cast<const asiUI_HeaderViewDataApi*>( model() );

  QSet<int> aTouchedSections;

  asiUI_HeaderIndexSet  aBands  = m_bandsSelection[theHeader];
  asiUI_HeaderIndexList aBefore = HeaderSelection(theHeader, false);

  if ( (theCommand & Clear) != 0 )
  {
    aBands.clear();
  }

  asiUI_HeaderIndexList::const_iterator anIt = theIndexes.constBegin();
  for ( ; anIt != theIndexes.constEnd(); ++anIt )
  {
    const asiUI_HeaderIndex& anIndex = *anIt;

    if ( !anIndex.IsValid() )
    {
      continue;
    }

    if ( anIndex.IsSection() )
    {
      for ( int aSectionIt = anIndex.GetFirstSection(); 
                aSectionIt <= anIndex.GetLastSection();
                ++aSectionIt )
      {
        aTouchedSections += aSectionIt;
      }

      continue;
    }

    asiUI_HeaderIndexList aTouchedBands;

    aTouchedBands += anIndex;

    if ( (theCommand & Columns) != 0 && aHeaderModelApi )
    {
      int aSections = theHeader == Qt::Horizontal ? model()->columnCount() : model()->rowCount();
      for ( int aSectionIt = 0; aSectionIt < aSections; ++aSectionIt )
      {
        aTouchedBands += asiUI_HeaderIndex::Band( aSectionIt, anIndex.GetBandLine() );
      }
    }

    if ( (theCommand & Rows) != 0 && aHeaderModelApi )
    {
      int aBandRows = aHeaderModelApi->BandRowsCount(theHeader);
      for ( int aRowIt = 0; aRowIt < aBandRows; ++aRowIt )
      {
        aTouchedBands += asiUI_HeaderIndex::Band( anIndex.GetSection(), aRowIt );
      }
    }

    asiUI_HeaderIndexList::Iterator aTouchedIt = aTouchedBands.begin();
    for ( ; aTouchedIt != aTouchedBands.end(); ++aTouchedIt )
    {
      bool toSelect = true;
      toSelect &= !(theCommand & Deselect);
      toSelect &=  (theCommand & Select) || ( (theCommand & Toggle) && !aBands.contains(*aTouchedIt) );

      if ( toSelect )
      {
        aBands.insert(*aTouchedIt);
      }
      else if ( aBands.contains(*aTouchedIt) )
      {
        aBands.remove(*aTouchedIt);
      }
    }
  }

  // synchronize selection of columns and rows in view with header section selection
  QItemSelection aViewSelection = selection();

  if ( theCommand & Clear )
  {
    aViewSelection.clear();
  }

  QSet<int>::iterator aSectionIt = aTouchedSections.begin();
  for ( ; aSectionIt != aTouchedSections.end(); ++aSectionIt )
  {
    int aRowCount = model()->rowCount();
    int aColCount = model()->columnCount();
    QModelIndex aTopLeft = theHeader == Qt::Horizontal 
      ? model()->index(0, *aSectionIt)
      : model()->index(*aSectionIt, 0);

    QModelIndex aBotRight = theHeader == Qt::Horizontal
      ? model()->index(aRowCount - 1, *aSectionIt)
      : model()->index(*aSectionIt, aColCount - 1);

    QItemSelection aSectionSelection(aTopLeft, aBotRight);

    aViewSelection.merge(aSectionSelection, theCommand & ~Clear);
  }

  select(aViewSelection, ClearAndSelect);

  m_bandsSelection[theHeader] = aBands;

  if ( theHeader == Qt::Horizontal )
  {
    m_onlySelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    m_interSelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] + 
        m_titlesIntersection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();
  }
  else
  {
    m_onlySelectionVer = 
      ( m_titlesSelection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    m_interSelectionVer = 
      ( m_titlesSelection[Qt::Vertical] + 
        m_titlesIntersection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();
  }

  emitSelectionChanged( theHeader, HeaderSelection(theHeader), aBefore );
}

//! Redefine behavior of view item selection to handle clearing of
//! band header items selection when necessary.
//! \param theIndex [in] the item model index.
//! \param theCommand [in] the selection command.
void asiUI_DatumViewSelectionModel::select(const QModelIndex& theIndex, QItemSelectionModel::SelectionFlags theCommand)
{
  asiUI_HeaderIndexList aHorLast;
  asiUI_HeaderIndexList aVerLast;

  if ( theCommand & QItemSelectionModel::Clear )
  {
    aHorLast = HeaderSelection(Qt::Horizontal);
    aVerLast = HeaderSelection(Qt::Vertical);
    m_bandsSelection[Qt::Horizontal].clear();
    m_bandsSelection[Qt::Vertical].clear();
  }

  if ( !theIndex.isValid() && theCommand == Select )
  {
    this->clear();
    return;
  }

  QItemSelectionModel::select(theIndex, theCommand);

  if ( theCommand & QItemSelectionModel::Clear )
  {
    m_onlySelectionVer = 
      ( m_titlesSelection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    m_interSelectionVer = 
      ( m_titlesSelection[Qt::Vertical] + 
        m_titlesIntersection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    m_onlySelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    m_interSelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] + 
        m_titlesIntersection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    emitSelectionChanged( Qt::Horizontal, HeaderSelection(Qt::Horizontal), aHorLast );
    emitSelectionChanged( Qt::Vertical, HeaderSelection(Qt::Vertical), aVerLast );
  }
}

//! Redefine behavior of view item selection to handle clearing of
//! band header items selection when necessary.
//! \param theIndex [in] the item model index.
//! \param theCommand [in] the selection command.
void asiUI_DatumViewSelectionModel::select(const QItemSelection& theSelection, QItemSelectionModel::SelectionFlags theCommand)
{
  asiUI_HeaderIndexList aHorLast;
  asiUI_HeaderIndexList aVerLast;

  if ( theCommand & QItemSelectionModel::Clear )
  {
    aHorLast = HeaderSelection(Qt::Horizontal);
    aVerLast = HeaderSelection(Qt::Vertical);
    m_bandsSelection[Qt::Horizontal].clear();
    m_bandsSelection[Qt::Vertical].clear();
  }

  if ( theSelection.isEmpty() && theCommand == Select )
  {
    QItemSelectionModel::clear();
  }
  else
  {
    QItemSelectionModel::select(theSelection, theCommand);
  }

  if ( theCommand & QItemSelectionModel::Clear )
  {
    m_onlySelectionVer = 
      ( m_titlesSelection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    m_interSelectionVer = 
      ( m_titlesSelection[Qt::Vertical] + 
        m_titlesIntersection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    m_onlySelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    m_interSelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] + 
        m_titlesIntersection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    emitSelectionChanged( Qt::Horizontal, HeaderSelection(Qt::Horizontal), aHorLast );
    emitSelectionChanged( Qt::Vertical, HeaderSelection(Qt::Vertical), aVerLast );
  }
}

//! Clears selection model.
void asiUI_DatumViewSelectionModel::clear()
{
  asiUI_HeaderIndexList aHorLast = HeaderSelection(Qt::Horizontal);
  asiUI_HeaderIndexList aVerLast = HeaderSelection(Qt::Vertical);

  m_bandsSelection[Qt::Horizontal].clear();
  m_bandsSelection[Qt::Vertical].clear();

  m_onlySelectionVer = 
    ( m_titlesSelection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();

  m_interSelectionVer = 
    ( m_titlesSelection[Qt::Vertical] + 
      m_titlesIntersection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();

  m_onlySelectionHor = 
    ( m_titlesSelection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();

  m_interSelectionHor = 
    ( m_titlesSelection[Qt::Horizontal] + 
      m_titlesIntersection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();

  emitSelectionChanged( Qt::Horizontal, asiUI_HeaderIndexList(), aHorLast );
  emitSelectionChanged( Qt::Vertical, asiUI_HeaderIndexList(), aVerLast );

  QItemSelectionModel::clear();
}

//! Reset selection model.
void asiUI_DatumViewSelectionModel::reset()
{
  asiUI_HeaderIndexList aHorLast = HeaderSelection(Qt::Horizontal);
  asiUI_HeaderIndexList aVerLast = HeaderSelection(Qt::Vertical);

  m_bandsSelection[Qt::Horizontal].clear();
  m_bandsSelection[Qt::Vertical].clear();

  m_onlySelectionVer = 
    ( m_titlesSelection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();

  m_interSelectionVer = 
    ( m_titlesSelection[Qt::Vertical] + 
      m_titlesIntersection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();

  m_onlySelectionHor = 
    ( m_titlesSelection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();

  m_interSelectionHor = 
    ( m_titlesSelection[Qt::Horizontal] + 
      m_titlesIntersection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();

  emitSelectionChanged( Qt::Horizontal, asiUI_HeaderIndexList(), aHorLast );
  emitSelectionChanged( Qt::Vertical, asiUI_HeaderIndexList(), aVerLast );

  QItemSelectionModel::reset();
}

//! Emit selection signal if anything changed and notify of newly selected
//! and deselected items.
//! \param theSelection [in] the current selection.
//! \param theOldSelection [in] the old selection.
void asiUI_DatumViewSelectionModel::emitSelectionChanged(const Qt::Orientation theHeader,
                                                          const asiUI_HeaderIndexList& theSelection,
                                                          const asiUI_HeaderIndexList& theOldSelection)
{
  asiUI_HeaderIndexSet aCurrSet = theSelection.toSet();
  asiUI_HeaderIndexSet aPrevSet = theOldSelection.toSet();

  if ( aCurrSet == aPrevSet )
  {
    return;
  }

  asiUI_HeaderIndexSet aDeselected = aPrevSet.subtract(aCurrSet);
  asiUI_HeaderIndexSet aSelected   = aCurrSet.subtract(aPrevSet);

  emit selectionChanged( theHeader, aSelected.toList(), aDeselected.toList() );
}

//! Return all of the indexes associated with the passed section.
//! \param theSection [in] the section to explore.
//! \param theHeader [in] the header.
asiUI_HeaderIndexSet asiUI_DatumViewSelectionModel::getTopIndexes(const int theSection,
                                                                    const Qt::Orientation theHeader) const
{
  const asiUI_HeaderViewDataApi* aHeaderModelApi = dynamic_cast<const asiUI_HeaderViewDataApi*>( model() );
  if ( !aHeaderModelApi )
  {
    return asiUI_HeaderIndexSet();
  }
  
  asiUI_HeaderIndexSet aSections;
  
  asiUI_HeaderIndex anIndex = asiUI_HeaderIndex::UnitarySection(theSection);
  
  while( anIndex.IsValid() )
  {
    aSections += anIndex;
    anIndex = aHeaderModelApi->ParentSection(theHeader, anIndex);
  }
  
  return aSections;
}

//! Synchronize header selection state when the selection changes.
//! \param theSelected [in] the selected indexes.
//! \param theDeselected [in] the deselected indexes.
void asiUI_DatumViewSelectionModel::OnSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected)
{
  QModelIndexList aSelectedIndexes = theSelected.indexes();
  QModelIndexList aDeselectedIndexes = theDeselected.indexes();

  QSet<int> aTouchedRows;
  QSet<int> aTouchedCols;

  // check only top-level items & indexes
  QModelIndexList::Iterator aSelectedIt = aSelectedIndexes.begin();
  for ( ; aSelectedIt != aSelectedIndexes.end(); ++aSelectedIt )
  {
    QModelIndex& aSelectedIndex = *aSelectedIt;

    if ( aSelectedIndex.parent().isValid() )
    {
      continue;
    }

    aTouchedCols += aSelectedIndex.column();
    aTouchedRows += aSelectedIndex.row();
  }

  // check only top-level items & indexes
  QModelIndexList::Iterator aDeselectedIt = aDeselectedIndexes.begin();
  for ( ; aDeselectedIt != aDeselectedIndexes.end(); ++aDeselectedIt )
  {
    QModelIndex& aDeselectedIndex = *aDeselectedIt;

    if ( aDeselectedIndex.parent().isValid() )
    {
      continue;
    }

    aTouchedCols += aDeselectedIndex.column();
    aTouchedRows += aDeselectedIndex.row();
  }

  if ( !aTouchedRows.isEmpty() )
  {
    asiUI_HeaderIndexList aBefore = HeaderSelection(Qt::Vertical);

    OnSelectionInSections(Qt::Vertical, aTouchedRows);

    m_onlySelectionVer = 
      ( m_titlesSelection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    m_interSelectionVer = 
      ( m_titlesSelection[Qt::Vertical] + 
        m_titlesIntersection[Qt::Vertical] +
        m_bandsSelection[Qt::Vertical] ).toList();

    emitSelectionChanged(Qt::Vertical, HeaderSelection(Qt::Vertical), aBefore);
  }

  if ( !aTouchedCols.isEmpty() )
  {
    asiUI_HeaderIndexList aBefore = HeaderSelection(Qt::Horizontal);

    OnSelectionInSections(Qt::Horizontal, aTouchedCols);

    m_onlySelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    m_interSelectionHor = 
      ( m_titlesSelection[Qt::Horizontal] + 
        m_titlesIntersection[Qt::Horizontal] +
        m_bandsSelection[Qt::Horizontal] ).toList();

    emitSelectionChanged(Qt::Horizontal, HeaderSelection(Qt::Horizontal), aBefore);
  }
}

//! Invalidate changes in header indexes when model selection changes.
//! \param theHeader [in] the header.
//! \param theSections [in] the sections touched by selection.
void asiUI_DatumViewSelectionModel::OnSelectionInSections(const Qt::Orientation theHeader,
                                                           const QSet<int>& theSections)
{
  asiUI_HeaderIndexSet aSelectedIndexes;
  asiUI_HeaderIndexSet aIntersectedIndexes;
  asiUI_HeaderIndexSet aTouchedIndexes;

  QMap<int, bool> aSelectedSections;
  QMap<int, bool> aIntersectingSections;
  QSet<int> aSectionsByBands;

  asiUI_HeaderIndexSet::Iterator aBandsIt = m_bandsSelection[theHeader].begin();

  for ( ; aBandsIt != m_bandsSelection[theHeader].end(); ++aBandsIt )
  {
    aSectionsByBands += (*aBandsIt).GetFirstSection();
  }

  QModelIndex aRoot;

  // get top header sections of all the selected rows and columns
  QSet<int>::ConstIterator aSectionIt = theSections.constBegin();

  while ( aSectionIt != theSections.constEnd() )
  {
    int aSectionId = *aSectionIt;

    bool isSelected = theHeader == Qt::Horizontal
      ? isColumnSelected( aSectionId, aRoot )
      : isRowSelected( aSectionId, aRoot );

    bool isIntersecting = isSelected;
    if ( !isIntersecting )
    {
      if ( aSectionsByBands.contains(aSectionId) )
      {
        isIntersecting = true;
      }
      else
      {
        isIntersecting = theHeader == Qt::Horizontal
          ? columnIntersectsSelection( aSectionId, aRoot )
          : rowIntersectsSelection( aSectionId, aRoot );
      }
    }

    asiUI_HeaderIndexSet aSectionIndexes = getTopIndexes(aSectionId, theHeader);

    aTouchedIndexes += aSectionIndexes;

    if ( isSelected )
    {
      aSelectedIndexes += aSectionIndexes;
    }

    if ( isIntersecting )
    {
      aIntersectedIndexes += aSectionIndexes;
    }

    aSelectedSections[aSectionId]     = isSelected;
    aIntersectingSections[aSectionId] = isIntersecting;

    aSectionIt++;
  }

  // drop top sections which are not covered completely by the selection
  asiUI_HeaderIndexSet::iterator aSelectionIt = aSelectedIndexes.begin();

  while ( aSelectionIt != aSelectedIndexes.end() )
  {
    const asiUI_HeaderIndex& aIndex = *aSelectionIt;

    int aSectionIt2 = aIndex.GetFirstSection();

    for ( ; aSectionIt2 <= aIndex.GetLastSection(); ++aSectionIt2 )
    {
      // not yet checked...
      if ( !aSelectedSections.contains(aSectionIt2) )
      {
        bool isSelected = theHeader == Qt::Horizontal
          ? isColumnSelected( aSectionIt2, aRoot )
          : isRowSelected( aSectionIt2, aRoot );

        aSelectedSections[aSectionIt2] = isSelected;
      }

      if ( aSelectedSections[aSectionIt2] == false )
      {
        break;
      }
    }

    bool isValid = aSectionIt2 > aIndex.GetLastSection();
    if ( !isValid )
    {
      aSelectionIt = aSelectedIndexes.erase(aSelectionIt);
    }
    else
    {
      aSelectionIt++;
    }
  }

  // drop top sections which are not covered completely by the intersection
  asiUI_HeaderIndexSet::iterator aIntersectionIt = aIntersectedIndexes.begin();

  while ( aIntersectionIt != aIntersectedIndexes.end() )
  {
    const asiUI_HeaderIndex& aIndex = *aIntersectionIt;

    int aSectionIt2 = aIndex.GetFirstSection();

    for ( ; aSectionIt2 <= aIndex.GetLastSection(); ++aSectionIt2 )
    {
      // not yet checked...
      if ( !aIntersectingSections.contains(aSectionIt2) )
      {
        bool isIntersecting = theHeader == Qt::Horizontal
          ? columnIntersectsSelection( aSectionIt2, QModelIndex() )
          : rowIntersectsSelection( aSectionIt2, QModelIndex() );

        aIntersectingSections[aSectionIt2] = isIntersecting;
      }

      if ( aIntersectingSections[aSectionIt2] == false )
      {
        break;
      }
    }

    bool isValid = aSectionIt2 > aIndex.GetLastSection();
    if ( !isValid )
    {
      aIntersectionIt = aIntersectedIndexes.erase(aIntersectionIt);
    }
    else
    {
      aIntersectionIt++;
    }
  }

  // clean touched indexes
  m_titlesIntersection[theHeader] -= aTouchedIndexes;
  m_titlesSelection[theHeader]    -= aTouchedIndexes;

  // and add the ones newly selected
  m_titlesSelection[theHeader]    += aSelectedIndexes;
  m_titlesIntersection[theHeader] += aIntersectedIndexes;
}

//! Synchronize header selection state when the new rows are inserted to the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewSelectionModel::OnRowsInserted(const QModelIndex& theParent, int theFirst, int theLast)
{
  // not a top-level
  if ( theParent.isValid() )
  {
    return;
  }

  int aInsertNB = theLast - theFirst + 1;

  asiUI_HeaderIndexSet aModifiedSet;
  asiUI_HeaderIndexSet::Iterator aBandsIt = m_bandsSelection[Qt::Vertical].begin();
  for ( ; aBandsIt != m_bandsSelection[Qt::Vertical].end(); ++aBandsIt )
  {
    const asiUI_HeaderIndex& aBandIndex = (*aBandsIt);
    if ( aBandIndex.GetSection() < theFirst )
    {
      aModifiedSet += aBandIndex;
      continue;
    }

    aModifiedSet += asiUI_HeaderIndex::Band( aBandIndex.GetSection() + aInsertNB, aBandIndex.GetBandLine() );
  }

  m_bandsSelection[Qt::Vertical] = aModifiedSet;

  m_onlySelectionVer = 
    ( m_titlesSelection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();

  m_interSelectionVer = 
    ( m_titlesSelection[Qt::Vertical] + 
      m_titlesIntersection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();
}

//! Synchronize header selection state when the rows are removed from the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewSelectionModel::OnRowsRemoved(const QModelIndex& theParent, int theFirst, int theLast)
{
  // not a top-level
  if ( theParent.isValid() )
  {
    return;
  }

  asiUI_HeaderIndexSet aModifiedSet;
  asiUI_HeaderIndexSet::Iterator aBandsIt = m_bandsSelection[Qt::Vertical].begin();
  for ( ; aBandsIt != m_bandsSelection[Qt::Vertical].end(); ++aBandsIt )
  {
    const asiUI_HeaderIndex& aBandIndex = (*aBandsIt);
    if ( aBandIndex.GetSection() < theFirst || aBandIndex.GetSection() > theLast )
    {
      aModifiedSet += aBandIndex;
    }
  }

  m_bandsSelection[Qt::Vertical] = aModifiedSet;

  m_onlySelectionVer = 
    ( m_titlesSelection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();

  m_interSelectionVer =
    ( m_titlesSelection[Qt::Vertical] + 
      m_titlesIntersection[Qt::Vertical] +
      m_bandsSelection[Qt::Vertical] ).toList();
}

//! Synchronize header selection state when the new columns are inserted to the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewSelectionModel::OnColumnsInserted(const QModelIndex& theParent, int theFirst, int theLast)
{
  // not a top-level
  if ( theParent.isValid() )
  {
    return;
  }

  int aInsertNB = theLast - theFirst + 1;

  asiUI_HeaderIndexSet aModifiedSet;
  asiUI_HeaderIndexSet::Iterator aBandsIt = m_bandsSelection[Qt::Horizontal].begin();
  for ( ; aBandsIt != m_bandsSelection[Qt::Horizontal].end(); ++aBandsIt )
  {
    const asiUI_HeaderIndex& aBandIndex = (*aBandsIt);
    if ( aBandIndex.GetSection() < theFirst )
    {
      aModifiedSet += aBandIndex;
      continue;
    }

    aModifiedSet += asiUI_HeaderIndex::Band( aBandIndex.GetSection() + aInsertNB, aBandIndex.GetBandLine() );
  }

  m_bandsSelection[Qt::Horizontal] = aModifiedSet;

  m_onlySelectionHor =
    ( m_titlesSelection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();

  m_interSelectionHor =
    ( m_titlesSelection[Qt::Horizontal] + 
      m_titlesIntersection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();
}

//! Synchronize header selection state when the columns are removed from the model.
//! \param theParent [in] the parent index to insert at.
//! \param theFirst [in] the first row.
//! \param theLast [in] the last row.
void asiUI_DatumViewSelectionModel::OnColumnsRemoved(const QModelIndex& theParent, int theFirst, int theLast)
{
  // not a top-level
  if ( theParent.isValid() )
  {
    return;
  }

  asiUI_HeaderIndexSet aModifiedSet;
  asiUI_HeaderIndexSet::Iterator aBandsIt = m_bandsSelection[Qt::Horizontal].begin();
  for ( ; aBandsIt != m_bandsSelection[Qt::Horizontal].end(); ++aBandsIt )
  {
    const asiUI_HeaderIndex& aBandIndex = (*aBandsIt);
    if ( aBandIndex.GetSection() < theFirst || aBandIndex.GetSection() > theLast )
    {
      aModifiedSet += aBandIndex;
    }
  }

  m_bandsSelection[Qt::Horizontal] = aModifiedSet;

  m_onlySelectionHor =
    ( m_titlesSelection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();

  m_interSelectionHor =
    ( m_titlesSelection[Qt::Horizontal] + 
      m_titlesIntersection[Qt::Horizontal] +
      m_bandsSelection[Qt::Horizontal] ).toList();
}
