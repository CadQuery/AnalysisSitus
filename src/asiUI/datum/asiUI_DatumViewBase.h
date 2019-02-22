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

#ifndef asiUI_DatumViewBase_HeaderFile
#define asiUI_DatumViewBase_HeaderFile

// asiUI includes
#include <asiUI.h>
#include <asiUI_Datum.h>
#include <asiUI_DatumViewItem.h>
#include <asiUI_DatumViewModel.h>
#include <asiUI_DatumViewDelegate.h>
#include <asiUI_WidgetFactory.h>

// Qt includes
#pragma warning(push, 0)
#include <QPointer>
#include <QAbstractItemView>
#include <QProxyStyle>
#pragma warning(pop)

#pragma warning(disable: 4251) // "Class XXX needs to have dll-interface..."

// Forward declaration
class QKeyEvent;
class QStandardItem;
class QStandardItemModel;
class asiUI_DatumViewEventHandler;
class QAbstractProxyModel;

// ============================================================================
//                      Common internal interface to datum views
// ============================================================================

//! \ingroup GUI
//!
//! Public interface to implement basics access to datum views.
class asiUI_DatumViewBaseInterface
{
// interface for data model and delegate classes
public:

  //! Access value conversion datum instance associated with the specified model index.
  //! This methods is used by data model layer to access datums as value
  //! formatting and conversion tool.
  //! \param theIndex [in] the model index.
  virtual asiUI_Datum* AccessViewDatum(const QModelIndex& theIndex) = 0;

  //! Return datum dictionary identification string assigned
  //! for the specified item model index.
  //! \param theIndex [in] the item model index.
  //! \return data dictionary id, or empty string if no datum assigned.
  virtual QString DatumId(const QModelIndex& theIndex) const = 0;

  //! Return datum dictionary identification string assigned for the specified
  //! section header index.
  //! \param theHeader [in] the header type.
  //! \param theIndex [in] the section index.
  //! \return data dictionary id, or empty string if no datum assigned.
  virtual QString HeaderDatumId(const Qt::Orientation theHeader, const asiUI_HeaderIndex& theIndex) const = 0;

  //! Access widget factory to create datum instances.
  virtual Handle(asiUI_WidgetFactory) AccessFactory() const = 0;

  //! Notification method, tells that editor has been created and used by
  //! datum delegate assigned for the item view. Please, remind, that the editor
  //! is a wrapper owner the datum associated with cell item.
  virtual void NotifyDelegateEditorCreated(QWidget* theEditor, const QModelIndex& theIndex) = 0;

  //! Notification method, tells that custom selector has been triggered
  //! for the item with the passed index.
  virtual void NotifyCustomSelectorTriggered(const QModelIndex& theIndex ) = 0;

// common view operations
public:

  virtual void RefreshData() = 0;

// internal api for view's base event handler
public:
  
  //! Notification method, tells that local system of units has changed, so the
  //! contents of datum view should be refreshed.
  virtual void NotifyUnitsChanged() = 0;

  //! Return signal event handler for attaching unit change signal by widget factory.
  //! \return the associated event handler.
  virtual asiUI_DatumViewEventHandler* GetEventHandler() const = 0;
};

// ============================================================================
//         Generic implementation of common data view's functionality
// ============================================================================

