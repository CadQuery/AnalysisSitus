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

#ifndef asiUI_DatumDelegate_HeaderFile
#define asiUI_DatumDelegate_HeaderFile

#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QSet>
#include <QStyledItemDelegate>
#include <QPersistentModelIndex>
#pragma warning(pop)

#pragma warning(disable: 4251) // "Class XXX needs to have dll-interface..." by member collections

class asiUI_DatumCustomSelector;
class asiUI_DatumItemEditor;
class asiUI_DatumPathItem;

//! \ingroup GUI
//!
//! Base delegate for populating item views with datum editor controls.
//! A set of editor factory methods should be redefined to implement
//! desired delegate's behavior.
class asiUI_EXPORT asiUI_DatumDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:

  asiUI_DatumDelegate(QWidget* theParent);

  //! Get row margin for the height.
  //! \return the margin.
  int GetRowMargin() const;

  //! Set margin for the row height.
  //! \param theMargin [in] margin size.
  void SetRowMargin(const int theMargin);

signals:

  //! The signal is emitted when editor widget created
  //! for the item.
  //! \param theEditor [in] the editor widget.
  //! \param theIndex [in] the item's model index.
  void
    EditorCreated(QWidget* theEditor,
                  const QModelIndex& theIndex) const;

  //! The signal is emitted when custom selector
  //! button is triggered.
  //! \param theIndex [in] the item index.
  void
    CustomSelectorTriggered(const QModelIndex& theIndex);

// editor factory methods
protected:

  //! Create datum editor for the given model index.
  //! This method should be redefined to provide a
  //! datum editor for the edited model index.
  //! \param theParent [in] parent widget.
  //! \param theIndex [in] model index.
  virtual asiUI_DatumItemEditor* 
    createDatum(QWidget* theParent,
                const QModelIndex& theIndex) const = 0;

  //! Check if the value for the specified index is color.
  //! For the items that gets this flag defined as true,
  //! corresponding presentation is provided by filling
  //! the item's rect with color.
  //! \param theIndex [in] model index.
  //! \return boolean flag indicating whether the items value
  //!         is color.
  virtual bool 
    isColorItem(const QModelIndex& theIndex) const = 0;

  //! Check if the value for the specified index is boolean.
  //! For the items that gets this flag defined as true,
  //! corresponding presentation is provided by drawing
  //! check-box in the item's rect.
  //! \param theIndex [in] model index.
  //! \return boolean flag indicating whether the items value
  //!         is boolean.
  virtual bool 
    isBoolItem(const QModelIndex& theIndex) const = 0;

  //! Check if the item represents path field.
  //! If yes, the method should retrurn corresponding
  //! datum.
  //! \param theIndex [in] model index.
  //! \return path browser datum.
  virtual asiUI_DatumPathItem*
    isPathItem(const QModelIndex& theIndex) const = 0;

  //! Check if the item represents custom selector field.
  //! If yes, the method should return corresponding datum.
  //! \param theIndex [in] model index.
  //! \return custom selector datum.
  virtual asiUI_DatumCustomSelector*
    isCustomSelector(const QModelIndex& theIndex) const = 0;

  //! Specifies default alignment for check-box controls.
  virtual Qt::Alignment
    checkBoxAlignment() const = 0;

protected:

  //! Redefine method to return model role for storing and
  //! getting value for putting in editor.
  //! \param theIndex [in] the model index.
  //! \return integer id corresponding to the role.
  virtual int valueRole(const QModelIndex& theIndex) const = 0;

  //! Redefine method to return model role used for
  //! storing display value of datum.
  //! \param theIndex [in] the model index.
  //! \return integer id corresponding to the role.
  virtual int displayRole(const QModelIndex& theIndex) const = 0;

protected: //! \name implementation for path editor

  virtual bool
    pathDatumEvent(QEvent* theEvent,
                   QAbstractItemModel* theModel,
                   const QStyleOptionViewItem& theOption,
                   asiUI_DatumPathItem* theItem,
                   const QModelIndex& theIndex);

  virtual QRect
    getBrowseRect(const QStyleOptionViewItem& theOption) const;

protected: //! \name implementation for selector editor

  virtual bool
    selectorDatumEvent(QEvent* theEvent,
                       QAbstractItemModel* theModel,
                       const QStyleOptionViewItem& theOption,
                       asiUI_DatumCustomSelector* theItem,
                       const QModelIndex& theIndex);

  virtual QRect
    getIconRect(const QStyleOptionViewItem& theOption,
                const QSize& theIconSize,
                QRect& theBorderRect) const;

protected: //! \name methods for built-in editors

  virtual bool
    checkBoxDatumEvent(QEvent* theEvent,
                       QAbstractItemModel* theModel,
                       const QStyleOptionViewItem& theOption,
                       const QModelIndex& theIndex);

protected: //! \name delegate services

  virtual void
    paint(QPainter* thePainter,
          const QStyleOptionViewItem& theOption,
          const QModelIndex& theIndex) const;

  virtual void
    setModelData(QWidget* theEditor,
                 QAbstractItemModel* theModel,
                  const QModelIndex& theIndex) const;

  virtual void
    setEditorData(QWidget* theEditor,
                  const QModelIndex& theIndex) const;

  virtual QWidget*
    createEditor(QWidget* theParent,
                 const QStyleOptionViewItem& theOption,
                 const QModelIndex& theIndex) const;

  virtual QSize
    sizeHint(const QStyleOptionViewItem& theOption,
             const QModelIndex& theIndex) const;

  virtual bool
    editorEvent(QEvent* theEvent,
                QAbstractItemModel* theModel,
                const QStyleOptionViewItem& theOption,
                const QModelIndex& theIndex);

  virtual bool
    eventFilter(QObject* theObject, QEvent* theEvent);

  virtual void initStyleOption(QStyleOptionViewItem *theOption,
                               const QModelIndex &theIndex) const;

protected slots:

  void
    onCommitValue();

  void
    onCloseEditor();

protected:

  QRect checkBoxRect(const QStyleOptionViewItem& theOption,
                     const QRect& theBounding) const;

  void drawCheck(QPainter* thePainter,
                 const QStyle* theStyle,
                 const QStyleOptionViewItem& theOption,
                 const QRect& theRect,
                 Qt::CheckState theState) const;

  void drawBackground(QPainter* thePainter,
                      const QStyleOptionViewItem& theOption,
                      const QModelIndex& theIndex) const;

private: //! \name internals

  asiUI_DatumItemEditor* castDatumEditor(QWidget* theWidget) const;

private: //! \name internal fields

  int m_iRowMargin;
  QPixmap m_Browse;
  QSet<QPersistentModelIndex> m_TriggeredMap;
};

#pragma warning(default: 4251) // "Class XXX needs to have dll-interface..." by member collections

#endif
