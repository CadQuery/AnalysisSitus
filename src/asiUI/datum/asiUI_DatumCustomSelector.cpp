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
// Own include
#include <asiUI_DatumCustomSelector.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>
#include <asiAlgo_Dictionary.h>

// SUIT includes
//#pragma warning(push, 0)
//#include <SUIT_Session.h>
//#include <SUIT_ResourceMgr.h>
//#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#pragma warning(pop)

#define ICON_OPTION_NAME "icon"
#define LINE_EDIT_OBJ_NAME "line edit"
#define BUTTON_OBJ_NAME "button"
#define LABEL_MIN_WIDTH 50

//! Construct datum.
//! \param theDictId [in] dictionary record item for the datum.
//! \param theParent [in] parent object for the datum.
//! \param theFlags [in] datum initialization flags.
asiUI_DatumCustomSelector::asiUI_DatumCustomSelector(const QString& theDictId,
                                                     QWidget* theParent,
                                                     const int theFlags)
: asiUI_Datum(theParent)
{
  // create and initialize internal datum editor
  m_pEditor = new Editor(theDictId, theParent, theFlags);

  init(m_pEditor);

  // retranslate signals
  connect( m_pEditor, SIGNAL( SelectorTriggered() ), this, SIGNAL( SelectorTriggered() ) );
}

//! Access view control used as a value display component of the datum.
//! The method provides access to the lowest level and should
//! be used with advisedly. The general purpose is to fine-tune
//! properties and outlook of the editor widget.
QLineEdit* asiUI_DatumCustomSelector::AccessValueView() const
{
  return m_pEditor->GetValueView();
}

//! Access button control used as a action provider component of the datum.
//! The method provides access to the lowest level and should
//! be used with advisedly. The general purpose is to fine-tune
//! properties and outlook of the editor widget.
QToolButton* asiUI_DatumCustomSelector::AccessButton() const
{
  return m_pEditor->GetButton();
}

//! Proceed callback when units system settings changes.
void asiUI_DatumCustomSelector::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

//! \return the internal datum control.
QDS_Datum* asiUI_DatumCustomSelector::getDatum() const
{
  return m_pEditor;
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Label datum internals constructor.
//! \param theDictId [in] the dictionary record identifier.
//! \param theParent [in] the parent object for the datum.
//! \param theFlags [in] the initialization flags.
asiUI_DatumCustomSelector::Editor::Editor(
                                  const QString& theDictId,
                                  QWidget* theParent,
                                  const int theFlags)
: QDS_Datum(theDictId, theParent, theFlags)
{
  // access dictionary item
  Handle(asiAlgo_DictionaryItem) anItem = 
    asiAlgo_Dictionary::GetDictionaryItem( QStr2AsciiStr(theDictId) );

  // access resource mananger
  //SUIT_Session* aSession = SUIT_Session::session();
  //SUIT_ResourceMgr* aResMgr = NULL;
  //if ( aSession )
  //  aResMgr = aSession->resourceMgr();

  //if ( anItem.IsNull() || /*!aResMgr ||*/ !anItem->GetWidgetParams().IsBound( ICON_OPTION_NAME ) )
  //{
    m_Icon = QPixmap(asiUI_PixmapNotFound);
 /*   return;
  }

  QString aIconName = AsciiStr2QStr( anItem->GetWidgetParams().Find( ICON_OPTION_NAME ) );

  m_Icon = aResMgr->loadPixmap( aResMgr->appName(), aIconName );*/
}

//! Proceed with unit system changing callback.
void asiUI_DatumCustomSelector::Editor::UpdateUnits()
{
  unitSystemChanged( QString() );
}

//! \return label control.
QLineEdit* asiUI_DatumCustomSelector::Editor::GetValueView() const
{
  QWidget* aWidget = widget(QDS::Control);
  return aWidget->findChild<QLineEdit*>(LINE_EDIT_OBJ_NAME);
}

//! \return button control.
QToolButton* asiUI_DatumCustomSelector::Editor::GetButton() const
{
  QWidget* aWidget = widget(QDS::Control);
  return aWidget->findChild<QToolButton*>(BUTTON_OBJ_NAME);
}

//! \return value string.
QString asiUI_DatumCustomSelector::Editor::getString() const
{
  return (GetValueView() == NULL) ? QString() : GetValueView()->text();
}

//! Set value string.
//! \param theString [in] the string to set up.
void asiUI_DatumCustomSelector::Editor::setString(const QString& theString)
{
  if ( QLineEdit* aLineEdit = GetValueView() )
    aLineEdit->setText(theString);
}

//! Create label control widget.
//! \param theParent [in] the 
QWidget* asiUI_DatumCustomSelector::Editor::createControl(QWidget* theParent)
{
  QFrame* aPane = new QFrame(theParent);
  QHBoxLayout* aLay = new QHBoxLayout(aPane);
  aLay->setMargin(0);
  aLay->setSpacing(5);

  // create value label
  QLineEdit* aLineEdit = new QLineEdit(aPane);
  aLineEdit->setObjectName(LINE_EDIT_OBJ_NAME);
  aLineEdit->setReadOnly(true);

  // create browse button
  QToolButton* aButton = new QToolButton(aPane);
  aButton->setIcon( QPixmap(m_Icon) );
  aButton->setObjectName(BUTTON_OBJ_NAME);

  aLay->addWidget(aLineEdit);
  aLay->addWidget(aButton);

  aPane->setFocusProxy(aButton);
  aPane->setLayout(aLay);

  connect(aButton, SIGNAL( clicked(bool) ), this, SIGNAL( SelectorTriggered() ) );
  return aPane;
}
