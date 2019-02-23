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
#include <asiUI_ParameterEditorView.h>

// asiUI includes
#include <asiUI_WidgetFactory.h>
#include <asiUI_DatumItemEditor.h>
#include <asiUI_ParameterEditor.h>
#include <asiUI_ParameterEditorItem.h>
#include <asiUI_ParameterEditorDelegate.h>
#include <asiUI_Common.h>
#include <asiUI_DatumLineEdit.h>
#include <asiUI_DatumCheckBox.h>
#include <asiUI_DatumComboBox.h>
#include <asiUI_DatumViewBase.h>
#include <asiUI_EvalButton.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_CheckBox.h>
#include <QDS_ComboBox.h>
#include <QDS_Datum.h>
#include <QDS_LineEdit.h>
#include <QDS_SpinBox.h>
#pragma warning(pop)

// SUIT includes
#include <DDS_Dictionary.h>
//#include <SUIT_Session.h>
#include <SUIT_TreeSync.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QHeaderView>
#include <QPainter>
#pragma warning(pop)

// OCC includes
#include <Standard_ProgramError.hxx>

// =====================================================================================
//                           PARAMETER EDITOR VIEW
// =====================================================================================

//! Constructor for parameter view widget
//! \param[in] factory widget factory.
//! \param[in] parent  parent widget.
asiUI_ParameterEditorView::asiUI_ParameterEditorView(const Handle(asiUI_WidgetFactory)& factory,
                                                     QWidget*                           parent)
