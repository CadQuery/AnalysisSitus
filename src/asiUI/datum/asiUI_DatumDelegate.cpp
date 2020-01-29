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

// Own include
#include <asiUI_DatumDelegate.h>

// asiUI includes
#include <asiUI_DatumPathItem.h>
#include <asiUI_DatumItemEditor.h>
#include <asiUI_DatumCustomSelector.h>
#include <asiUI_DatumViewItem.h>
#include <asiUI_Common.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// SUIT includes
//#pragma warning(push, 0)
//#include <SUIT_Session.h>
//#include <SUIT_ResourceMgr.h>
//#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QToolButton>
#include <QPainter>
#include <QStylePainter>
#include <QAbstractItemView>
#include <QStyleOptionViewItemV4>
#include <QMargins>
#pragma warning(pop)

#define MIN_ITEM_HEIGHT 15

//! Constructor.
//! \param theParent [in] the parent object.
asiUI_DatumDelegate::asiUI_DatumDelegate(QWidget* theParent)
: QStyledItemDelegate(theParent),
  m_iRowMargin(10)
{
  /* ===================================
   *  Load pixmap for path browser item
   * =================================== */

  /*SUIT_Session* aSession = SUIT_Session::session();
  SUIT_ResourceMgr* aResMgr = NULL;
  if ( aSession )
    aResMgr = aSession->resourceMgr();*/

  m_Browse = /*aResMgr ? 
    aResMgr->loadPixmap( "asiUI", tr("ICON_BROWSE") ) :*/ QPixmap(asiUI_BrowsePixmap);
}

//! Get row margin for the height.
//! \return the margin.
int asiUI_DatumDelegate::GetRowMargin() const
{
  return m_iRowMargin;
}

//! Set margin for the row height.
//! \param theMargin [in] margin size.
void asiUI_DatumDelegate::SetRowMargin(const int theMargin)
{
  m_iRowMargin = theMargin;
}

//! Create custom editor for item.
//! \sa createDatum(...) method is called to provide datum editor for the model index.
//! \param theParent [in] - parent widget for editor.
//! \param theOption [in] - style options for widget.
//! \param theIndex [in] - the model index for the item's cell.
QWidget* asiUI_DatumDelegate::createEditor(QWidget* theParent, 
                                            const QStyleOptionViewItem& theOption,
                                            const QModelIndex &theIndex) const
{
  if ( isCustomSelector(theIndex) || isBoolItem(theIndex) )
  {
    return nullptr;
  }

  asiUI_DatumItemEditor* anEditor = createDatum(theParent, theIndex);

  if ( !anEditor )
  {
    QWidget* anEditorWidget = QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
    emit EditorCreated(anEditorWidget, theIndex);
    return anEditorWidget;
  }

  connect(anEditor, SIGNAL(CommitValue()), this, SLOT(onCommitValue()));
  connect(anEditor, SIGNAL(CloseEditor()), this, SLOT(onCloseEditor()));

  emit EditorCreated(anEditor, theIndex);

  return anEditor;
}

//! This slot is called when changes made to editor
//! item's value and can be committed to update the model.
void asiUI_DatumDelegate::onCommitValue()
{
  QWidget* anEditor = qobject_cast<QWidget*>(sender());
  if ( anEditor )
    emit commitData(anEditor);
}

//! This slot is called when editor is to be committed.
void asiUI_DatumDelegate::onCloseEditor()
{
  QWidget* anEditor = qobject_cast<QWidget*>(sender());
  if ( anEditor )
    emit closeEditor(anEditor);
}

