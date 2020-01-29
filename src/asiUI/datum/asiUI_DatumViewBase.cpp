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
#include <asiUI_Common.h>
#include <asiUI_DatumViewBase.h>
#include <asiUI_DatumViewItem.h>
#include <asiUI_ItemRoles.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>
#include <asiAlgo_Dictionary.h>

// Qt includes
#pragma warning(push, 0)
#include <QKeyEvent>
#include <QApplication>
#include <QAbstractProxyModel>
#include <QStandardItem>
#include <QTreeView>
#include <QTableView>
#include <QAbstractProxyModel>
#pragma warning(pop)

//! Default constructor.
//! Initialization of common functionality.
//! \param theFactory [in] the widget factory to instantiate datum editors.
//! \param theParent [in] parent widget to take ownership of the view.
template <typename ParentItemViewClass>
asiUI_DatumViewBase<ParentItemViewClass>::asiUI_DatumViewBase(QWidget* theParent)
: ParentItemViewClass(theParent),
  m_bShowUnits(true)
{
}

//! Get flag indicating whether the datum editor's units should be
//! displayed in headers.
//! The column(row) headers use dictionary labels of the column(row) datums.
//! If the headers uses user-defined labels, the units are not shown.
//! \return the boolean flag.
template <typename ParentItemViewClass>
bool asiUI_DatumViewBase<ParentItemViewClass>::IsShowUnitsInLabel() const
{
  return m_bShowUnits;
}

//! Set flag indicating whether the datum editor's units should be
//! displayed in headers.
//! If the header uses user-defined labels, the units are not shown.
//! The column(row) headers use dictionary labels of the column(row) datums.
//! By default, this option is switched on.
//! \param theIsShown [in] the boolean flag.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::SetShowUnitsInLabel(const bool theIsShown)
{
  m_bShowUnits = theIsShown;
  refreshUnitsInLabels();
}

//! Access value conversion datum instance associated with the specified
//! model index. This methods is used by data model layer to access datum
//! as value formatting and conversion tool.
//! \param theIndex [in] the model index.
template <typename ParentItemViewClass>
asiUI_Datum* asiUI_DatumViewBase<ParentItemViewClass>::AccessViewDatum(const QModelIndex& theIndex)
{
  if ( !theIndex.isValid() )
    return nullptr;

  actualizeDatum(theIndex);

  QString aDatumId = m_ActualDatumMap.value( theIndex, QString() );
  if ( aDatumId.isNull() )
    return nullptr;

  return m_DatumEntityMap.value(aDatumId, nullptr);
}

//! Return datum dictionary identification string assigned
//! for the specified item model index.
//! \param theIndex [in] the item model index.
//! \return data dictionary id, or empty string if no datum assigned.
template <typename ParentItemViewClass>
QString asiUI_DatumViewBase<ParentItemViewClass>::DatumId(const QModelIndex& theIndex) const
{
  return this->getItemDatum(theIndex);
}

//! Return datum dictionary identification string assigned for the specified header index.
//! \param theHeader [in] the header type.
//! \param theIndex [in] the item model index.
//! \return data dictionary id, or empty string if no datum assigned.
template <typename ParentItemViewClass>
QString asiUI_DatumViewBase<ParentItemViewClass>::HeaderDatumId(const Qt::Orientation theHeader,
                                                                 const asiUI_HeaderIndex& theIndex) const
{
  return this->getHeaderDatum(theHeader, theIndex);
}

//! Access widget factory to create datum instances.
template <typename ParentItemViewClass>
Handle(asiUI_WidgetFactory) asiUI_DatumViewBase<ParentItemViewClass>::AccessFactory() const
{
  return this->widgetFactory();
}

//! Enable editing items by a single click.
//! By default, the edit by a single click is enabled.
//! \param theIsEdit [in] boolean flag indicating whether the single click
//! editing is enabled or not.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::SetEditByClick(const bool theIsEdit) const
{
  m_pEventHandler->EnableEditByClickHandling(theIsEdit);
}

//! Return signal event handler for attaching unit change signal by widget factory.
//! \return the associated event handler.
template <typename ParentItemViewClass>
asiUI_DatumViewEventHandler* asiUI_DatumViewBase<ParentItemViewClass>::GetEventHandler() const
{
  return m_pEventHandler;
}

