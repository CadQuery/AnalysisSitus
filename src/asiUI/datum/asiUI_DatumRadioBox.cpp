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
#include <asiUI_DatumRadioBox.h>

// SUIT includes
#pragma warning(push, 0)
#include <QDS_RadioBox.h>
#pragma warning(pop)

//! Constructor.
//! \param theDicID [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theDatumFlags [in] datum subwidget and option flags.
asiUI_DatumRadioBox::asiUI_DatumRadioBox(const QString& theDicID, 
                                           QWidget* theParent,
                                           const int theDatumFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDicID, theParent, convertFlags(theDatumFlags));

  init(m_pEditor);

  connect(m_pEditor, SIGNAL(activated(int)), this, SIGNAL(Activated(int)));
}

//! Get number of buttons in radio box.
//! If theIsTotal is false, only visible buttons are taken into account; 
//! otherwise total number of buttons is returned.
//! \param theIsTotal [in] return only visible buttons count if false,
//!        otherwise get total number of buttons in radio box list.
//! \return count of buttons.
int asiUI_DatumRadioBox::GetButtonCount(bool theIsTotal) const
{
  return m_pEditor->count(theIsTotal);
}

//! Get button ids.
//! \param theIDs [out] list of returned IDs
//! \param theIsTotal [in]return only visible buttons ids if false,
//!        otherwise get total number of buttons in radio box list. 
void asiUI_DatumRadioBox::GetValues(QList<int>& theIDs, bool theIsTotal) const
{
  m_pEditor->values(theIDs, theIsTotal);
}

//! Get visibility state of the button specified by theID.
//! \param theID [in] button id.
//! \return button's visibility state.
bool asiUI_DatumRadioBox::GetVisibility(const int theID) const
{
  return m_pEditor->state(theID);
}

//! Set the visibility state of the button specified by theID.
//! If theID is -1 then specified state will be set to all buttons.
//! If theIsAppend append is set to true, keep current statuses already set
//! for other button, otherwise the statuses will be cleared.
//! \param theID [in] button id.
//! \param theIsVisible [in] visibility state.
//! \param theIsAppend [in] if true, keep statuses set for other buttons.
void asiUI_DatumRadioBox::SetVisibility(const int theID, 
                                         const bool theIsVisible,
                                         const bool theIsAppend)
{
  m_pEditor->setState(theIsVisible, theID, theIsAppend);
}

//! Set the visibility state for set of buttons specified by theIDs list.
//! If theIsAppend append is set to true, keep current statuses already set
//! for other buttons, otherwise the statuses will be cleared.
//! \param theIDs [in] list of button ids.
//! \param theIsVisible [in] visibility state.
//! \param theIsAppend [in] if true, keep statuses set for other buttons.
void asiUI_DatumRadioBox::SetVisibility(const QList<int>& theIDs,
                                         const bool theIsVisible,
                                         const bool theIsAppend)
{
  m_pEditor->setState(theIsVisible, theIDs, theIsAppend);
}

//! Add custom user buttons into the radio box.
//! This method allows overriding the buttons.
//! \param theIDs [in] button IDs
//! \param theNames [in] button name strings.
void asiUI_DatumRadioBox::SetValues(const QList<int>& theIDs,
                                     const QStringList& theNames)
{
  m_pEditor->setValues(theIDs, theNames);
}

//! Add custom buttons into the radio box.
//! The ids for buttons will be generated automatically, starting
//! from 0. (0, 1, 2, ...).
//! \param theNames [in] button name strings.
void asiUI_DatumRadioBox::SetValues(const QStringList& theNames)
{
  m_pEditor->setValues(theNames);
}

//! Update unit system.
void asiUI_DatumRadioBox::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumRadioBox::getDatum() const
{
  return m_pEditor;
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Constructor. Initializes controls.
asiUI_DatumRadioBox::Editor::Editor(const QString& theDicID,
                                     QWidget* theParent,
                                     const int theFlags)
: QDS_RadioBox(theDicID, theParent, theFlags)
{
}
