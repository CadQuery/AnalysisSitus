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

#ifndef asiUI_ParameterEditorView_HeaderFile
#define asiUI_ParameterEditorView_HeaderFile

// asiUI includes
#include <asiUI_ParameterEditor.h>
#include <asiUI_WidgetFactory.h>

// Qt includes
#pragma warning(push, 0)
#include <QTreeWidget>
#include <QEvent>
#include <QApplication>
#pragma warning(pop)

class asiUI_ParameterEditorItem;
class asiUI_Datum;
class QPushButton;

//-----------------------------------------------------------------------------

//! View widget for parameters display.
class asiUI_ParameterEditorView : public QTreeWidget
{
  Q_OBJECT

public:

  class TreeSync;

  //! Column enumeration.
  enum ColumnID
  {
    Column_Name     = 0,
    Column_Computed = 1,
    Column_Value    = 2
  };

public:

  asiUI_ParameterEditorView(const Handle(asiUI_WidgetFactory)& factory,
                            QWidget* theParent);

  ~asiUI_ParameterEditorView();

  void SetParameters(const asiUI_ParamDataList& theParameters);

  void UpdateParameters(const asiUI_ParamDataList& theParameters);

  void ClearParameters();

  bool IsReadOnly() const;

  void SetReadOnly(const bool theIsReadOnly);

  void SetComboValues(const int theParameterId,
                      const QStringList&     theStrings,
                      const QList<int>&      theIds);

  //! Set the color of borders for the cells.
  //! \param theBorderColor [in] color.
  void SetBorderColor(const QColor& theBorderColor)
  {
    m_BorderColor = theBorderColor;
  }

  //! Get color of cell's border.
  //! \return the border color.
  QColor GetBorderColor() const
  {
    return m_BorderColor;
  }

  asiUI_ParameterEditorItem* GetItem(const QModelIndex& theIndex) const;

  asiUI_ParameterEditorItem* GetItem(const int theParameterId) const;

  virtual bool eventFilter(QObject* theWatched,
                           QEvent*  theEvent);

protected:

  void closeEditors();

  void drawRow(QPainter*                   thePainter,
               const QStyleOptionViewItem& theOption,
               const QModelIndex&          theIndex) const;

  virtual bool edit(const QModelIndex&             theIndex,
                    QAbstractItemView::EditTrigger theTrigger,
                    QEvent*                        theEvent);

  virtual void keyPressEvent(QKeyEvent *theEvent);

  virtual bool event (QEvent *theEvent);

signals:

  //! signal emitted when editor activates for editing
  //! the parameter.
  //! \param theParamID [in] parameter identifier within the node.
  void ParamEditorActivated(const int theParamID);

  //! signal emitted when parameter editor closes.
  //! \param theParamID [in] parameter identifier within the node.
  //! \param theIsCommit [in] Boolean value indicating whether the
  //!        editor closes with committing or rejecting the data.
  void ParamEditorClosed(const int theParamID, const bool theIsCommited);

  //! signal emitted when user changed value field of the parameter.
  //! \param theParamID [in] parameter identifier within the node.
  //! \param theValue [in] new value.
  void ParamModified(const int theParamID, const QVariant& theValue);

  //! signal emitted when user changes "computed" state for the parameter
  //! by click corresponding check-box.
  //! \param theParamID [in] parameter identifier within the node.
  //! \param theIsComputed [in] Boolean value indicating whether the parameter
  //!        should be evaluated by its expression or not.
  void ParamComputedChanged(const int theParamID, const bool theIsComputed);

  //! signal emitted when user triggers action button of custom selector
  //! datum set for the parameter.
  //! \param theParamID [in] parameter identifier within the node.
  void CustomSelectorTriggered(const int theParamID);

  //! internal signal used to notify "computed" button controls when 
  //! "read only" state is set or removed.
  void readOnlyChanged(const bool theIsReadOnly);

protected slots:

  virtual void closeEditor(QWidget* theEditor, 
                           QAbstractItemDelegate::EndEditHint theEndHint);

private slots:

  void onItemChanged(QTreeWidgetItem* theItem, int theColumn);

  void onItemPressed(QTreeWidgetItem* theItem, int theColumn);

  void onComputedToggled(const bool theIsToggled);

  void onEditorCreated(QWidget*           theEditor,
                       const QModelIndex& theIndex);

  void onCustomSelectorTriggered(const QModelIndex& theIndex);

  //! enable/disable custom processing of the signals.
  //! \param theIsEnabled [in] the value that indicates the
  //!        the state of signals processing.
  //! \return previous state
  bool enableSignalProcessing(bool theIsEnabled)
  {
    bool isProcessed = m_bSignalsEnabled;

    m_bSignalsEnabled = theIsEnabled;

    return isProcessed;
  }

  //! indicates whether the custom processing of the signal is enabled/disabled.
  //! \return Boolean value corresponding to the processing state.
  bool isProcessingSignal() const
  {
    return m_bSignalsEnabled;
  }

  asiUI_Datum* createDatum(const asiUI_ParameterEditorData& theParam);

private:

  void onModelUpdated(const QModelIndex& theIndex);

  //! map of item widgets
  QMap<QPushButton*, asiUI_ParameterEditorItem*> m_ItemButton;

  //! map of editor widgets for items
  QMap<QWidget*, asiUI_ParameterEditorItem*> m_EditorMap;

  //! custom palette
  QColor m_BorderColor;

  bool m_bSignalsEnabled;

  //! Factory of datums.
  Handle(asiUI_WidgetFactory) m_factory;
};

//! \ingroup GUI
//!
//! Tree synchronizer for object browser
class asiUI_ParameterEditorView::TreeSync
{
public:

  typedef QTreeWidgetItem* QTreeWidgetItemPtr;
  
public:
  
  //! Constructor
  TreeSync(asiUI_ParameterEditorView* theView) 
  : m_pView(theView)
  {
  }

  //! Destructor
  virtual ~TreeSync()
  {
    m_pView = 0;
  }

  bool isEqual(const QTreeWidgetItemPtr& theSrc,
               const QTreeWidgetItemPtr& theTrg) const;

  QTreeWidgetItemPtr nullSrc() const;

  QTreeWidgetItemPtr nullTrg() const;

  QTreeWidgetItemPtr createItem(const QTreeWidgetItemPtr& theSrc,
                                const QTreeWidgetItemPtr& theParent,
                                const QTreeWidgetItemPtr& theAfter) const;

  void updateItem(const QTreeWidgetItemPtr& theSrc,
                  const QTreeWidgetItemPtr& theTrg) const;

  void deleteItemWithChildren(const QTreeWidgetItemPtr& theItem) const;

  QList<QTreeWidgetItemPtr> children(const QTreeWidgetItemPtr& theParent) const;

  QTreeWidgetItemPtr parent(const QTreeWidgetItemPtr& theChild) const;

private:

  bool needUpdate(const QTreeWidgetItemPtr& theObj) const;

private:

  QPointer<asiUI_ParameterEditorView> m_pView;
};

#endif