//! Process notification from datum view delegate notifying that the
//! editor has been created and used.
//! \param theEditor [in] the created editor.
//! \param theIndex [in] the edited model index.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::NotifyDelegateEditorCreated(
                                              QWidget* theEditor,
                                              const QModelIndex& theIndex)
{
  m_EditorMap.insert(theIndex, theEditor);
  this->showStatusTip(theIndex);
  this->emitValueChanging(theIndex, theEditor);
}

//! Process notification from datum view delegate notifying
//! that the custom selector is triggered.
//! \param theIndex [in] the edited model index.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::NotifyCustomSelectorTriggered(
                                              const QModelIndex& theIndex)
{
  this->emitCustomSelectorTriggered(theIndex);
}

//! Notification method, tells that local system of units has changed, so the
//! contents of datum view should be refreshed.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::NotifyUnitsChanged()
{
  this->updateItems(true);
  this->updateHeaderItems();
  this->refreshUnitsInLabels();
}

//! Checks if there any proxy set for the datum view internal data.
//! \return TRUE if proxy model was set.
template <typename ParentItemViewClass>
bool asiUI_DatumViewBase<ParentItemViewClass>::IsProxyModel() const
{
  if ( qobject_cast<QAbstractProxyModel*>( ParentItemViewClass::model() ) )
  {
    return true;
  }

  return false;
}

//! Set new item prototype.
//! Recursively changes prototype of already created model items.
//! \param thePrototype [in] the item prototype.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::setItemPrototype(
                                                const asiUI_DatumViewItem* thePrototype)
{
  asiUI_DatumViewModel* anItemModel = itemModel();
  anItemModel->setItemPrototype(thePrototype);
  anItemModel->SetHeaderItemPrototype(thePrototype);
  resetItemPrototype(anItemModel->invisibleRootItem(), thePrototype);
}

//! Access item from the specified index.
//! \param theIndex [in] the index to access the item.
//! \return item under the specified model index.
template <typename ParentItemViewClass>
asiUI_DatumViewItem* asiUI_DatumViewBase<ParentItemViewClass>::accessItem(
                                                       const QModelIndex& theIndex) const
{
  asiUI_DatumViewModel* aModel = itemModel();
  QStandardItem* anItem = aModel->itemFromIndex(theIndex);
  if ( theIndex.isValid() && anItem == nullptr )
  {
    aModel->setData( theIndex, QVariant() ); // init item internally
    anItem = aModel->itemFromIndex(theIndex);
  }

  return dynamic_cast<asiUI_DatumViewItem*>(anItem);
}

//! Get interface to implementation of internal item model.
//! \return pointer to data model of appropriate type.
template <typename ParentItemViewClass>
asiUI_DatumViewModel* asiUI_DatumViewBase<ParentItemViewClass>::itemModel() const
{
  return itemModel( ParentItemViewClass::model() );
}

//! Access item model wrapped by the given proxy model.
//! \param theModel [in] the proxy model.
//! \return item model or this if the given model is not a proxy.
template <typename ParentItemViewClass>
asiUI_DatumViewModel* asiUI_DatumViewBase<ParentItemViewClass>::itemModel(QAbstractItemModel* theModel) const
{
  asiUI_DatumViewModel* aModelStandard = qobject_cast<asiUI_DatumViewModel*>(theModel);
  if ( aModelStandard )
  {
    return aModelStandard;
  }

  QAbstractProxyModel* aQtModernProxy = qobject_cast<QAbstractProxyModel*>(theModel);
  if ( aQtModernProxy )
  {
    return qobject_cast<asiUI_DatumViewModel*>( aQtModernProxy->sourceModel() );
  }

  return nullptr;
}

//! Returns source index from the proxy model.
//! \param theProxy [in] the proxy index.
//! \return index source.
template <typename ParentItemViewClass>
QModelIndex asiUI_DatumViewBase<ParentItemViewClass>::sourceIndex(const QModelIndex& theProxy) const
{
  if ( theProxy.model() == itemModel() )
  {
    return theProxy;
  }

  if ( theProxy.model() != ParentItemViewClass::model() )
  {
    return theProxy;
  }

  QAbstractProxyModel* aQtModernProxy = qobject_cast<QAbstractProxyModel*>( ParentItemViewClass::model() );
  if ( aQtModernProxy )
  {
    return aQtModernProxy->mapToSource(theProxy);
  }

  return theProxy;
}

