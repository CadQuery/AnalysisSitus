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

#ifndef asiUI_DatumDateEdit_HeaderFile
#define asiUI_DatumDateEdit_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#pragma warning(pop)

// Forward includes
class QDateTimeEdit;
class QDate;
class QTime;
class QDateTime;
class QVariant;

class asiUI_EXPORT asiUI_DatumDateEdit : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  asiUI_DatumDateEdit(const QString& theDicID, 
                       QWidget*       theParent = 0,
                       const int      theDatumFlags = All);

  bool IsCalendarPopup() const;

  QDate GetDate() const;

  QDateTime GetDateTime() const;

  QTime GetTime() const;

  void SetCalendarPopup(bool theIsEnabled);

  void SetDate(const QDate& theDate);

  void SetTime(const QTime& theTime);

  void SetDateTime(const QDateTime& theDateTime);

public slots:

  //! Update system of units.
  void onUnitsUpdated();

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
//! \todo provide comment here
class asiUI_DatumDateEdit::Editor : public QDS_Datum
{
  Q_OBJECT

public:

  Editor(const QString& theDicID,
         QWidget*       theParent = 0,
         const int      theFlags = All);

  bool IsCalendarPopup() const;

  QDate GetDate() const;

  QDateTime GetDateTime() const;

  QTime GetTime() const;

  void SetCalendarPopup(bool theIsEnabled);

  void SetDate(const QDate& theDate);

  void SetTime(const QTime& theTime);

  void SetDateTime(const QDateTime& theDateTime);

  virtual QString getString() const;

  virtual void setString(const QString& theString);

  virtual void setValue(const QVariant& theValue);

  virtual QVariant value() const;

  //! Update system of units.
  void UpdateUnits()
  {
    unitSystemChanged(QString());
  }

protected:

  QDateTimeEdit* dateTimeEdit() const;

  virtual QWidget* createControl(QWidget* theParent);

  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);
};

#endif
