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

#ifndef asiUI_DatumItemEditor_HeaderFile
#define asiUI_DatumItemEditor_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// Qt includes
#pragma warning(push, 0)
#include <QPointer>
#include <QWidget>
#include <QVariant>
#include <QDialog>
#include <QFocusEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QApplication>
#pragma warning(pop)

#pragma warning(disable: 4251) // "Class XXX needs to have dll-interface..."

//! This is an adapter for datum editors to be used
//! by QItemDelegate. It provides access to editor's
//! value, also it controls focus for complex widgets.
class asiUI_EXPORT asiUI_DatumItemEditor : public QWidget
{
  Q_OBJECT
  
  // Property used by item delegate
  Q_PROPERTY(QVariant value READ GetValue WRITE SetValue USER true)
  
public:

  //! Constructor for datum editor.
  asiUI_DatumItemEditor(asiUI_Datum* theDatum, QWidget* theParent);

  //! Destructor.
  virtual ~asiUI_DatumItemEditor()
  {
    if ( !m_pDatum.isNull() )
    {
      // return parent ship to datum, as the layout might capture it...
      QWidget* aLabWidget  = m_pDatum->GetWidget(asiUI_Datum::Label);
      QWidget* aCntrWidget = m_pDatum->GetWidget(asiUI_Datum::Control);
      QWidget* aUnitWidget = m_pDatum->GetWidget(asiUI_Datum::Units);
      if (aLabWidget != 0)
        aLabWidget->setParent ((QWidget*)m_pDatum->parent());
      if (aCntrWidget != 0)
        aCntrWidget->setParent((QWidget*)m_pDatum->parent());
      if (aUnitWidget != 0)
        aUnitWidget->setParent((QWidget*)m_pDatum->parent());
      m_pDatum->Hide(asiUI_Datum::All);
    }
  }

  void SetAlignment(const Qt::Alignment theAlign);

  void SetMargin(const QMargins& theMargins);

public:

  //! Get datum control.
  //! \return pointer to datum object.
  inline asiUI_Datum* GetDatum() const
  {
    return m_pDatum;
  }

  //! start dialog if control contains it.
  inline virtual void showEvent(QShowEvent*) 
  {
    if ( QDialog* aDlg = dialogControl() )
      aDlg->show();
  }

  //! hide dialog if it is shown.
  inline virtual void hideEvent(QHideEvent*) 
  {
    if ( QDialog* aDlg = dialogControl() )
    {
      if ( aDlg->isVisible() )
        aDlg->close();
    }
  }

  //! The key events should be accepted after sending them through
  //! the descendant editor controls. If the event isn't accepted
  //! by the editor, it will be re-thrown back to the parameter editor view
  //! and, as a result, processed by it twice, causing double editing of the item.
  inline virtual void keyPressEvent(QKeyEvent* theEvent)
  {
    theEvent->accept();
  }

// Value getters/setters
public:

  //! Get string value.
  inline QString GetString() const
  {
    return m_pDatum->GetString();
  }

  //! Get integer value.
  inline int GetInt() const
  {
    return m_pDatum->GetInteger();
  }

  //! Get double value.
  inline double GetDouble() const
  {
    return m_pDatum->GetDouble();
  }

  //! Get QVariant value.
  inline QVariant GetValue() const
  {
    return m_pDatum->GetValue();
  }

  //! Set string value.
  inline void SetString(const QString& theVal)
  {
    m_pDatum->SetString(theVal);
  }

  //! Set integer value.
  inline void SetInteger(const int theVal)
  {
    m_pDatum->SetInteger(theVal);
  }

  //! Set double value.
  inline void SetDouble(const double theVal)
  {
    m_pDatum->SetDouble(theVal);
  }

  //! Set variant value.
  inline void SetValue(const QVariant& theVal)
  {
    m_pDatum->SetValue(theVal);
  }

signals:

  void CommitValue();

  void CloseEditor();

private:

  //! check wheter datum contains dialog.
  QDialog* dialogControl() const
  {
    QWidget* aWidget = m_pDatum->GetWidget(asiUI_Datum::Control);
    if ( !aWidget )
      return 0;

    QDialog* aDlg = qobject_cast<QDialog*>(aWidget->focusProxy());
    return aDlg;
  }

  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);

  void installFilters(QWidget* theWidget);

  void connectDatum(asiUI_Datum* theDatum);

private:

  QPointer<asiUI_Datum> m_pDatum;
};

#pragma warning(default: 4251) // "Class XXX needs to have dll-interface..."

#endif