//! Initialize model and internal delegate and item types.
//! Call this method from constructor to set initial model, delegate type and prototype.
//! \param theCreator [in] the widget factory.
//! \param theItemModel [in] the model for data items.
//! \param thePrototype [in] the model items prototype.
//! \param theDelegate [in] the model editing delegate.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::init(const Handle(asiUI_WidgetFactory)& theCreator,
                                                     asiUI_DatumViewModel* theItemModel,
                                                     asiUI_DatumViewItem* thePrototype,
                                                     asiUI_DatumViewDelegate* theDelegate)
{
  m_pWidgetFactory = theCreator;
  m_pEventHandler = new asiUI_DatumViewEventHandler(this);

  ParentItemViewClass::setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  ParentItemViewClass::setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  ParentItemViewClass::setSelectionBehavior(QAbstractItemView::SelectItems);
  ParentItemViewClass::setSelectionMode(QAbstractItemView::ExtendedSelection);

  ParentItemViewClass::setTabKeyNavigation(true);
  ParentItemViewClass::setStyle( new asiUI_ViewStyleProxy(nullptr) );

  ParentItemViewClass::setEditTriggers(QAbstractItemView::EditKeyPressed |
                                       QAbstractItemView::DoubleClicked  |
                                       QAbstractItemView::AnyKeyPressed);

  SetEditByClick(false);

  if ( theItemModel )
  {
    theItemModel->setParent(this);
    setModel(theItemModel);
  }

  if ( theDelegate )
  {
    theDelegate->setParent(this);
    ParentItemViewClass::setItemDelegate(theDelegate);
  }

  if ( thePrototype )
  {
    setItemPrototype(thePrototype);
  }
}

//! Sets model and installs expanded header selection model.
//! \param theModel [in] the model.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::setModel(QAbstractItemModel* theModel)
{
  ParentItemViewClass::setModel(theModel);

  asiUI_DatumViewSelectionModel*
    aSelectionModel = new asiUI_DatumViewSelectionModel( ParentItemViewClass::model() );

  aSelectionModel->setParent(this);

  ParentItemViewClass::connect( ParentItemViewClass::model(), SIGNAL( destroyed() ), aSelectionModel, SLOT( deleteLater() ) );

  ParentItemViewClass::setSelectionModel(aSelectionModel);
}

//! Get value for datum view item's index.
//! If the datum is assigned, the returned value is the one converted to the SI 
//! unit system. This value can differ with the displayed string.
//! \param theIndex [in] the item model index.
//! \return SI unit system value or 0 if invalid index specified.
template <typename ParentItemViewClass>
QVariant asiUI_DatumViewBase<ParentItemViewClass>::getValue(const QModelIndex& theIndex) const
{
  QStandardItem* anItem = itemModel()->itemFromIndex(theIndex);
  if ( !anItem )
    return QVariant();

  return anItem->data(DatumViewRole_ModelValue);
}

//! Set value for datum view item's index.
//! If the datum is assigned, the value will be considered as the one converted to the
//! SI unit system. This value can differ with the displayed string.
//! \param theIndex [in] the item model index.
//! \param theValue [in] the SI unit system value.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::setValue(const QModelIndex& theIndex,
                                                         const QVariant& theValue)
{
  if ( !theIndex.isValid() )
    return;

  itemModel()->setData(theIndex, theValue, DatumViewRole_ModelValue);
}

//! Get displayed string for datum view item's index.
//! The value corresponds to the editor's contents that corresponds to the value
//! converted to local unit system.
//! \param theIndex [in] the item model index.
//! \return displayed string or null string if invalid index is specified
template <typename ParentItemViewClass>
QString asiUI_DatumViewBase<ParentItemViewClass>::getString(const QModelIndex& theIndex) const
{
  QStandardItem* anItem = itemModel()->itemFromIndex(theIndex);
  if ( !anItem )
    return QString();

  return anItem->data(Qt::DisplayRole).toString();
}

//! Set displayed string for datum view item's index.
//! The value corresponds to the editor's contents which is a value converted
//! to local unit system.
//! \param theIndex [in] the item model index.
//! \param theString [in] the string to set.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::setString(const QModelIndex& theIndex,
                                                          const QString& theString)
{
  if ( !theIndex.isValid() )
    return;

  itemModel()->setData(theIndex, theString, Qt::DisplayRole);
}

