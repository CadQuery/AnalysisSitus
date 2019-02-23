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
#include <asiUI_DatumSelector.h>

// Qt includes
#pragma warning(push, 0)
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QEvent>
#include <QSizePolicy>
#include <QFont>
#include <QKeyEvent>
#include <QValidator>
#pragma warning(pop)

//! Constructor.
//! \param theDicID [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theDatumFlags [in] datum subwidget and option flags.
asiUI_DatumSelector::asiUI_DatumSelector(const QString& theDicID, 
                                           QWidget* theParent,
                                           const int theDatumFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDicID, theParent, convertFlags(theDatumFlags));
  init(m_pEditor);

  connect(m_pEditor, SIGNAL(SelectionFinished()), SIGNAL(SelectionFinished()));
}

//! Update unit system.
void asiUI_DatumSelector::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumSelector::getDatum() const
{
  return m_pEditor;
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Constructor. Initializes controls and dialog mode
asiUI_DatumSelector::Editor::Editor(const QString& theDicID,
                                     QWidget* theParent,
                                     const int theFlags)
: QDS_Datum(theDicID, theParent, theFlags)
{
}

//! Update system of units.
void asiUI_DatumSelector::Editor::UpdateUnits()
{
  unitSystemChanged( QString() );
}

//! Proceed with changing of unit system.
//! \param theUnitsId [in] unit system identifier.
void asiUI_DatumSelector::Editor::unitSystemChanged(const QString& theUnitsId)
{
  QDS_Datum::unitSystemChanged(theUnitsId);

  QLineEdit* aLineEdit = lineEdit();
  if ( !aLineEdit )
    return;

  if ( aLineEdit->validator() )
    delete aLineEdit->validator();

  aLineEdit->setValidator( validator() );
}

//! Create control widget
QWidget* asiUI_DatumSelector::Editor::createControl(QWidget* theParent)
{
  QFrame* anEditorFrame = new QFrame(theParent);

  QHBoxLayout* aLay = new QHBoxLayout(anEditorFrame);
  aLay->setMargin(0);
  aLay->setSpacing(2);

  // create editor
  QLineEdit* anEditor = new QLineEdit(anEditorFrame);
  anEditor->setObjectName("LineEditor");
  anEditor->setReadOnly(true);
  anEditor->setValidator( validator() );

  // create browse button
  QToolButton* aBtnOk = new QToolButton(anEditorFrame);
  aBtnOk->setText(tr("BUTTON_OK"));
  
  QFont aFont = aBtnOk->font();
  aFont.setPointSize(aFont.pointSize() - 1);
  aBtnOk->setFont(aFont);

  QSizePolicy aPolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  aPolicy.setHeightForWidth(true);
  aBtnOk->setSizePolicy(aPolicy);

  aLay->addWidget(anEditor, 10);
  aLay->addWidget(aBtnOk, 0);

  anEditorFrame->setLayout(aLay);
  anEditorFrame->setFocusProxy(anEditor);

  connect(aBtnOk, SIGNAL(clicked()), this, SIGNAL(SelectionFinished()));

  return anEditorFrame;
}

//! Get line edit
QLineEdit* asiUI_DatumSelector::Editor::lineEdit() const
{
  QWidget* aWidget = widget(QDS::Control);
  return aWidget->findChild<QLineEdit*>("LineEditor");
}

//! Get string value from path edit
//! \return string value
QString asiUI_DatumSelector::Editor::getString() const
{
  QLineEdit* anEditor = lineEdit();
  return ( anEditor != 0 ) ? lineEdit()->text() : QString();
}

//! Set string value to path editor
void asiUI_DatumSelector::Editor::setString(const QString& theString)
{
  QLineEdit* anEditor = lineEdit();
  if ( anEditor != 0 )
    anEditor->setText(theString);
}