//! Compute rectangular area for drawing check-box within item.
//! \param theOption [in] the view item options.
//! \param theBounding [in] the bounding rectangle.
QRect asiUI_DatumDelegate::checkBoxRect(const QStyleOptionViewItem& theOption,
                                         const QRect& theBounding) const
{
  QStyleOptionViewItem aOpt4 = theOption;
  QStyleOptionButton anOpt;
  anOpt.QStyleOption::operator=(theOption);
  anOpt.rect = theBounding;
  QWidget* aWid = qobject_cast<QWidget*>( parent() );
  QStyle* aStyle = aWid ? aWid->style() : QApplication::style();
  QRect aRect = aStyle->subElementRect(QStyle::SE_ViewItemCheckIndicator, &anOpt, aWid);

  aRect = QStyle::alignedRect( aOpt4.direction,
                               this->checkBoxAlignment(),
                               aRect.size(),
                               aOpt4.rect);

  // ...
  // Add margin to layout
  // ...

  int aBorder = aRect.height() < theOption.rect.height() ? ( theOption.rect.height() - aRect.height() ) / 2 : 0;

  if ( theOption.rect.width() - aBorder * 2 > aRect.width() )
  {
    if ( (checkBoxAlignment() & Qt::AlignLeft) != 0 )
    {
      aRect.adjust(aBorder, 0, aBorder, 0);
    }
    else if ( (checkBoxAlignment() & Qt::AlignRight) != 0 )
    {
      aRect.adjust(-aBorder, 0, -aBorder, 0);
    }
  }

  return aRect;
}

//! Paint check box control.
//! \param thePainter [in] the painter.
//! \param theStyle [in] the style to paint the item.
//! \param theOption [in] the view options.
//! \param theRect [in] the view item rect.
//! \param theState [in] the check box state.
void asiUI_DatumDelegate::drawCheck(QPainter* thePainter,
                                     const QStyle* theStyle,
                                     const QStyleOptionViewItem& theOption,
                                     const QRect& theRect,
                                     Qt::CheckState theState) const
{
  if ( !theRect.isValid() )
    return;

  QStyleOptionViewItem aPaintOpt(theOption);
  aPaintOpt.rect = theRect;
  aPaintOpt.state = aPaintOpt.state & ~QStyle::State_HasFocus;

  switch (theState) 
  {
    case Qt::Unchecked:
      aPaintOpt.state |= QStyle::State_Off;
      break;
    case Qt::PartiallyChecked:
      aPaintOpt.state |= QStyle::State_NoChange;
      break;
    case Qt::Checked:
      aPaintOpt.state |= QStyle::State_On;
      break;
  }

  theStyle->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &aPaintOpt, thePainter);
}

//! Draw background of the item according to its style and props.
//! \param thePainter [in] the painter to fill bg.
//! \param theOption [in] the style options.
//! \param theIndex [in] the model index.
void asiUI_DatumDelegate::drawBackground(QPainter* thePainter,
                                          const QStyleOptionViewItem& theOption,
                                          const QModelIndex& theIndex) const
{
  if ( theOption.showDecorationSelected && (theOption.state & QStyle::State_Selected) )
  {
    QPalette::ColorGroup aGround =
      theOption.state & QStyle::State_Enabled 
        ? QPalette::Normal
        : QPalette::Disabled;

    if ( aGround == QPalette::Normal && !(theOption.state & QStyle::State_Active) )
    {
      aGround = QPalette::Inactive;
    }

    const QBrush& aBrush = theOption.palette.brush(aGround, QPalette::Highlight);

    thePainter->fillRect(theOption.rect, aBrush);
  } 
  else 
  {
    QVariant aValue = theIndex.data(Qt::BackgroundRole);
    if ( aValue.canConvert(QMetaType::QBrush) )
    {
      QPointF anOldBO = thePainter->brushOrigin();
      thePainter->setBrushOrigin(theOption.rect.topLeft());
      thePainter->fillRect(theOption.rect, qvariant_cast<QBrush>(aValue));
      thePainter->setBrushOrigin(anOldBO);
    }
  }
}