//! \ingroup GUI
//!
//! This layer implements most of the functionality required by any view
//! that supports value display and editing with help of datums.
//! Subclassed instances are just to implement convenient layout.
//! Broadly, the datums can be thought of as value conversion tools instantiated
//! separately for data representation in delegates and value conversion in model.
//! The persistence of value conversion datums is done internally, via actualizeDatum method.
//! The method analyses and decides which datum is required for item, should it be created or
//! updated, should it be destroyed if outdated, thus taking all datum management functions on its own.
//! The subclasses are just to provide datum ids for the specified indexes for their generic layout.
template<typename ParentItemViewClass>
class asiUI_EXPORT asiUI_DatumViewBase : public ParentItemViewClass,
                                           public asiUI_DatumViewBaseInterface
{
public:

  asiUI_DatumViewBase(QWidget* theParent = 0);

// options flags
public:

  bool IsShowUnitsInLabel() const;

  void SetShowUnitsInLabel(const bool theIsShown);

  void SetEditByClick(const bool theIsEdit) const;

  void Reset();

  void RefreshData();

// data model & delegate interface implementations
public:

  virtual asiUI_Datum* AccessViewDatum(const QModelIndex& theIndex);

  virtual QString DatumId(const QModelIndex& theIndex) const;

  virtual QString HeaderDatumId(const Qt::Orientation theHeader, const asiUI_HeaderIndex& theIndex) const;

  virtual Handle(asiUI_WidgetFactory) AccessFactory() const;

  void NotifyDelegateEditorCreated(QWidget* theEditor,
                                   const QModelIndex& theIndex);

  void NotifyCustomSelectorTriggered(const QModelIndex& theIndex);

// event handler interface implementation
public:

  asiUI_DatumViewEventHandler* GetEventHandler() const;

  virtual void NotifyUnitsChanged();

// basic methods to implement in subclasses
protected:

  //! Implement this getter to provide datum identifier
  //! for item within the view specified by its index.
  //! \param theIndex [in] the index of the view item.
  virtual QString
    getItemDatum(const QModelIndex& theIndex) const = 0;

  //! Implement this getter to provide datum identifier
  //! for header item of the passed section.
  //! \param theHeader [in] the header type.
  //! \param theIndex [in] the index of the header section.
  virtual QString
    getHeaderDatum(const Qt::Orientation theHeader,
                   const asiUI_HeaderIndex& theIndex) const = 0;

  //! this method should be overridden to update
  //! units shown in labels describing the item
  //! elements (e.g. row, column, record...)
  //! it is invoked by internal logics.
  virtual void
    refreshUnitsInLabels() = 0;

// signal invocation callbacks, to implement in subclasses
protected:

  //! Emit signal when value of the specified item is changed.
  //! Redefine this method to map model index into user-friendly view's item layout location.
  //! \param theIndex [in] the model index of changed item.
  //! \param theValue [in] the new value.
  virtual void emitValueChanged(const QModelIndex& theIndex, const QVariant& theValue) = 0;

  //! Emit signal when value is started to be changed.
  //! Redefine this method to map model index into user-friendly view's item layout location.
  //! \param theIndex [in] the model index of changed item.
  //! \param theWidget [in] the editor widget.
  virtual void emitValueChanging(const QModelIndex& theIndex, QWidget* theWidget) = 0; 

  //! Emit signal when datum is closed and editing is finished.
  //! Redefine this method to map model index into user-friendly view's item layout location.
  //! \param theIndex [in] the model index of changed item.
  //! \param theWidget [in] the editor widget.
  virtual void emitDatumClosed(const QModelIndex& theIndex, QWidget* theWidget) = 0;

  //! Emit signal when selection changes.
  //! Redefine this method to provide signal invocation.
  //! \param theSelected [in] the new selection.
  //! \param theDeselected [in] the previous selection.
  virtual void emitSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected) = 0;

  //! Emit signal when custom selector is triggered for datum item.
  //! Redefine this method to map model index into user-friendly view's item layout location.
  //! \param theIndex [in] the model index of triggered item.
  virtual void emitCustomSelectorTriggered(const QModelIndex& theIndex) = 0;

public:

  bool IsProxyModel() const;

// basics data model access
protected:

  void setItemPrototype(const asiUI_DatumViewItem* thePrototype);

  asiUI_DatumViewItem* accessItem(const QModelIndex& theIndex) const;

  asiUI_DatumViewModel* itemModel() const;
  asiUI_DatumViewModel* itemModel(QAbstractItemModel* theModel) const;

  QModelIndex sourceIndex(const QModelIndex& theProxy) const;

protected:

  void init( const Handle(asiUI_WidgetFactory)& theCreator,
             asiUI_DatumViewModel* theItemModel = new asiUI_DatumViewModel(),
             asiUI_DatumViewItem* thePrototype = new asiUI_DatumViewItem(),
             asiUI_DatumViewDelegate* theDelegate = new asiUI_DatumViewDelegate() );

  virtual void setModel(QAbstractItemModel* theModel);

  QVariant getValue(const QModelIndex& theIndex) const;

  void setValue(const QModelIndex& theIndex, const QVariant& theValue);

  QString getString(const QModelIndex& theIndex) const;

  void setString(const QModelIndex& theIndex, const QString& theString);

