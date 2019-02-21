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

#ifndef asiUI_DatumRadioBox_HeaderFile
#define asiUI_DatumRadioBox_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

#pragma warning(push, 0)
#include <QDS_RadioBox.h>
#include <QStringList>
#pragma warning(pop)

//! \ingroup GUI
//!
//! Datum with control corresponding to button group with set of exclusive radio buttons.
//!
//! This control used for datum with enumerable values. It can be used for datum which has
//! type of value 'List'. Each radio button of group box is defined by two properties:
//! integer identifier and string name. All operations on radio buttons are performed via identifier.
//!
//! Current item can be manipulated through identifier: GetInteger, SetInteger
//! datum's methods, or by item's string name: GetString, SetString methods.
class asiUI_EXPORT asiUI_DatumRadioBox : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  asiUI_DatumRadioBox(const QString& theDicID, 
                       QWidget*       theParent = 0,
                       const int      theDatumFlags = All);

  int GetButtonCount(bool theIsTotal = false) const;

  void GetValues(QList<int>& theIDs, 
                 bool        theIsTotal = false) const;

  bool GetVisibility(const int theID) const;

  void SetVisibility(const int  theID, 
                     const bool theIsVisible,
                     const bool theIsAppend = true);

  void SetVisibility(const QList<int>& theIDs,
                     const bool        theIsVisible,
                     const bool        theIsAppend = true);

  void SetValues(const QList<int>& theIDs, 
                 const QStringList& theNames);

  void SetValues(const QStringList& theNames);

signals:

  //! Signal when radio button is clicked.
  //! \param theID [in] radio box item's id.
  void Activated(int theID);

public slots:

  virtual void onUnitsUpdated();

protected:

  virtual QDS_Datum* getDatum() const;

private slots:

  // re-emit signal.
  void onActivated(int theID)
  {
    emit Activated(theID);
  }

private:

  Editor* m_pEditor;
};

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! \ingroup GUI
//!
//! Check Box editor with boolean value type result.
class asiUI_DatumRadioBox::Editor : public QDS_RadioBox
{
  Q_OBJECT

public:

  Editor(const QString& theDicID,
         QWidget*       theParent = 0,
         const int      theFlags = All);

  //! Update system of units.
  void UpdateUnits()
  {
    unitSystemChanged(QString());
  }
};

#endif