//! Paint display presentation for the cell.
//! \param thePainter [in] - the qt painter.
//! \param theOption [in] - the cell painting options.
//! \param theIndex [in] - the cell's model index.
void asiUI_DatumDelegate::paint(QPainter* thePainter,
                                 const QStyleOptionViewItem& theOption,
                                 const QModelIndex& theIndex) const
{
  // prepare style for custom painting
  QStyle* aStyle = QApplication::style();
  QStyleOptionViewItem aOpt4 = theOption;
  initStyleOption(&aOpt4, theIndex);

  /* ============================
   *  Paint color selector item
   * ============================ */

  int aValueRole   = valueRole(theIndex);
  int aDisplayRole = displayRole(theIndex);

  if ( isColorItem(theIndex) )
  {
    QRect aSmallRect = theOption.rect;
    aSmallRect.adjust(1, 1, -1, -1);

    int aColorCode = theIndex.data(aValueRole).toInt();

    QColor aBgColor = asiVisu_Utils::IntToColor(aColorCode);

    thePainter->fillRect(aSmallRect, aBgColor);
    thePainter->setPen(Qt::black);
    thePainter->drawRect(aSmallRect);

    thePainter->setPen(Qt::gray);
    thePainter->drawRect(theOption.rect);
    return;
  }

  /* =========================
   *  Paint check box item
   * ========================= */

  if ( isBoolItem(theIndex) )
  {
    bool isValid = false;
    int aIntValue = theIndex.data(Qt::EditRole).toInt(&isValid);
    Qt::CheckState aCheckState = Qt::PartiallyChecked;
    if (isValid)
    {
      switch (aIntValue)
      {
        case 0:  aCheckState = Qt::Unchecked; break;
        case 1:  aCheckState = Qt::Checked;   break;
      }
    }

    aOpt4.features &= ~QStyleOptionViewItemV2::HasDecoration;
    aOpt4.features &= ~QStyleOptionViewItemV2::HasCheckIndicator;
    aOpt4.features &= ~QStyleOptionViewItemV2::HasDisplay;
    aOpt4.text = QString();

    QRect aCheckBoxRect = checkBoxRect(theOption, theOption.rect);

    // draw frame & background
    aStyle->drawControl(QStyle::CE_ItemViewItem, &aOpt4, thePainter);

    // draw check box
    drawCheck(thePainter, aStyle, theOption, aCheckBoxRect, aCheckState);

    return;
  }

  /* =========================
   *  Paint path browser item
   * ========================= */

  if ( asiUI_DatumPathItem* aPathItem = isPathItem(theIndex) )
  {
    // draw short path name...
    QString aString = theIndex.data(Qt::DisplayRole).toString();
    QStringList aSeparatedList =
      aString.split( QRegExp("(\\\\|/)") );
    QString aShortPath = aSeparatedList.last();
    aOpt4.text = aShortPath;

    // draw text and browse icon
    aStyle->drawControl(QStyle::CE_ItemViewItem, &aOpt4, thePainter);
    thePainter->drawPixmap( getBrowseRect(theOption), m_Browse );
    return;
  }

  /* ============================
   *  Paint custom selector item 
   * ============================ */

  if ( asiUI_DatumCustomSelector* aSelectorItem = isCustomSelector(theIndex) )
  {
    aStyle->drawControl(QStyle::CE_ItemViewItem, &aOpt4, thePainter);
    QToolButton* aButton = aSelectorItem->AccessButton();

    QRect aBorderRect;
    QRect aPixRect = this->getIconRect(theOption, aButton->iconSize(), aBorderRect);

    // draw pixmap
    QImage aIcon = aButton->icon().pixmap( aPixRect.size() ).toImage();

    if ( m_TriggeredMap.contains(theIndex) )
    {
      aIcon.invertPixels();
      thePainter->setPen(Qt::black);
    }
    else
    {
      thePainter->setPen(Qt::gray);
    }

    thePainter->drawImage(aPixRect, aIcon);
    thePainter->drawRoundedRect(aBorderRect, 2, 2, Qt::AbsoluteSize);
    return;
  }

  // change displayed value
  QVariant aDisplayValue = theIndex.data(aDisplayRole);
  QString aDisplayText = displayText(aDisplayValue, aOpt4.locale);
  aOpt4.text = aDisplayText;

  aStyle->drawControl(QStyle::CE_ItemViewItem, &aOpt4, thePainter);
}

//! Set data for the editor widget.
//! \param theEditor [in] the editor widget.
//! \param theIndex [in] the index of edited data model item.
void asiUI_DatumDelegate::setEditorData(QWidget* theEditor,
                                         const QModelIndex& theIndex) const
{
  // disable signal duplication for datum editors
  bool isBlocked = false;
  asiUI_DatumItemEditor* aDatum = castDatumEditor(theEditor);
  if ( aDatum )
  {
    isBlocked = aDatum->blockSignals(true);
    int aValueRole = valueRole(theIndex);
    aDatum->SetValue( theIndex.data(aValueRole) );
    aDatum->blockSignals(isBlocked);
  }
  else
  {
    QStyledItemDelegate::setEditorData(theEditor, theIndex);
  }
}