// datum and data related functionality
protected:

  Handle(asiUI_WidgetFactory) widgetFactory() const;

  void updateItems(const bool theSkipEdited);

  void updateItems(const QModelIndexList& theIndexes,
                   const bool theSkipEdited);

  void updateHeaderItems();

  QString labelString(const QString& theDictId,
                      const QString& theTitle) const;

  void actualizeDatum(const QModelIndex& theIndex);

  virtual void dataChanged(const QModelIndex &theTopLeft,
                           const QModelIndex &theBottomRight,
                           const QVector<int> &roles = QVector<int>());

  virtual void closeEditor(QWidget* theEditor,
                           QAbstractItemDelegate::EndEditHint theEndHint);

  virtual void selectionChanged(const QItemSelection& theSelected,
                                const QItemSelection& theDeselected);

// view behavior functionality
protected:

  virtual void keyPressEvent(QKeyEvent *theEvent);

  virtual void showStatusTip(const QModelIndex& theIndex);

  virtual void hideStatusTip(const QModelIndex& theIndex);

// internals
private:

  void resetItemPrototype(QStandardItem* theRoot,
                          const asiUI_DatumViewItem* thePrototype) const;

  void collectIndexes(const QModelIndex& theParentIndex,
                      QModelIndexList& theIndexes,
                      const bool theIsRecursive = true) const;

private:

  bool m_bShowUnits;  //!< shown units in headers or not.

private:

  typedef QList<QPersistentModelIndex> IndexList;
  typedef QMap<QString, asiUI_Datum*> DictId2DatumMap;
  typedef QMap<QString, IndexList>     Datum2IndexList;
  typedef QMap<QPersistentModelIndex, QString> Index2DatumMap;
  typedef QMap<QPersistentModelIndex, QWidget*> Index2EditorMap;

  // item-to-datum, item-to-opened editor maps
  Index2EditorMap m_EditorMap;       //!< map to maintain opened editors
  Index2DatumMap  m_ActualDatumMap;  //!< map of actual datum tools for items
  Datum2IndexList m_ActualDatumRefs; //!< map to maintain count of datum references

  // datum duplication map
  DictId2DatumMap m_DatumEntityMap;    //!< map to access datum by their ids
  
  // helper entities
  Handle(asiUI_WidgetFactory) m_pWidgetFactory;      //!< instance of widget factory
  asiUI_DatumViewEventHandler* m_pEventHandler; //!< signal events handler
};

// ============================================================================
//         Base layer signals and events handler
// ============================================================================

//! \ingroup GUI
//!
//! This class provides qt's signal events handling and invokation
//! of appropriate methods for desired event processing.
class asiUI_DatumViewEventHandler : public QObject
{
  Q_OBJECT

public:

  asiUI_DatumViewEventHandler(QAbstractItemView* theHandled);

  void EnableEditByClickHandling(const bool theIsEnabled);

protected slots:

  void onUnitsChanged();

  void onItemPressed(const QModelIndex& theIndex);

  void onItemActivated(const QModelIndex& theIndex);

private:

  QModelIndex m_LastPressIdx; //!< last pressed item with left mouse button.
};

// =====================================================================================
//                           STYLE HINT TO ACTIVATE ON SINGLE CLICK
// =====================================================================================

//! \ingroup GUI
//!
//! Generic header drawing style, which
//! supports additional headers features, 
//! such as word wrapping.
class asiUI_ViewStyleProxy : public QProxyStyle
{
public:

  asiUI_ViewStyleProxy(QStyle* theStyle)
    : QProxyStyle(theStyle) {}

public:

  //! Override activate on 
  //! single click hint.
  //! \param theHint [in] the hint.
  //! \param theOption [in] the widget options.
  //! \param theWidget [in] the widget.
  //! \param theRetData [in] the returned data.
  virtual int 
    styleHint(StyleHint theHint,
              const QStyleOption* theOption,
              const QWidget* theWidget,
              QStyleHintReturn* theRetData) const
  {
    switch ( theHint )
    {
      case SH_ItemView_ActivateItemOnSingleClick:
        return true;

      default:
        break;
    }

    return QProxyStyle::styleHint(theHint, theOption, theWidget, theRetData);
  }
};

#pragma warning(default: 4251) // "Class XXX needs to have dll-interface..."

#endif