//! Access widget factory instance.
//! \return shared pointer to the widget factory instance.
template <typename ParentItemViewClass>
Handle(asiUI_WidgetFactory) asiUI_DatumViewBase<ParentItemViewClass>::widgetFactory() const
{
  return m_pWidgetFactory;
}

//! Update every item state, displayed and internal data.
//! \param theSkipEdited [in] the boolean flag that defines whether
//! the edited item should be skipped or not.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::updateItems(const bool theSkipEdited)
{
  QModelIndexList aIndexes;
  collectIndexes( QModelIndex(), aIndexes, true );
  updateItems(aIndexes, theSkipEdited);
}

//! Update every item of the extended header views.
//! The method is usually called when overall refresh is
//! requested, e.g. on changing system of units.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::updateHeaderItems()
{
  asiUI_DatumViewModel* aItemModel = itemModel();

  for ( int aCol = 0; aCol < aItemModel->columnCount(); ++aCol )
  {
    // update items of horizontal band header view
    for ( int aBandRow = 0; aBandRow < aItemModel->BandRowsCount(Qt::Horizontal); ++aBandRow )
    {
      asiUI_HeaderIndex aIndex = asiUI_HeaderIndex::Band(aCol, aBandRow);
      asiUI_DatumViewItem* aDatumItem =
        dynamic_cast<asiUI_DatumViewItem*>( aItemModel->HeaderItem(Qt::Horizontal, aIndex) );
      
      if ( !aDatumItem )
      {
        continue;
      }

      aDatumItem->Update();
    }
  }

  for ( int aRow = 0; aRow < aItemModel->rowCount(); ++aRow )
  {
    // update items of vertical band header view
    for ( int aBandRow = 0; aBandRow < aItemModel->BandRowsCount(Qt::Vertical); ++aBandRow )
    {
      asiUI_HeaderIndex aIndex = asiUI_HeaderIndex::Band(aRow, aBandRow);
      asiUI_DatumViewItem* aDatumItem =
        dynamic_cast<asiUI_DatumViewItem*>( aItemModel->HeaderItem(Qt::Vertical, aIndex) );
      
      if ( !aDatumItem )
      {
        continue;
      }
      
      aDatumItem->Update();
    }
  }
}

//! Update item state, displayed and internal data for the specified
//! model indexes list.
//! \param theIndexes [in] the list of indexes to update.
//! \param theSkipEdited [in] the boolean flag that defines whether
//! the edited item should be skipped or not.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::updateItems(
                                                  const QModelIndexList& theIndexes,
                                                  const bool theSkipEdited)
{
  // Increase performance by avoiding view updates when changing single item
  bool isModelBlocked = ParentItemViewClass::model()->blockSignals(true);

  QModelIndexList::ConstIterator anIt = theIndexes.constBegin();
  for ( ; anIt != theIndexes.constEnd(); anIt++ )
  {
    asiUI_DatumViewItem* anItem = accessItem(*anIt);
    if ( !anItem )
      continue;

    actualizeDatum(*anIt);

    if ( !theSkipEdited || !m_EditorMap.contains(*anIt) )
      anItem->Update();
  }

  // Unblock model and update view with single call
  ParentItemViewClass::model()->blockSignals(isModelBlocked);
}

//! Generate appropriate label string by the 
//! dictionary id and title string specified.
//! \param theDictId [in] dictionary id of associated editor.
//! \param theTitle [in] the user-override title.
//! \return generated label string.
template <typename ParentItemViewClass>
QString asiUI_DatumViewBase<ParentItemViewClass>::labelString(
                                                     const QString& theDictId,
                                                     const QString& theTitle) const
{
  bool isUnits = IsShowUnitsInLabel();

  QString aLabel = theTitle;
  QString aUnits = QString();
  if ( theTitle.isEmpty() || isUnits )
  {
    Handle(asiAlgo_DictionaryItem) anItem = 
      asiAlgo_Dictionary::GetDictionaryItem( QStr2ExtStr(theDictId) );

    if ( theTitle.isNull() )
    {
      aLabel = anItem.IsNull() ? 
        QString() : ExtStr2QStr( anItem->GetLabel() );
    }

    if ( isUnits && !anItem.IsNull() )
    {
      aUnits = AsciiStr2QStr( anItem->GetUnits() );
    }
  }

  if ( !aUnits.isEmpty() )
    aLabel += " [" + aUnits + "]";

  return aLabel;
}

