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

#ifndef asiUI_DatumLineEdit_HeaderFile
#define asiUI_DatumLineEdit_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// SUIT includes
#pragma warning(push, 0)
#include <QDS_LineEdit.h>
#pragma warning(pop)

//! \ingroup GUI
//!
//! Datum with control corresponding to the line edit.
//! User can enter parameter value in single line editor.
//! The value entered by the user is checked by the validator according to item type
//! according to the item properties (minimum, maximum, filter, precision).
//! If user input is not valid, the value is displayed in red color.
class asiUI_EXPORT asiUI_DatumLineEdit : public asiUI_Datum
{
  Q_OBJECT

  class LineEditImpl;

public:

  asiUI_DatumLineEdit(const QString& theDictId,
                       QWidget* theParent = 0,
                       const int theFlags = All);

  void Deselect();

  void SelectAll();

  void SetAlignment(const int theAlignment,
                    const int theWidgets);

  bool HasSelection() const;

  void SetSelection(const bool theIsSelected);

signals:

  //! This signal is emitted when the Return/Enter
  //! key is pressed.
  void ReturnPressed();

  //! This signal is emitted when the Return/Enter
  //! key is pressed or the line edit loses
  //! focus.
  void EditingFinished();

protected:

  virtual QDS_Datum* getDatum() const;

private:

  LineEditImpl* m_pDatumImpl;
};

// ============================================================================
//                         Extended line editor
// ============================================================================

//! \ingroup GUI
//!
//! \todo provide comment here
class asiUI_DatumLineEdit::LineEditImpl : public asiUI_Datum::DatumImpl<QDS_LineEdit>
{
  Q_OBJECT

public:

  LineEditImpl(const QString& theDictId,
               QWidget* theParent = 0,
               const int theFlags = All);

signals:

  //! This signal is emitted when the Return or Enter 
  //! key is pressed or the line edit loses focus.
  void EditingFinished();

protected slots:

  virtual void onParamChanged();

protected:

  virtual QWidget* createControl(QWidget* theParent);
};

#endif
