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

#ifndef asiUI_DatumSpinBox_HeaderFile
#define asiUI_DatumSpinBox_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_SpinBox.h>
#pragma warning(pop)

class QtxIntSpinBox;

//! Datum with control corresponding to spin box.
//! This control used for integer numbers. 
//! User can input data directly in the spin box or can modify
//! current value by clicking arrow (+/-) buttons.
class asiUI_EXPORT asiUI_DatumSpinBox : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  asiUI_DatumSpinBox(const QString& theDicID, 
                      QWidget*       theParent = 0,
                      const int      theDatumFlags = All);

  int GetStep() const;

  void SetStep(const int theStepSize);

public slots:

  virtual void onUnitsUpdated();

signals:

  //! This signal is emitted when the Return/Enter
  //! key is pressed or the editor loses focus.
  void EditingFinished();

protected:

  virtual QDS_Datum* getDatum() const;

  virtual void colorizeText(const QColor& theColor);

private:

  Editor* m_pEditor;
};


//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! \ingroup GUI
//!
//! integer spin box with syntax coloring.
class asiUI_DatumSpinBox::Editor : public QDS_SpinBox
{
  Q_OBJECT

public:

  Editor(const QString& theDicID,
         QWidget*       theParent = 0,
         const int      theFlags = All);

  void UpdateUnits();

  QtxIntSpinBox* GetSpinBox() { return spinBox(); }

signals:

  //! This signal is emitted when the Return or Enter 
  //! key is pressed or the editor loses focus.
  void EditingFinished();

protected:

  virtual void unitSystemChanged(const QString& theSystem);

};

#endif