//! Actualize datum conversion entity created for internal data conversion
//! for the specified model index.
//! Create new datums if not yet instantiated and removes outdated datums.
//! \param theIndex [in] the item model index to refresh the datum for.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::actualizeDatum(const QModelIndex& theIndex)
{
  QString aRequiredDatum = getItemDatum(theIndex);
  QString aPrevDatum = m_ActualDatumMap.value(theIndex, QString() );

  // the same datum as already registered
  if ( !aRequiredDatum.isNull() && aPrevDatum == aRequiredDatum )
  {
    return;
  }

  // --------------------------------------------------------------------------
  //     this code executes only if datum definition is outdated, or changed
  // --------------------------------------------------------------------------

  // remove old, outdated value conversion datum
  if ( !aPrevDatum.isNull() && aPrevDatum != aRequiredDatum )
  {
    m_ActualDatumMap.remove(theIndex);

    Datum2IndexList::Iterator anIt = m_ActualDatumRefs.find(aPrevDatum);
    if ( anIt != m_ActualDatumRefs.end() )
    {
      // decrease references
      IndexList& aIndexList = *anIt;
      aIndexList.removeAll(theIndex);

      // if no more references - remove
      if ( aIndexList.size() == 0 )
        m_ActualDatumRefs.remove(aPrevDatum);
    }

    // release memory if not needed
    if ( !m_ActualDatumRefs.contains(aPrevDatum) 
      &&  m_DatumEntityMap.contains(aPrevDatum) )
    {
      m_DatumEntityMap.take(aPrevDatum)->deleteLater();
    }
  }

  // datum of the required type is already created,
  // just add a new reference
  if ( m_DatumEntityMap.contains(aRequiredDatum) )
  {
    m_ActualDatumMap.insert(theIndex, aRequiredDatum);
    m_ActualDatumRefs[aRequiredDatum].append(theIndex);
    return;
  }

  // --------------------------------------------------------------------------
  //     this block is case of creating new value conversion datum instance
  // --------------------------------------------------------------------------

  // create new instance, register and return it
  asiUI_Datum* aNewDatum = widgetFactory()->CreateEditor(aRequiredDatum, this, asiUI_Datum::Control);
  if ( aNewDatum )
  {
    aNewDatum->Hide();

    m_ActualDatumMap.insert(theIndex, aRequiredDatum);
    m_ActualDatumRefs.insert(aRequiredDatum, IndexList() );
    m_ActualDatumRefs[aRequiredDatum].append(theIndex);
    m_DatumEntityMap.insert(aRequiredDatum, aNewDatum);
  }
}

//! Invoked when data model data changes.
//! \param theTopLeft [in] the top left index of the changed range.
//! \param theBottomRight [in] the bottom right index of the changed range.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::dataChanged(
                                                  const QModelIndex &theTopLeft,
                                                  const QModelIndex &theBottomRight,
                                                  const QVector<int> &roles)
{
  ParentItemViewClass::dataChanged(theTopLeft, theBottomRight, roles);

  QModelIndex aSrcIndex = sourceIndex(theTopLeft);

  this->emitValueChanged( aSrcIndex, aSrcIndex.data(DatumViewRole_ModelValue) );
}

//! Unregister editors.
//! \param theEditor [in] the editor.
//! \param theEndHint [in] closing hint.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::closeEditor(
                                           QWidget* theEditor,
                                           QAbstractItemDelegate::EndEditHint theEndHint)
{
  QModelIndex aCurIndex;
  Index2EditorMap::Iterator aMapIt = m_EditorMap.begin();
  for ( ; aMapIt != m_EditorMap.end(); aMapIt++ )
  {
    if ( theEditor == aMapIt.value() )
    {
      aCurIndex = aMapIt.key();
      m_EditorMap.erase(aMapIt);
      break;
    }
  }

  ParentItemViewClass::closeEditor(theEditor, theEndHint);
  
  this->hideStatusTip(aCurIndex);
  this->emitDatumClosed(aCurIndex, theEditor);
}

