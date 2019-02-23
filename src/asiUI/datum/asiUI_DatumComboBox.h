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

#ifndef asiUI_DatumComboBox_HeaderFile
#define asiUI_DatumComboBox_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// SUIT includes
#pragma warning(push, 0)
#include <QDS_ComboBox.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QStringList>
#pragma warning(pop)

//! \ingroup GUI
//!
//! Datum with control corresponding to the combo box. 
//! This control is used for datum with enumerable values.
//! It can be used for datum which has type of value 'List'.
//!
//! Each item of the combo box is defined by two properties: integer identifier
//! and string name. All operations on items are performed via identifier.
//!
//! Current item can be manipulated through identifier: GetInteger, SetInteger
//! datum's methods, or by item's string name: GetString, SetString methods.
class asiUI_EXPORT asiUI_DatumComboBox : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  asiUI_DatumComboBox(const QString& theDicID, 
                       QWidget*       theParent = 0,
                       const int      theDatumFlags = All);

  bool IsEditable() const;

  void SetEditable(const bool theIsEditable);
  
  int GetItemCount(bool theIsTotal = false) const;

  void GetValues(QList<int>& theIDs,
                 bool        theIsTotal = false) const;

  bool GetVisibility(const int theID) const;

  void SetVisibility(const int  theID,
                     const bool theIsVisible,
                     const bool theIsAppend = true);

  void SetVisibility(const QList<int>& theIDs,
                     const bool        theIsVisible,
                     const bool        theIsAppend = true);

  void SetValues(const QList<int>&  theIDs,
                 const QStringList& theNames);

  void SetValues(const QStringList& theNames);

  int GetIdForName(const QString& theName) const;

  QString GetNameForId(const int theID) const;

  inline Editor* GetEditor() const
  {
    return m_pEditor;
  }

  void SetListAutoEnlarge(const bool theToEnlarge);

signals:

  //! Signal when item activated in combo box.
  //! \param theValue [in] item's value (identifier for combo box item).
  void Activated(int theValue);

  //! Signal when item activated in combo box.
  //! \param theStringName [in] item's value string name.
  void Activated(const QString& theStringName);

public slots:

  virtual void onUnitsUpdated();

protected:

  virtual QDS_Datum* getDatum() const;

private:

  Editor* m_pEditor;
};

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! \ingroup GUI
//!
//! Combo-box item editor for asiUI_DatumComboBox.
class asiUI_DatumComboBox::Editor : public QDS_ComboBox
{
  Q_OBJECT

public:

  Editor(const QString& theDicID,
         QWidget*       theParent = 0,
         const int      theFlags = All);

  void SetValues(const QList<int>&  theIDs,
                 const QStringList& theNames);

  void UpdateUnits();

  void SetListAutoEnlarge(const bool theToEnlarge);

public:

  virtual void
    setIntegerValue(const int theVal);

  virtual void
    setDoubleValue(const double theVal);

  virtual void
    setStringValue(const QString& theStr);

protected:

  virtual void setString(const QString& theString);

private:

  bool m_bAutoEnlarge;
};

#endif