//! Set model data from datum editor.
//! \param theEditor [in] the editor.
//! \param theModel [in] the data model.
//! \param theIndex [in] the index of the cell being edited.
void asiUI_DatumDelegate::setModelData(QWidget* theEditor,
                                        QAbstractItemModel* theModel,
                                        const QModelIndex& theIndex) const
{
  asiUI_DatumItemEditor* aDatum = castDatumEditor(theEditor);
  if ( aDatum )
  {
    if ( aDatum->GetDatum()->IsValid(false) )
    {
      theModel->setData( theIndex, aDatum->GetValue(), valueRole(theIndex) );
    }

    return;
  }

  QStyledItemDelegate::setModelData(theEditor, theModel, theIndex);
}

//! Returns row height with custom margin added to it.
//! \param theOption [in] cell options.
//! \param theIndex [in] index in view's model.
QSize asiUI_DatumDelegate::sizeHint(const QStyleOptionViewItem& theOption,
                                     const QModelIndex& theIndex) const
{
  QSize aSize = QStyledItemDelegate::sizeHint(theOption, theIndex);
  aSize.setHeight( qMax(aSize.height(), MIN_ITEM_HEIGHT) + m_iRowMargin );
  return aSize;
}

//! Extend event processing with advanced
//! editor scenarios.
//! \param theEvent [in] the edit event.
//! \param theModel [in] the item model.
//! \param theOption [in] the option.
//! \param theIndex [in] the item model index.
//! \return true if event processed.
bool asiUI_DatumDelegate::editorEvent(QEvent* theEvent,
                                       QAbstractItemModel* theModel,
                                       const QStyleOptionViewItem& theOption,
                                       const QModelIndex& theIndex)
{
  /* =================================
   *  Handle events for generic items 
   * ================================= */

  if ( asiUI_DatumPathItem* aItem = isPathItem(theIndex) )
  {
    return pathDatumEvent(theEvent, theModel, theOption, aItem, theIndex);
  }

  if ( asiUI_DatumCustomSelector* aItem = isCustomSelector(theIndex) )
  {
    return selectorDatumEvent(theEvent, theModel, theOption, aItem, theIndex);
  }

  if ( isBoolItem(theIndex) )
  {
    return checkBoxDatumEvent(theEvent, theModel, theOption, theIndex);
  }

  return QStyledItemDelegate::editorEvent(theEvent, theModel, theOption, theIndex);
}

//! Handle key events of editor and do not commit cache if the input datum
//! value is not valid.
//! \param theObject [in] the hooked object.
//! \param theEvent [in] the hooked event.
bool asiUI_DatumDelegate::eventFilter(QObject* theObject, QEvent* theEvent)
{
  asiUI_DatumItemEditor* aValidationEditor = qobject_cast<asiUI_DatumItemEditor*>(theObject);
  if (!aValidationEditor)
  {
    return QStyledItemDelegate::eventFilter(theObject, theEvent);
  }
  
  if (theEvent->type() == QEvent::KeyPress)
  {
    QKeyEvent* aKeyEvent = static_cast<QKeyEvent*>(theEvent);
    switch ( aKeyEvent->key() )
    {
      case Qt::Key_Enter  :
      case Qt::Key_Return :
      {
        if ( !aValidationEditor->GetDatum()->IsValid(false) )
        {
          emit closeEditor(aValidationEditor, QAbstractItemDelegate::RevertModelCache);
          return false;
        }

        return QStyledItemDelegate::eventFilter(theObject, theEvent);
      }

      default:
        break;
    }
  }
  
  return QStyledItemDelegate::eventFilter(theObject, theEvent);
}

//! Initialize option with the values using the index.
//! \param theOption [in] the option.
//! \param theIndex [in] the index.
void asiUI_DatumDelegate::initStyleOption(QStyleOptionViewItem *theOption,
                                           const QModelIndex &theIndex) const
{
  QStyledItemDelegate::initStyleOption(theOption, theIndex);

  if (QStyleOptionViewItem *aV4Opt = qstyleoption_cast<QStyleOptionViewItem*>(theOption))
  {
    QVariant aValue = theIndex.data( displayRole(theIndex) );
    if (aValue.isValid() && !aValue.isNull())
    {
      aV4Opt->text = displayText(aValue, aV4Opt->locale);
    }
  }
}

