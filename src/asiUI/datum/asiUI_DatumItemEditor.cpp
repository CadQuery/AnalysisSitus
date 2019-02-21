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
#include <asiUI_DatumItemEditor.h>

// asiUI includes
#include <asiUI_DatumSelector.h>
#include <asiUI_DatumCheckBox.h>
#include <asiUI_DatumComboBox.h>
#include <asiUI_DatumPathEdit.h>
#include <asiUI_DatumRadioBox.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QEvent>
#include <QPalette>
#include <QHBoxLayout>
#pragma warning(pop)

// Create editor widget
asiUI_DatumItemEditor::asiUI_DatumItemEditor(asiUI_Datum* theDatum, QWidget* theParent)
: QWidget(theParent),
  m_pDatum(theDatum)
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);

  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(0);

  QWidget* aControl = theDatum->GetWidget(asiUI_Datum::Control);

  aLayout->addWidget(aControl);

  theDatum->Show();

  installFilters(aControl);

  // search for the editor's focus proxy
  QWidget* aProxy = aControl;
  QDialog* aDialog = dialogControl();
  if ( aDialog )
  {
    aProxy = aDialog;
  }
  else
  {
    while ( aProxy->focusProxy() != 0 )
    {
      aProxy = aProxy->focusProxy();
    }

    if ( aProxy )
    {
      aProxy->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding) );
    }
  }

  setFocusProxy(aProxy);
  setFocusPolicy(Qt::ClickFocus);
  setAutoFillBackground(!aDialog);

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding  ) );

  connectDatum(theDatum);
}

//! Set alignment for fixed-size editor control.
//! \param theAlignment [in] the alignment in layout.
void asiUI_DatumItemEditor::SetAlignment(const Qt::Alignment theAlign)
{
  if ( !m_pDatum )
  {
    return;
  }

  QWidget* aControl = m_pDatum->GetWidget(asiUI_Datum::Control);
  if ( !aControl )
  {
    return;
  }

  layout()->setAlignment(aControl, theAlign);
}

//! Set margins for editor layout.
//! \param theMargins [in] the layout margins.
void asiUI_DatumItemEditor::SetMargin(const QMargins& theMargins)
{
  layout()->setContentsMargins(theMargins);
}

//! filter focus out events
bool asiUI_DatumItemEditor::eventFilter(QObject* theObj, QEvent* theEvent)
{
  QWidget* aWidget = qobject_cast<QWidget*>(theObj);

  if ( theEvent->type() == QEvent::ChildAdded )
  {
    QChildEvent* anEvent = dynamic_cast<QChildEvent*>( theEvent );
    if ( anEvent )
    {
      if ( QWidget* aChildWidget = qobject_cast<QWidget*>(anEvent->child()) )
        installFilters(aChildWidget);
    }
  }

  if ( theEvent->type() == QEvent::FocusOut )
  {
    // custom focus handling for selector widget
    if ( asiUI_DatumSelector* aSel = qobject_cast<asiUI_DatumSelector*>( m_pDatum ) )
    {
      theEvent->ignore();
      return true;
    }

    if ( QApplication::focusWidget() != aWidget )
    {
      // skip the internal changes of focus
      // the editor remains active
      QWidget* aWid = QApplication::focusWidget();
      while (aWid)
      {
        if (aWid == this)
        {
          theEvent->ignore();
          return true;
        }

        aWid = (QWidget*)aWid->parentWidget();
      }
  
      // otherwise send focus out event for this editor
      QApplication::sendEvent(this, theEvent);
    }
  }

  return false;
}

//! install focus filters on all child controls
void asiUI_DatumItemEditor::installFilters(QWidget* theWidget)
{
  if ( theWidget->isWindow() )
    return;

  theWidget->installEventFilter(this);

  QList<QWidget*> aWidList = theWidget->findChildren<QWidget*>();
  QList<QWidget*>::Iterator aWidIt = aWidList.begin();
  for( ; aWidIt != aWidList.end(); aWidIt++ )
    installFilters(*aWidIt);
}

//! Connect signals of the datum that normally invokes data commit.
//! \param theDatum [in] datum editor to be connected.
void asiUI_DatumItemEditor::connectDatum(asiUI_Datum* theDatum)
{
  // check box
  if ( qobject_cast<asiUI_DatumCheckBox*>(theDatum) != 0 )
  {
    connect(theDatum, SIGNAL(Toggled(bool)), SIGNAL(CommitValue()));
  }
  // combo box
  else if ( qobject_cast<asiUI_DatumComboBox*>(theDatum) != 0 )
  {
    connect(theDatum, SIGNAL(Activated(int)), SIGNAL(CommitValue()));
  }
  // radio box
  else if ( qobject_cast<asiUI_DatumRadioBox*>(theDatum) != 0 )
  {
    connect(theDatum, SIGNAL(Activated(int)), SIGNAL(CommitValue()));
  }
  // path edit
  else if ( qobject_cast<asiUI_DatumPathEdit*>(theDatum) != 0 )
  {
    connect(theDatum, SIGNAL(PathSelected()), SIGNAL(CommitValue()));
  }
  // selector
  else if ( qobject_cast<asiUI_DatumSelector*>(theDatum) != 0 )
  {
    connect(theDatum, SIGNAL(SelectionFinished()), SIGNAL(CommitValue()));
    connect(theDatum, SIGNAL(SelectionFinished()), SIGNAL(CloseEditor()));
  }
}
