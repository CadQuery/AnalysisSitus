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

#ifndef asiUI_DatumColorEdit_HeaderFile
#define asiUI_DatumColorEdit_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>
#include <asiUI_Tools.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#pragma warning(pop)

// Qtx includes
#pragma warning(push, 0)
#include <QtxColorButton.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QVariant>
#pragma warning(pop)

//! \ingroup GUI
//!
//! This datum provides modification of color values.
//! The values stored in data model in form of integer value.
//! The editor control is the colored push button that opens
//! color dialog when clicking on it.
class asiUI_EXPORT asiUI_DatumColorEdit : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  asiUI_DatumColorEdit(const QString& theDicID, 
                        QWidget*       theParent = 0,
                        const int      theDatumFlags = All);

signals:

  //! Signal when current color value has been modified.
  void ColorChanged(const int theValue);

public slots:

  virtual void onUnitsUpdated();

protected:

  virtual QDS_Datum* getDatum() const;

private:

  friend class Editor;

  Editor* m_pEditor;
};

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! \ingroup GUI
//!
//! Color item editor for asiUI_DatumColorEdit.
class asiUI_DatumColorEdit::Editor : public QDS_Datum
{
  Q_OBJECT

public:

  Editor(const QString& theDicID,
         QWidget*       theParent = 0,
         const int      theFlags = All);

  //! Update system of units.
  //! \param theUnitSystem [in] identification name of active system of units.
  void UpdateUnits()
  {
    unitSystemChanged(QString());
  }

  //! get value converted to string.
  //! \return value of string type.
  virtual QString getString() const
  {
    QColor aColor = colorButton()->color();

    return QString::number(asiUI_Tools::ColorToInt(aColor));
  }

signals:

  //! the signal is emitted when color value changes.
  void ColorChanged(const int theColorValue);

protected:

  virtual QWidget* createControl(QWidget* theParent);

  //! set value in form of string.
  //! \param theString [in] value converted to
  //!        stirng type.
  virtual void setString(const QString& theString)
  {
    QtxColorButton* aButton = colorButton();
    if ( aButton )
      aButton->setColor(asiUI_Tools::StringToColor(theString));
  }

  QtxColorButton* colorButton() const;

private slots:

  void onColorChanged(QColor theColor)
  {
    int aValue = asiUI_Tools::ColorToInt(theColor);
    setIntegerValue(aValue);

    emit ColorChanged(aValue);
  }
};

#endif