//! Cast widget to a datum item editor.
//! \return casted editor or a null pointer if the widget is 
//!         of another type.
asiUI_DatumItemEditor* 
  asiUI_DatumDelegate::castDatumEditor(QWidget* theWidget) const
{
  return qobject_cast<asiUI_DatumItemEditor*>(theWidget);
}

//! Process editor event on path browse item.
//! \param theEvent [in] the event to process.
//! \param theModel [in] the item model.
//! \param theOption [in] the view options.
//! \param theIndex [in] the item index.
bool asiUI_DatumDelegate::pathDatumEvent(QEvent* theEvent,
                                          QAbstractItemModel* theModel,
                                          const QStyleOptionViewItem& theOption,
                                          asiUI_DatumPathItem* theItem,
                                          const QModelIndex& theIndex)
{
  if ( theEvent->type() == QEvent::MouseButtonPress ||
       theEvent->type() == QEvent::MouseButtonRelease )
  {
    QMouseEvent* aMouseEv = dynamic_cast<QMouseEvent*>( theEvent );
    if ( aMouseEv->button() != Qt::LeftButton )
    {
      m_TriggeredMap.remove( theIndex );
      return false;
    }

    // fall back if no click on button
    if ( !getBrowseRect(theOption).contains( aMouseEv->pos() ) )
    {
      m_TriggeredMap.remove( theIndex );
      return QStyledItemDelegate::editorEvent(theEvent, theModel, theOption, theIndex);
    }

    // add to internal trigger map
    if ( theEvent->type() == QEvent::MouseButtonPress )
      m_TriggeredMap.insert( theIndex );
    else
      m_TriggeredMap.remove( theIndex );

    // open dialog
    QString aBrowsePath = theItem->Browse();
    if ( !aBrowsePath.isNull() )
    {
      theModel->setData( theIndex, aBrowsePath, valueRole(theIndex) );
    }

    return true; // event accepted
  }

  m_TriggeredMap.remove( theIndex );

  return QStyledItemDelegate::editorEvent(theEvent, theModel, theOption, theIndex);
}

//! Get rect for drawing browse pixmap for path item editor.
//! \param theOption [in] the item drawing option.
//! \return rectangle for pixmap.
QRect asiUI_DatumDelegate::getBrowseRect(const QStyleOptionViewItem& theOption) const
{
  QRect aBrowseRect = m_Browse.rect();
  QRect anItemRect  = theOption.rect;

  int aWOffset = ( anItemRect.width() - aBrowseRect.width() );
  int aHOffset = ( anItemRect.height() - aBrowseRect.height() ) * 0.5;

  anItemRect.setSize( aBrowseRect.size() );
  anItemRect.translate( aWOffset - aHOffset, aHOffset );

  return anItemRect;
}

//! Process editor event on custom selector item.
//! \param theEvent [in] the event to process.
//! \param theModel [in] the item model.
//! \param theOption [in] the view options.
//! \param theIndex [in] the item index.
bool asiUI_DatumDelegate::selectorDatumEvent(
                                          QEvent* theEvent,
                                          QAbstractItemModel* theModel,
                                          const QStyleOptionViewItem& theOption,
                                          asiUI_DatumCustomSelector* theItem,
                                          const QModelIndex& theIndex)
{
  if ( theEvent->type() == QEvent::MouseButtonPress ||
       theEvent->type() == QEvent::MouseButtonRelease )
  {
    QMouseEvent* aMouseEv = dynamic_cast<QMouseEvent*>( theEvent );
    if ( aMouseEv->button() != Qt::LeftButton )
    {
      m_TriggeredMap.remove( theIndex );
      return false;
    }

    QToolButton* aButton = theItem->AccessButton();

    QRect aBorderRect;
    QRect aPixRect = this->getIconRect(
      theOption, aButton->iconSize(), aBorderRect);

    // fall back if no click on button
    if ( !aBorderRect.contains( aMouseEv->pos() ) )
    {
      return QStyledItemDelegate::editorEvent(theEvent, theModel, theOption, theIndex);
    }

    if ( theEvent->type() == QEvent::MouseButtonRelease )
    {
      // emit custom selector event
      emit CustomSelectorTriggered(theIndex);
    }

    // add to internal trigger map
    if ( theEvent->type() == QEvent::MouseButtonPress )
      m_TriggeredMap.insert( theIndex );
    else
      m_TriggeredMap.remove( theIndex );

    return true; // event accepted
  }

  m_TriggeredMap.remove( theIndex );

  return QStyledItemDelegate::editorEvent(theEvent, theModel, theOption, theIndex);
}