//! Emit selection change signal to listeners.
//! \param theSelected [in] the selected items.
//! \param theDeselected [in] the deselected items.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::selectionChanged(
                                           const QItemSelection& theSelected,
                                           const QItemSelection& theDeselected)
{
  QAbstractItemView::selectionChanged(theSelected, theDeselected);
  this->emitSelectionChanged(theSelected, theDeselected);
}

//! Handle backspace key appropriately for editing purposes.
//! \param theEvent [in] the event.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::keyPressEvent(QKeyEvent *theEvent)
{
  switch ( theEvent->key() )
  {
    case Qt::Key_Backspace :
    {
      if ( ParentItemViewClass::edit( ParentItemViewClass::currentIndex(), QAbstractItemView::AnyKeyPressed, theEvent ) )
        theEvent->ignore();
      else
        theEvent->accept();
      break;
    }

    case Qt::Key_Enter :
    case Qt::Key_Return :
    {
      if ( ParentItemViewClass::state() != QAbstractItemView::EditingState )
      {
        if ( theEvent->modifiers() == Qt::ShiftModifier )
          ParentItemViewClass::setCurrentIndex( ParentItemViewClass::moveCursor( QAbstractItemView::MovePrevious, Qt::NoModifier) );
        else
          ParentItemViewClass::setCurrentIndex( ParentItemViewClass::moveCursor( QAbstractItemView::MoveNext, Qt::NoModifier) );
      }
      break;
    }

    default:
      ParentItemViewClass::keyPressEvent(theEvent);
  }
}

//! Triggered when modification starts on any cell of datum view.
//! \param theIndex [in] the index of triggered item.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::showStatusTip(const QModelIndex& theIndex)
{
  QString aTip = itemModel()->data(theIndex, DatumViewRole_StatusUnits).toString();

  QStatusTipEvent aTipEvent(aTip);
  QApplication::sendEvent(this->parent(), &aTipEvent);
}

//! Triggered when modification finishes on any cell of datum view.
//! \param theIndex [in] the index of triggered item.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::hideStatusTip(const QModelIndex& asiUI_NotUsed(theIndex))
{
  QString aTip = QString::null;
  QStatusTipEvent aTipEvent(aTip);
  QApplication::sendEvent(this->parent(), &aTipEvent);
}

//! Reset internal data and table state.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::Reset()
{
  if ( itemModel() )
  {
    itemModel()->clear();
  }

  ParentItemViewClass::reset();
}

//! Forces update of data loaded from model.
//! You can use this method in pair with blocked
//! update of item model.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::RefreshData()
{
  this->updateEditorData();
  this->doItemsLayout();
}

//! Resets item prototypes of child items created for the specified root.
//! \param theRoot [in] the root item.
//! \param thePrototype [in] the prototype to replace with.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::resetItemPrototype(
                                      QStandardItem* theRoot,
                                      const asiUI_DatumViewItem* thePrototype) const
{
  QStandardItem* aRootToReplace = thePrototype->clone();

  int aRows = theRoot->rowCount();
  int aCols = theRoot->columnCount();

  aRootToReplace->setRowCount(aRows);
  aRootToReplace->setColumnCount(aCols);

  for ( int aRowIt = 0; aRowIt < aRows; aRowIt++ )
  {
    for ( int aColIt = 0; aColIt < aCols; aColIt++ )
    {
      // change item type to prototype first
      QStandardItem* aToReset = theRoot->takeChild(aRowIt, aColIt);

      if ( !aToReset )
        continue;

      // replace bottom level childs directly
      if ( !aToReset->hasChildren() )
      {
        theRoot->setChild( aRowIt, aColIt, thePrototype->clone() );
      }
      else
      {
        resetItemPrototype(aToReset, thePrototype);
      }

      // reparent child to new prototype root
      aRootToReplace->setChild(aRowIt, aColIt, aToReset);
    }
  }

  // replace root where possible
  QStandardItem* aRootParent = theRoot->parent();
  if ( !aRootParent )
    return;

  int aRootRow = theRoot->row();
  int aRootCol = theRoot->column();
  
  aRootParent->takeChild(aRootRow, aRootCol);
  aRootParent->setChild(aRootRow, aRootCol, aRootToReplace);
}

