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

#ifndef asiUI_DatumCustomSelector_HeaderFile
#define asiUI_DatumCustomSelector_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// SUIT Toolkit includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#pragma warning(pop)

// Forward declaration
class QLineEdit;
class QToolButton;

//! Generic datum that contains two functional widgets:
//! -+ value view (read-only line edit) to display string.
//! -+ button to start custom selection.
//! This datum sends specific signal when
//! user clicks on its button.
//! The representation and event processing in delegate
//! is implemented intrinsically by datum delegate base class.
class asiUI_EXPORT asiUI_DatumCustomSelector : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  asiUI_DatumCustomSelector(const QString& theDictId, 
                             QWidget* theParent = 0,
                             const int theFlags = All);

  QLineEdit* AccessValueView() const;

  QToolButton* AccessButton() const;

signals:

  //! Signal is emitted when selector
  //! button triggered
  void SelectorTriggered();

public slots:

  virtual void onUnitsUpdated();

protected:

  virtual QDS_Datum* getDatum() const;

private:

  Editor* m_pEditor;
};

// ============================================================================
//                         Standard Datum
// ============================================================================

//! \ingroup GUI
//!
//! Custom selector datum internals.
//! Standard label control extended with basics datum services
//! and push button to trigger custom selector action.
class asiUI_DatumCustomSelector::Editor : public QDS_Datum
{
  Q_OBJECT

public:

  Editor(const QString& theDictId,
         QWidget* theParent = 0,
         const int theFlags = All);

  void UpdateUnits();

  QLineEdit* GetValueView() const;

  QToolButton* GetButton() const;

  QPixmap GetIcon() const;

  virtual QString getString() const;

signals:

  //! Signal is emitted when selector
  //! button triggered
  void SelectorTriggered();

protected:

  virtual void setString(const QString& theString);

  virtual QWidget* createControl(QWidget* theParent);

private:

  QPixmap m_Icon; //!< action button icon
};

#endif
