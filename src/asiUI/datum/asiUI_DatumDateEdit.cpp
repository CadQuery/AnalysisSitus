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
#include <asiUI_DatumDateEdit.h>

// asiUI includes
#include <asiUI_Common.h>

// Qt includes
#pragma warning(push, 0)
#include <QDateTimeEdit>
#include <QVariant>
#include <QEvent>
#pragma warning(pop)

//! Constructor.
//! \param theDicID [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theDatumFlags [in] datum subwidget and option flags.
asiUI_DatumDateEdit::asiUI_DatumDateEdit(const QString& theDicID, 
                                           QWidget* theParent,
                                           const int theDatumFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDicID, theParent, convertFlags(theDatumFlags));

  init(m_pEditor);
}

QDS_Datum* asiUI_DatumDateEdit::getDatum() const
{
  return m_pEditor;
}

bool asiUI_DatumDateEdit::IsCalendarPopup() const
{
  return m_pEditor->IsCalendarPopup();
}

QDate asiUI_DatumDateEdit::GetDate() const
{
  return m_pEditor->GetDate();
}

QDateTime asiUI_DatumDateEdit::GetDateTime() const
{
  return m_pEditor->GetDateTime();
}

QTime asiUI_DatumDateEdit::GetTime() const
{
  return m_pEditor->GetTime();
}

void asiUI_DatumDateEdit::SetCalendarPopup(bool theIsEnabled)
{
  m_pEditor->SetCalendarPopup(theIsEnabled);
}

void asiUI_DatumDateEdit::SetDate(const QDate& theDate)
{
  m_pEditor->SetDate(theDate);
}

void asiUI_DatumDateEdit::SetTime(const QTime& theTime)
{
  m_pEditor->SetTime(theTime);
}

void asiUI_DatumDateEdit::SetDateTime(const QDateTime& theDateTime)
{
  m_pEditor->SetDateTime(theDateTime);
}

//! Update unit system.
void asiUI_DatumDateEdit::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Constructor. Initializes controls.
asiUI_DatumDateEdit::Editor::Editor(const QString& theDicID,
                                     QWidget* theParent,
                                     const int theFlags)
: QDS_Datum(theDicID, theParent, theFlags)
{
  SetCalendarPopup(true);
}

//! Create control widget
//! \param theParent [in] parent widget for editor.
//! \return widget pointer.
QWidget* asiUI_DatumDateEdit::Editor::createControl(QWidget* theParent)
{
  QDateTimeEdit* anEditor = new QDateTimeEdit(theParent);

  // set display format
  QString aFormatStr = format();
  if ( !aFormatStr.isEmpty() )
    anEditor->setDisplayFormat(aFormatStr);

  anEditor->installEventFilter(this);

  return anEditor;
}

bool asiUI_DatumDateEdit::Editor::eventFilter(QObject* /*theObj*/,
                                               QEvent*  theEvent)
{
  if ( theEvent->type() == QEvent::KeyPress )
  {
    QDateTimeEdit* anEdit = dateTimeEdit();
    if ( anEdit && !anEdit->hasFocus() )
      anEdit->setFocus();
  }

  return false;
}

//! Get date editor control.
//! \return pointer to a date editor control.
QDateTimeEdit* asiUI_DatumDateEdit::Editor::dateTimeEdit() const
{
  return ::qobject_cast<QDateTimeEdit*>( controlWidget() );
}

//! Get calendar popup showing mode.
//! \return true if enabled or false if disabled.
bool asiUI_DatumDateEdit::Editor::IsCalendarPopup() const
{
  QDateTimeEdit* anEdit = dateTimeEdit();

  if ( !anEdit )
    return false;

  return anEdit->calendarPopup();
}

//! Get date.
//! \return date value.
QDate asiUI_DatumDateEdit::Editor::GetDate() const
{
  QDateTimeEdit* anEdit = dateTimeEdit();

  if ( !anEdit )
    return QDate();
  
  return anEdit->date();
}

//! Get date time.
//! \return date time value.
QDateTime asiUI_DatumDateEdit::Editor::GetDateTime() const
{
  QDateTimeEdit* anEdit = dateTimeEdit();

  if ( !anEdit )
    return QDateTime();
  
  return anEdit->dateTime();
}

//! Get time.
//! \return time value.
QTime asiUI_DatumDateEdit::Editor::GetTime() const
{
  QDateTimeEdit* anEdit = dateTimeEdit();

  if ( !anEdit )
    return QTime();
  
  return anEdit->time();
}

//! Set calendar popup showing mode.
//! The calendar pop-up will be shown upon clicking the arrow button.
//! This popup is valid only if there is a valid date display format.
//! \param theIsEnabled [in] enable/disable calendar popup.
void asiUI_DatumDateEdit::Editor::SetCalendarPopup(bool theIsEnabled)
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  if ( anEdit )
    anEdit->setCalendarPopup(theIsEnabled);
}

//! Set date.
//! \param theDate [in] the date to be set.
void asiUI_DatumDateEdit::Editor::SetDate(const QDate& theDate)
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  if ( anEdit )
    anEdit->setDate(theDate);
}

//! Set time.
//! \param theTime [in] the time to be set.
void asiUI_DatumDateEdit::Editor::SetTime(const QTime& theTime)
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  if ( anEdit )
    anEdit->setTime(theTime);
}

//! Set date,time value.
//! \param theDateTime [in] the date,time to be set
void asiUI_DatumDateEdit::Editor::SetDateTime(const QDateTime& theDateTime)
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  if ( anEdit )
    anEdit->setDateTime(theDateTime);
}

//! Get string value from date time editor.
//! \return string value.
QString asiUI_DatumDateEdit::Editor::getString() const
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  
  if ( !anEdit )
      return QString();

  return anEdit->dateTime().toString(anEdit->displayFormat());
}

//! Set string value to date time editor.
//! \param theString [in] the input value.
void asiUI_DatumDateEdit::Editor::setString(const QString& theString)
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  if ( anEdit )
  {
    QDateTime aDateTime = QDateTime::fromString(theString, anEdit->displayFormat());
    anEdit->setDateTime(aDateTime);
  }
}

//! Set Date and Time from qvariant value.
//! \param theValue [in] date in form of qvariant value.
void asiUI_DatumDateEdit::Editor::setValue(const QVariant& theValue)
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  if ( anEdit )
    anEdit->setDateTime(theValue.toDateTime());
}

//! Get date in time converted to qvariant value.
//! \return qvariant date and time.
QVariant asiUI_DatumDateEdit::Editor::value() const
{
  QDateTimeEdit* anEdit = dateTimeEdit();
  
  return anEdit != 0 ? QVariant(anEdit->dateTime()) : QVariant();
}