//! Collect list of model indexes persistent in data model.
//! \param theParentIndex [in] the parent index to start from.
//! \param theIndexes [out] list of collected indexes.
//! \param theIsRecursive [in] boolean flag defines whether the
//! indexes should be collected for subchildrens or not.
template <typename ParentItemViewClass>
void asiUI_DatumViewBase<ParentItemViewClass>::collectIndexes(
                                      const QModelIndex& theParentIndex,
                                      QModelIndexList& theIndexes,
                                      const bool theIsRecursive) const
{
  QStandardItem* aParentItem = theParentIndex.isValid() ?
    itemModel()->itemFromIndex(theParentIndex) :
    itemModel()->invisibleRootItem();

  const int aRows = aParentItem->rowCount();
  const int aCols = aParentItem->columnCount();
  for ( int aRowIt = 0; aRowIt < aRows; aRowIt++ )
  {
    for ( int aColIt = 0; aColIt < aCols; aColIt++ )
    {
      QModelIndex aChildAt = aParentItem->child(aRowIt, aColIt)->index();

      theIndexes.append(aChildAt);

      if ( theIsRecursive )
        collectIndexes(aChildAt, theIndexes, true);
    }
  }
}

// ============================================================================
//         Base layer signals and events handler
// ============================================================================

//! Default constructor.
//! \param theHandled [in] the handled item view.
asiUI_DatumViewEventHandler::asiUI_DatumViewEventHandler(QAbstractItemView* theHandled)
: QObject(theHandled)
{
  //
}

//! Enabled or disable edit by click mode handling.
//! \param theIsEnabled [in] boolean flag that turns on/off the handling state.
void asiUI_DatumViewEventHandler::EnableEditByClickHandling(const bool theIsEnabled)
{
  QAbstractItemView* aView = qobject_cast<QAbstractItemView*>( parent() );
  if ( !aView )
    return;

  if ( theIsEnabled )
  {
    connect( aView, SIGNAL( pressed(const QModelIndex&) ),
      this, SLOT( onItemPressed(const QModelIndex&) ) );
    connect( aView, SIGNAL(activated(const QModelIndex&) ),
      this, SLOT( onItemActivated(const QModelIndex&) ) );
  }
  else
  {
    disconnect( aView, SIGNAL( pressed(const QModelIndex&) ),
      this, SLOT( onItemPressed(const QModelIndex&) ) );
    disconnect( aView, SIGNAL( activated(const QModelIndex&) ),
      this, SLOT( onItemActivated(const QModelIndex&) ) );
  }
}

//! Invoke datum view's callback when system of units changes.
//! This slot is attached by widget factory to gui framework's
//! units system manager's signal.
void asiUI_DatumViewEventHandler::onUnitsChanged()
{
  asiUI_DatumViewBaseInterface* aViewBaseApi =
    dynamic_cast<asiUI_DatumViewBaseInterface*>( parent() );

  if ( !aViewBaseApi )
    return;

  aViewBaseApi->NotifyUnitsChanged();
}

//! Remember pressed item with left mouse button.
//! \param theIndex [in] index of the pressed item.
void asiUI_DatumViewEventHandler::onItemPressed(const QModelIndex& theIndex)
{
  // no modifiers allowed
  if ( QApplication::keyboardModifiers() != Qt::NoModifier 
    || QApplication::mouseButtons()      != Qt::LeftButton )
  {
    m_LastPressIdx = QModelIndex();
    return;
  }

  m_LastPressIdx = theIndex;
}

//! Trigger editing when item activated to support "edit on single click" feature.
//! Single click activation is provided by special style hint.
//! \sa asiUI_ViewStyleProxy.
//! \param theIndex [in] the clicked index.
void asiUI_DatumViewEventHandler::onItemActivated(const QModelIndex& theIndex)
{
  if ( m_LastPressIdx == theIndex )
  {
    QAbstractItemView* aView = qobject_cast<QAbstractItemView*>( parent() );
    if ( aView )
    {
      aView->setCurrentIndex(theIndex);
      aView->edit(theIndex);
    }
  }

  m_LastPressIdx = QModelIndex();
}

// ============================================================================
//                 Common template declaration
// ============================================================================

template class asiUI_DatumViewBase<QTableView>;
template class asiUI_DatumViewBase<QTreeView>;
