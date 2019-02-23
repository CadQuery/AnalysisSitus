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
#include <asiUI_DatumLineEdit.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#include <QDS_LineEdit.h>
#include <QLineEdit>
#pragma warning(pop)

//! Constructor.
//! \param theDictId [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theFlags [in] datum subwidget and option flags.
asiUI_DatumLineEdit::asiUI_DatumLineEdit(const QString& theDictId, 
                                           QWidget* theParent,
                                           const int theFlags)
: asiUI_Datum(theParent)
{
  m_pDatumImpl = new LineEditImpl(theDictId, theParent, convertFlags(theFlags) );
  init(m_pDatumImpl);

  connect( m_pDatumImpl, SIGNAL( EditingFinished() ), this, SIGNAL( EditingFinished() ) );
  connect( m_pDatumImpl, SIGNAL( returnPressed() ),   this, SIGNAL( ReturnPressed() ) );
}

//! Deselect all text in the editor.
void asiUI_DatumLineEdit::Deselect()
{
  m_pDatumImpl->deselect();
}

//! Select all text in the editor.
void asiUI_DatumLineEdit::SelectAll()
{
  m_pDatumImpl->selectAll();
}

//! Set the aligment for the line edit.
//! \param theAlignment [in] alignment type (Qt::Alignment).
//! \param theWidgets [in] subwidget flags.
void asiUI_DatumLineEdit::SetAlignment(const int theAlignment, const int theWidgets)
{
  m_pDatumImpl->setAlignment(theAlignment, convertWidgets(theWidgets));
}

//! Check if there is selection in the editor.
//! \return true if the editor has selected text.
bool asiUI_DatumLineEdit::HasSelection() const
{
  return m_pDatumImpl->hasSelection();
}

//! Select/deselect all text in the editor.
//! \param theIsSelected [in] select text if true and deselect if false.
void asiUI_DatumLineEdit::SetSelection(const bool theIsSelected)
{
  return m_pDatumImpl->setSelection(theIsSelected);
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumLineEdit::getDatum() const
{
  return m_pDatumImpl;
}

// ============================================================================
//                         Extended line editor
// ============================================================================

//! Default constructor.
//! Is simply void.
asiUI_DatumLineEdit::LineEditImpl::LineEditImpl(const QString& theDictId,
                                                 QWidget* theParent,
                                                 const int theFlags)
: asiUI_Datum::DatumImpl<QDS_LineEdit>(theDictId, theParent, theFlags, true)
{
  //
}

//! Create control widget.
//! \param theParent [in] the parent widget.
//! \return editor control.
QWidget* asiUI_DatumLineEdit::LineEditImpl::createControl(QWidget* theParent)
{
  asiUI_Datum::FormattedLineEdit* anEditor = new asiUI_Datum::FormattedLineEdit(m_Fmt, type(), theParent);
  connect( anEditor, SIGNAL( returnPressed() ), this, SIGNAL( returnPressed() ) );
  connect( anEditor, SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
  connect( anEditor, SIGNAL( editingFinished() ), this, SIGNAL( EditingFinished() ) );
  return anEditor;
}

//! Process parameter changed callback.
//! Do not invoke line edit implementation, only base.
void asiUI_DatumLineEdit::LineEditImpl::onParamChanged()
{
  QDS_Datum::onParamChanged();
}