//! Compute pixmap rectangle for custom selector icon.
//! The method also provides rect for drawing border.
//! \param theOption [in] the item drawing option.
//! \param theIconSize [in] the size of the icon.
//! \param theBorderRect [out] the border rectangle.
//! \return rectangle for pixmap.
QRect asiUI_DatumDelegate::getIconRect(const QStyleOptionViewItem& theOption,
                                        const QSize& theIconSize,
                                        QRect& theBorderRect) const
{
  QSize aIconSize = theIconSize;
  QSize aBorderSize = theIconSize;
  QSize aDrawRect = theOption.rect.size();

  const int aBorderMargin = 2;
  const int aPixmapMargin = 2;

  const int aMaxBrdWidth  = aDrawRect.width() - aBorderMargin * 2;
  const int aMaxBrdHeight = aDrawRect.height() - aBorderMargin * 2;
  const int aMaxPixWidth  = aDrawRect.width() - aPixmapMargin * 2;
  const int aMaxPixHeight = aDrawRect.height() - aPixmapMargin * 2;

  aIconSize.scale(aMaxPixWidth, aMaxPixHeight, Qt::KeepAspectRatio);
  aBorderSize.scale(aMaxBrdWidth, aMaxBrdHeight, Qt::KeepAspectRatio);

  int aRight = theOption.rect.right();
  int aBottom = theOption.rect.bottom();

  // align border rect to bottom right
  theBorderRect = QRect( QPoint(0, 0), aBorderSize );
  theBorderRect.moveBottomRight(
    QPoint( aRight - aBorderMargin, aBottom - aBorderMargin ) );

  // align result to bottom right 
  QRect aResultRect( QPoint(0, 0), aIconSize );
  aResultRect.moveBottomRight( 
    QPoint( aRight - aPixmapMargin, aBottom - aPixmapMargin) );

  return aResultRect;
}

//! Trigger editing of check box without invoking
//! external widget editor.
//! \param theEvent [in] the editor event.
//! \param theModel [in] the item model.
//! \param theOption [in] the item painting options.
//! \param theIndex [in] the item model index.
bool asiUI_DatumDelegate::checkBoxDatumEvent(QEvent* theEvent,
                                              QAbstractItemModel* theModel,
                                              const QStyleOptionViewItem& theOption,
                                              const QModelIndex& theIndex)
{
  // ...
  // Editable item is expected
  // ...

  Qt::ItemFlags aFlags = theModel->flags(theIndex);

  if ( !(theOption.state & QStyle::State_Enabled) || !(aFlags & Qt::ItemIsEnabled) )
  {
      return false;
  }

  // ...
  // Proper editing event is expected
  // ...

  if ( theEvent->type() == QEvent::MouseButtonRelease
    || theEvent->type() == QEvent::MouseButtonDblClick)
  {
    QRect aCheckRect = checkBoxRect(theOption, theOption.rect);

    QMouseEvent* aMouseEv = static_cast<QMouseEvent*>(theEvent);

    if ( aMouseEv->button() != Qt::LeftButton )
    {
      return false;
    }

    // Skip double clicking events
    if ( theEvent->type() == QEvent::MouseButtonDblClick )
    {
      return true;
    }

    if ( !aCheckRect.contains( aMouseEv->pos() ) )
    {
      return false;
    }
  }
  else if ( theEvent->type() == QEvent::KeyPress )
  {
    QKeyEvent* aKeyEvent = static_cast<QKeyEvent*>(theEvent);

    if ( aKeyEvent->key() != Qt::Key_Space && aKeyEvent->key() != Qt::Key_Select )
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  // ...
  // Commit changes
  // ...

  bool isChecked = theModel->data( theIndex, valueRole(theIndex) ).toBool();

  return theModel->setData( theIndex, !isChecked, valueRole(theIndex) );
}