: QTreeWidget       (parent),
  m_bSignalsEnabled (true),
  m_factory         (factory)
{
  // customize parameter editor
  setHeaderLabels(QStringList() << tr( "PARAMETER" ) << tr( "" ) << tr( "VALUE" ));
  setAlternatingRowColors(true);

  SetReadOnly(true);

  setStyle( new asiUI_ViewStyleProxy(NULL) );
  setIndentation(12);
  setColumnCount(3);
  header()->setSectionResizeMode(Column_Computed, QHeaderView::Fixed);
  header()->resizeSection(Column_Computed, 0);

  // set item delegate for value column
  asiUI_ParameterEditorDelegate* aDelegate = 
    new asiUI_ParameterEditorDelegate(this);

  setItemDelegate(aDelegate);
  
  setUniformRowHeights(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setAllColumnsShowFocus(true);
  setTabKeyNavigation(true);

  connect( this, SIGNAL( itemChanged(QTreeWidgetItem*, int) ), SLOT( onItemChanged(QTreeWidgetItem*, int) ) );
  connect( this, SIGNAL( itemPressed(QTreeWidgetItem*, int) ), SLOT( onItemPressed(QTreeWidgetItem*, int) ) );

  m_BorderColor = QColor::fromRgb(195, 195, 195);
}

asiUI_ParameterEditorView::~asiUI_ParameterEditorView()
{
}

//! Set parameter list in view.
//! \param theParameters [in] - list of parameters data.
void asiUI_ParameterEditorView::SetParameters(const asiUI_ParamDataList& theParameters)
{
  // don't emit any signals
  bool isSignals = enableSignalProcessing(false);

  ClearParameters();

  QTreeWidgetItem* aRootItem  = invisibleRootItem();

  QList<asiUI_ParameterEditorData>::ConstIterator aParamIt =
    theParameters.constBegin();

  QTreeWidgetItem* aGroupParent = aRootItem;

  // create tree
  bool aHasGroups = false;
  bool isReadOnly = IsReadOnly();

  asiUI_ParameterEditorItem* anItemParam = 0;
  for ( ; aParamIt != theParameters.constEnd(); aParamIt++ )
  {
    const asiUI_ParameterEditorData& aParam = *aParamIt;

    if ( aParam.IsGroup() )
    {
      // create new group item
      aGroupParent = new asiUI_ParameterEditorItem(
        aRootItem, aParam.GetName(), aParam.GetNodeID(), 0, 0);

      // disable editing
      aGroupParent->setFlags(aGroupParent->flags() & ~Qt::ItemIsEditable);
      aGroupParent->setFirstColumnSpanned(true);

      aHasGroups = true;
    }
    else
    {
      Handle(DDS_DicItem) aDicItem = DDS_Dictionary::Get()
        ->GetDicItem(asiUI_Common::ToAsciiString(aParam.GetDictID()));

      // create item for parameters
      anItemParam = new asiUI_ParameterEditorItem(
        aGroupParent, aParam.GetName(), aParam.GetNodeID(),
        aDicItem, createDatum(aParam));

      anItemParam->SetValue(aParam.GetValue());
      anItemParam->SetComputed(aParam.GetComputed());
      if ( aParam.GetComputed() == true )
        anItemParam->setFlags(anItemParam->flags() & ~Qt::ItemIsEditable);
      else
        anItemParam->setFlags(anItemParam->flags() | Qt::ItemIsEditable);

      if ( aParam.IsComputable() )
      {
        QSize aHint = sizeHintForIndex(indexFromItem(anItemParam));
        int aSize = aHint.height();

        // create "computed" button
        asiUI_EvalButton* anEvaluatedBtn = new asiUI_EvalButton( this );
        anEvaluatedBtn->setEnabled( !isReadOnly && aParam.IsEnabled() );
        anEvaluatedBtn->setChecked( aParam.GetComputed() );
        anEvaluatedBtn->setMinimumHeight( aSize );
        anEvaluatedBtn->setMinimumWidth( aSize );
        anEvaluatedBtn->setFocusPolicy( Qt::NoFocus );

        m_ItemButton.insert( anEvaluatedBtn, anItemParam );
        connect( anEvaluatedBtn, SIGNAL( toggled(bool) ), SLOT( onComputedToggled(bool) ) );

        // add button
        setItemWidget( anItemParam, Column_Computed, anEvaluatedBtn );
      }

      anItemParam->SetEnabled( aParam.IsEnabled() );
    }
  }

  // resize "computed" section
  if ( m_ItemButton.size() > 0 )
  {
    QMap<QPushButton*, asiUI_ParameterEditorItem*>::Iterator anIt =
      m_ItemButton.begin();

    QPushButton* aBtn = anIt.key();
    header()->resizeSection(Column_Computed, aBtn->width() + 2);
  }
  else
  {
    header()->resizeSection(Column_Computed, 0);
  }

  // expand all groups
  expandAll();

  enableSignalProcessing(isSignals);
}

//! Update list of displayed parameters. All columns, including
//! name, computed state, value can be updated. 
//! This method should be used carefully: it updates only parameter
//! values, (e.g. name, value, computed state), but it doesn't
//! update datum editors or inserts/removes computable buttons.
//! \param theParameters [in] list of updated parameters.
void asiUI_ParameterEditorView::UpdateParameters(const asiUI_ParamDataList& theParameters)
{
  bool isSignals = enableSignalProcessing(false);

  // create temporary tree
  QTreeWidgetItem* aRootItem = new QTreeWidgetItem();
  QList<asiUI_ParameterEditorData>::ConstIterator aParamIt =
    theParameters.constBegin();

  QTreeWidgetItem* aGroupParent = aRootItem;

  asiUI_ParameterEditorItem* anItemParam = 0;
  for ( ; aParamIt != theParameters.constEnd(); aParamIt++ )
  {
    const asiUI_ParameterEditorData& aParam = *aParamIt;

    if ( aParam.IsGroup() )
    {
      // create new group item
      aGroupParent = new asiUI_ParameterEditorItem(
        aRootItem, aParam.GetName(), aParam.GetNodeID(), 0, 0);

      // disable editing
      aGroupParent->setFlags(aGroupParent->flags() & ~Qt::ItemIsEditable);
    }
    else
    {
      // create item for parameter.
      anItemParam = new asiUI_ParameterEditorItem(
        aGroupParent, aParam.GetName(), aParam.GetNodeID(), 0, 0);

      anItemParam->SetValue(aParam.GetValue());
      anItemParam->SetComputed(aParam.GetComputed());
      if ( aParam.GetComputed() == true )
        anItemParam->setFlags(anItemParam->flags() & ~Qt::ItemIsEditable);
      else
        anItemParam->setFlags(anItemParam->flags() | Qt::ItemIsEditable);

      anItemParam->SetEnabled( aParam.IsEnabled() );
    }
  }

  synchronize<QTreeWidgetItem*, QTreeWidgetItem*, TreeSync>( 
    aRootItem, invisibleRootItem(), TreeSync(this));

  enableSignalProcessing(isSignals);
}

//! Remove all parameters
void asiUI_ParameterEditorView::ClearParameters()
{
  closeEditors();

  bool isSignals = enableSignalProcessing(false);

  QMap<QPushButton*, asiUI_ParameterEditorItem*>::iterator
    aButIt = m_ItemButton.begin();

  for ( ; aButIt != m_ItemButton.end(); aButIt++ )
    aButIt.key()->deleteLater();

  m_ItemButton.clear();

  // remove previous parameters
  this->clear();
  //QTreeWidgetItem* anItem = invisibleRootItem();
  //for ( int i = anItem->childCount(); i >= 0; i-- )
  //  delete anItem->child(i);

  enableSignalProcessing(isSignals);
}

//! Checks whether the editor is in "read only" state.
//! \return Boolean value indicating whether the editor's
//!         fields can be edited or not.
bool asiUI_ParameterEditorView::IsReadOnly() const
{
  return ( editTriggers() == QAbstractItemView::NoEditTriggers );
}

//! Set "read only" state for the parameter editor.
//! \param theIsReadOnly [in] specifies whether the
//!        editor should be set to "read only" state or not.
void asiUI_ParameterEditorView::SetReadOnly(const bool theIsReadOnly)
{
  closeEditors();

  QAbstractItemView::EditTriggers aTriggers =
    ( theIsReadOnly == false ) ? QAbstractItemView::EditKeyPressed  |
                                 QAbstractItemView::AnyKeyPressed
                               : QAbstractItemView::NoEditTriggers;

  setEditTriggers(aTriggers);

  QMap<QPushButton*, asiUI_ParameterEditorItem*>::Iterator aButtonIt = 
    m_ItemButton.begin();

  for ( ; aButtonIt != m_ItemButton.end(); aButtonIt++)
  {
    QPushButton* aButton = aButtonIt.key();
    asiUI_ParameterEditorItem* aCorrespondingItem = aButtonIt.value();
    aButton->setEnabled(!theIsReadOnly && aCorrespondingItem->IsEnabled());
  }
}

//! Override list of values for combo box parameter. The parameter must be defined in the parameter
//! editor. The values are cleared once the parameter list is changed.
//! Exception is raised if the parameter is not combo or is not defined in parameter editor.
//! The passed collections should have the same size.
//! \param theParameterId [in] the id of parameter.
//! \param theStrings [in] the names to display in combo-box.
//! \param theIds [in] the item ids or values to use if theValueMap is not defined.
void asiUI_ParameterEditorView::SetComboValues( const int theParameterId,
                                                 const QStringList& theStrings,
                                                 const QIntList& theIds )
{
  if ( theStrings.isEmpty()
    || theIds.isEmpty()
    || theIds.length() != theStrings.length() )
  {
    Standard_ProgramError::Raise("Empty or incompatible by length collections.");
  }

  asiUI_ParameterEditorItem* anItem = GetItem(theParameterId);
  if (anItem == NULL)
  {
    Standard_ProgramError::Raise("The parameter is not defined.");
  }

  asiUI_DatumComboBox* aCombo = qobject_cast<asiUI_DatumComboBox*>( anItem->GetDatum() );
  if (aCombo == NULL)
  {
    Standard_ProgramError::Raise("The parameter is not combo box.");
  }

  bool isSendingSignals = enableSignalProcessing(false);

  // remember current values
  QVariant aValue = anItem->GetValue();

  aCombo->SetValues(theIds, theStrings);

  // refresh value for new combo box contents.
  anItem->SetValue(aValue);

  enableSignalProcessing(isSendingSignals);
}

//! Get data item for the specified model index.
//! \param theIndex [in] model index of the data item.
//! \return data item.
asiUI_ParameterEditorItem* 
  asiUI_ParameterEditorView::GetItem(const QModelIndex& theIndex) const
{
  return (asiUI_ParameterEditorItem*)itemFromIndex(theIndex);
}

//! Get data item for the specified nodal id.
//! \param theParameterId [in] nodal id of the requested parameter item.
//! \return data item.
asiUI_ParameterEditorItem* 
  asiUI_ParameterEditorView::GetItem(const int theParameterId) const
{
  QModelIndexList aFindIndexes = model()->match( model()->index( 0, 0, QModelIndex() ),
                                                 asiUI_ParameterEditorItem::Role_ParamNodeID,
                                                 theParameterId, 1, Qt::MatchRecursive );
  return !aFindIndexes.isEmpty() 
    ? GetItem( aFindIndexes.first() )
    : NULL;
}

//! Close all opened editors and revert model cache.
void asiUI_ParameterEditorView::closeEditors()
{
  while ( m_EditorMap.begin() != m_EditorMap.end() )
    closeEditor(m_EditorMap.begin().key(), QAbstractItemDelegate::RevertModelCache);
}

//! Draw top level rows with indentation.
void asiUI_ParameterEditorView::drawRow(QPainter* thePainter,
                                         const QStyleOptionViewItem& theOption, 
                                         const QModelIndex &theIndex) const
{ 
  QTreeWidget::drawRow(thePainter, theOption, theIndex);

  // draw cell borders
  thePainter->setPen(GetBorderColor());

  // draw first cell without indentation
  int aRow = theIndex.row();
  QRect aCell = visualRect(theIndex.sibling(aRow, 0));
  aCell.setLeft(columnViewportPosition(0));
  thePainter->drawRect(aCell);

  for( int i = 1; i < columnCount(); i++ )
  {
    aCell = visualRect(theIndex.sibling(aRow, i));
    thePainter->drawRect(aCell);
  }
}

//! Enable edit event for "value" column only for mouse triggers.
//! Redirect keyboard edit triggers to "value" column.
bool asiUI_ParameterEditorView::edit(const QModelIndex& theIndex, 
                                      QAbstractItemView::EditTrigger theTrigger,
                                      QEvent* theEvent)
{
  // redirect all edit events on value column.
  if ( ( theTrigger & editTriggers() ) != 0 )
  {
    return QTreeWidget::edit(
      theIndex.sibling(theIndex.row(), Column_Value), theTrigger, theEvent);
  }

  return QTreeWidget::edit(theIndex, theTrigger, theEvent);
}

//! Handle backspace for editing purposes.
//! \param theEvent [in] the event.
void asiUI_ParameterEditorView::keyPressEvent(QKeyEvent *theEvent)
{
  // backspace input
  if ( theEvent->key() == Qt::Key_Backspace )
  {
    if ( edit(currentIndex(), AnyKeyPressed, theEvent) ) 
      theEvent->ignore();
    else
      theEvent->accept();

    return;
  }

  // cursor on enter
  if ( theEvent->key() == Qt::Key_Enter || theEvent->key() == Qt::Key_Return )
  {
    if ( m_EditorMap.empty() )
    {
      if ( theEvent->modifiers() == Qt::ShiftModifier )
        setCurrentIndex(moveCursor(QAbstractItemView::MovePrevious, Qt::NoModifier));
      else
        setCurrentIndex(moveCursor(QAbstractItemView::MoveNext, Qt::NoModifier));
    }
  }

  QTreeWidget::keyPressEvent(theEvent);
}

//! Send parameter changed signal.
//! \param theItem [in] changed item.
//! \param theColumn [in] column id within the item
void asiUI_ParameterEditorView::onItemChanged(QTreeWidgetItem* theItem, int theColumn)
{
  if ( theColumn != Column_Value )
    return;

  asiUI_ParameterEditorItem* anItem = 
    dynamic_cast<asiUI_ParameterEditorItem*>(theItem);

  if ( anItem && isProcessingSignal() )
    emit ParamModified(anItem->GetNodeId(), anItem->GetValue());
}

//! Callback to start editing when item is pressed.
//! The single click activation behavior is provided by special style hint.
//! \param theItem [in] the item to activate.
//! \param theColumn [in] the column to activate.
void asiUI_ParameterEditorView::onItemPressed(QTreeWidgetItem* theItem, int theColumn)
{
  if ( !IsReadOnly() && theColumn == Column_Value )
  {
    editItem(theItem, theColumn);
  }
}

//! Handle postponed events.
//! \param theEvent [in] the event
bool asiUI_ParameterEditorView::event (QEvent *theEvent)
{
  // delayed event for "data modified" notifications
  //if ( theEvent->type() == ModificationEvent::type() )
  //{
  //  ModificationEvent* aEv = static_cast<ModificationEvent*>(theEvent);

  //  asiUI_ParameterEditorItem* anItem = aEv->GetItem();
  //  if ( anItem )
  //    emit ParamModified(anItem->GetNodeId(), anItem->GetValue());
  //  
  //  aEv->accept();
  //  return true;
  //}

  if ( theEvent->type() == QEvent::EnabledChange )
  {
    // close active editors when enabled state changes
    closeEditors();
  }

  //return true;
  return QTreeWidget::event(theEvent);
}

//! Slot activated when computed state changed for the parameter.
//! \param theIsToggled [in] boolean value indicating whether the parameter
//!        should be computed by eval string or not.
void asiUI_ParameterEditorView::onComputedToggled(const bool theIsToggled)
{
  if ( QPushButton* aBtn = qobject_cast<QPushButton*>(sender()) )
  {
    if ( m_ItemButton.contains(aBtn) )
    {
      asiUI_ParameterEditorItem* anItem =
        (asiUI_ParameterEditorItem*)m_ItemButton.value(aBtn);

      if ( isProcessingSignal() )
        emit ParamComputedChanged(anItem->GetNodeId(), theIsToggled);
    }
  }
}

//! create datum for parameter by its dictionary string or
//! type of data.
//! \param theParam [in] provides value data and description
//!        of data model parameter for the parameter editor.
//! \return datum for the parameter.
asiUI_Datum* asiUI_ParameterEditorView::createDatum(const asiUI_ParameterEditorData& theParam)
{
  asiUI_Datum*
    datum = m_factory->CreateEditor(theParam.GetDictID(),
                                    this,
                                    asiUI_Datum::Control);

  // create datum by value type
  if ( !datum )
  {
    // boolean data type by default get check box datum.
    // all other parameters and data dictionary data types by default get line editor.
    switch ( theParam.GetValue().type() )
    {
      case QVariant::Bool :
        datum = new asiUI_DatumCheckBox("", this, asiUI_Datum::Control); break;

      default:
        datum = new asiUI_DatumLineEdit("", this, asiUI_Datum::Control); break;
    }
  }

  return datum;
}

//-----------------------------------------------------------------------------
// Editor handling
//-----------------------------------------------------------------------------

//! Register editor and handle its activate/close events.
//! \param theEditor [in] the editor.
//! \param theIndex [in] index of the item being edited by the editor.
void asiUI_ParameterEditorView::onEditorCreated(QWidget* theEditor,
                                                 const QModelIndex& theIndex)
{
  asiUI_ParameterEditorItem* anItem = 
    dynamic_cast<asiUI_ParameterEditorItem*>(itemFromIndex(theIndex));
  if ( anItem == 0 )
    return;

  // insert editor to the internal map to
  // have a reference on edited item
  m_EditorMap.insert(theEditor, anItem);

  // install show event filter
  theEditor->installEventFilter(this);
}

//! Callback for custom selector triggering event.
//! \param theIndex [in] the triggered index.
void asiUI_ParameterEditorView::onCustomSelectorTriggered(const QModelIndex& theIndex)
{
  asiUI_ParameterEditorItem* anItem = 
    dynamic_cast<asiUI_ParameterEditorItem*>(itemFromIndex(theIndex));
  if ( anItem == 0 )
    return;

  emit CustomSelectorTriggered( anItem->GetNodeId() );
}

//! Filter editor's events.
//! \param theWatched [in] the events are processed for this object.
//! \param theEvent [in] the event.
bool asiUI_ParameterEditorView::eventFilter(QObject* theWatched, QEvent* theEvent)
{
  QWidget* anEditor = qobject_cast<QWidget*>(theWatched);
  if ( !anEditor || !m_EditorMap.contains(anEditor) )
    return QTreeWidget::eventFilter(theWatched, theEvent);

  // process editor's events  
  switch ( theEvent->type() )
  {
    // process show editor events, to send appropriate notification
    case QEvent::Show : 
    {
      asiUI_ParameterEditorItem* anItem =
        dynamic_cast<asiUI_ParameterEditorItem*>(m_EditorMap.value(anEditor));

      if ( anItem != 0 )
        emit ParamEditorActivated(anItem->GetNodeId());
    }
    break;

    // post-process hide events, the hide events might be sent when
    // the parameter view itself looses its focus without committing
    // the data from editor. In that case, editor should be 
    // removed from the internal map and appropriate notification sent.
    case QEvent::Hide :
    {
      asiUI_ParameterEditorItem* anItem =
        dynamic_cast<asiUI_ParameterEditorItem*>(m_EditorMap.value(anEditor));

      if ( anItem != 0 )
        emit ParamEditorClosed(anItem->GetNodeId(), false);

      m_EditorMap.remove(anEditor);
    }
    break;
      
    default:
      break;
  }

  return QTreeWidget::eventFilter(theWatched, theEvent);
}

//! Send notify signal when parameter editor closes.
//! \param theEditor [in] the editor.
//! \param theEndHint [in] hint that specifies the behavior of
//!        selection after the editor is closed.
void asiUI_ParameterEditorView::closeEditor( QWidget* theEditor, QAbstractItemDelegate::EndEditHint theEndHint )
{
  bool hasEditor = m_EditorMap.contains(theEditor);
  
  // send notification signal
  asiUI_ParameterEditorItem* anItem = m_EditorMap.value(theEditor, 0);
  
  // remove editor from internal map.
  if ( m_EditorMap.contains(theEditor) )
  {
    m_EditorMap.remove(theEditor);
  }
  
  // clear focus to avoid focus changes on hide
  bool isFocused = theEditor->hasFocus();
  if ( isFocused )
    theEditor->clearFocus();
  
  // process editor animation and hide
  theEditor->repaint();
  theEditor->hide();
  
  // do not let focus go out of item view
  if ( isFocused )
    this->setFocus();
  
  if ( anItem != 0 && hasEditor && isProcessingSignal() )
  {
    emit ParamEditorClosed( anItem->GetNodeId(), theEndHint != QAbstractItemDelegate::RevertModelCache );
  }
  
  QTreeWidget::closeEditor(theEditor, theEndHint);
  
  if ( theEndHint == QAbstractItemDelegate::SubmitModelCache )
  {
    setCurrentIndex( moveCursor(QAbstractItemView::MoveNext, Qt::NoModifier) );
  }
}

//-----------------------------------------------------------------------------
// Tree sync
//-----------------------------------------------------------------------------

//! Check if item corresponds to the specified data object.
//! \param theSrc source parameter
//! \param theTrg target parameter in the tree.
//! \return true if item corresponds to the data object.
bool asiUI_ParameterEditorView::TreeSync::isEqual(
  const QTreeWidgetItemPtr& /*theSrc*/,
  const QTreeWidgetItemPtr& /*theTrg*/) const
{
  // tree items always corresponds to each other,
  // its values can be updated in updateItem method.
  return true;
}

//! Get null tree item.
//! \return null tree item.
asiUI_ParameterEditorView::TreeSync::QTreeWidgetItemPtr
  asiUI_ParameterEditorView::TreeSync::nullSrc() const
{
  return 0;
}

//! Get null tree item.
//! \return null tree item.
asiUI_ParameterEditorView::TreeSync::QTreeWidgetItemPtr
  asiUI_ParameterEditorView::TreeSync::nullTrg() const
{
  return 0;
}

//! Create an item corresponding to the specified data object.
//! \param theSrc [in] input data object.
//! \param theParent [in] parent tree item.
//! \param theAfter [in] tree item after each new one should be inserted.
//! \return created item.
asiUI_ParameterEditorView::TreeSync::QTreeWidgetItemPtr
  asiUI_ParameterEditorView::TreeSync::createItem(
    const QTreeWidgetItemPtr& theSrc,
    const QTreeWidgetItemPtr& theParent, 
    const QTreeWidgetItemPtr& theAfter) const
{
  int anIndex = -1;
  for ( int i = 0; i < theParent->childCount(); i++ )
  {
    if ( theParent->child(i) == theAfter )
      anIndex = i;
  }

  asiUI_ParameterEditorItem* aSrc = (asiUI_ParameterEditorItem*)theSrc;

  asiUI_ParameterEditorItem* aNewItem =
    new asiUI_ParameterEditorItem(0, aSrc->GetName(), aSrc->GetNodeId(),
                                   aSrc->GetDicItem(), aSrc->GetDatum());

  aNewItem->SetValue(aSrc->GetValue());
  aNewItem->SetComputed(aSrc->GetComputed());

  if ( anIndex < 0 )
    theParent->addChild(aNewItem);
  else
    theParent->insertChild(anIndex, aNewItem);

  return aNewItem;
}

//! Update tree item.
//! \param theSrc [in] reference data object
//! \param theItem [in] tree item to be updated
void asiUI_ParameterEditorView::TreeSync::updateItem(
  const QTreeWidgetItemPtr& theSrc,
  const QTreeWidgetItemPtr& theTrg) const
{
  // cast to parameter's item type
  asiUI_ParameterEditorItem* aSrc =
    dynamic_cast<asiUI_ParameterEditorItem*>(theSrc);
  asiUI_ParameterEditorItem* aTrg = 
    dynamic_cast<asiUI_ParameterEditorItem*>(theTrg);

  if ( aSrc == 0 || aTrg == 0 )
    return;

  // update name, value
  aTrg->SetName(aSrc->GetName());
  aTrg->SetValue(aSrc->GetValue());
  aTrg->SetComputed(aSrc->GetComputed());
  aTrg->setFlags(aSrc->flags());
  
  // update computed state
  QPushButton* aBtn = 
    qobject_cast<QPushButton*>(m_pView->itemWidget(aTrg, Column_Computed));
  if ( aBtn != 0 )
  {
    aBtn->setEnabled(!m_pView->IsReadOnly() && aTrg->IsEnabled());
    aBtn->setChecked(aTrg->GetComputed());
  }
}

//! Delete item with all children recursively.
//! \param theItem [in] tree item.
void asiUI_ParameterEditorView::TreeSync::deleteItemWithChildren(
  const QTreeWidgetItemPtr& theItem) const
{
  QTreeWidgetItemPtr aParent = theItem->parent();
  
  if ( aParent != 0 )
    aParent->removeChild(theItem);
}

//! Get all the children of the tree item.
//! \param obj tree item.
//! \return list of the children
QList<asiUI_ParameterEditorView::TreeSync::QTreeWidgetItemPtr>
  asiUI_ParameterEditorView::TreeSync::children(
    const QTreeWidgetItemPtr& theObject) const
{
  QList<QTreeWidgetItemPtr> aChildren;
  if ( theObject )
  {
    for ( int i = 0; i < theObject->childCount(); i++ )
    {
      aChildren.append(theObject->child(i));
    }
  }

  return aChildren;
}

//! Get item which is the parent for the specified item.
//! \param theItem [in] the tree item.
//! \return parent item.
asiUI_ParameterEditorView::TreeSync::QTreeWidgetItemPtr
  asiUI_ParameterEditorView::TreeSync::parent(const QTreeWidgetItemPtr& theItem) const
{
  return theItem ? dynamic_cast<QTreeWidgetItemPtr>( theItem->parent() ) : 0;
}

//! Check if the tree item needs updating.
//! \param theItem [in] tree item to be checked.
//! \return true if item needs updating
bool asiUI_ParameterEditorView::TreeSync::needUpdate(
  const QTreeWidgetItemPtr& /*theItem*/) const
{
  return true;
}
