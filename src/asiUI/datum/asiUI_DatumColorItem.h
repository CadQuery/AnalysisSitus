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

#ifndef asiUI_DatumColorItem_HeaderFile
#define asiUI_DatumColorItem_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>
#include <asiUI_Tools.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QColorDialog>
#include <QVariant>
#pragma warning(pop)

//! \ingroup GUI
//!
//! This is a datum for creating color item editor.
//! It provides modification of color values. The values
//! stored in data model in form of integer value.
class asiUI_EXPORT asiUI_DatumColorItem : public asiUI_Datum
{
  Q_OBJECT

  class Editor;
  class ColorItemDialog;

public:

  asiUI_DatumColorItem(const QString& theDicID, 
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
//! Color item editor for asiUI_DatumColorItem.
class asiUI_DatumColorItem::Editor : public QDS_Datum
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

  //! get value converted to string.
  //! \return value of string type.
  virtual QString getString() const
  {
    return QString::number(asiUI_Tools::ColorToInt(m_EditColor));
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
    m_EditColor = asiUI_Tools::StringToColor(theString);

    QColorDialog* aDialog = colorDialog();
    if ( aDialog )
      aDialog->setCurrentColor(m_EditColor);
  }

  //! get color value.
  //! \return qcolor value.
  virtual QVariant value() const
  {
    return asiUI_Tools::ColorToInt(m_EditColor);
  }

  QColorDialog* colorDialog() const;

private slots:

  void onColorChanged(const QColor& theColor)
  {
    m_EditColor = theColor;
    emit ColorChanged(asiUI_Tools::ColorToInt(m_EditColor));
  }

private:

  QColor m_EditColor;
};

//-----------------------------------------------------------------------------
// Editor color dialog
//-----------------------------------------------------------------------------

//! \ingroup GUI
//!
//! This is the default qt color dialog, modified to
//! to be used as color item editor for parameter editor widget.
class asiUI_DatumColorItem::ColorItemDialog : public QColorDialog
{
  Q_OBJECT

  Q_PROPERTY(QColor color READ currentColor WRITE setCurrentColor USER true)

public:

  //! Constructor.
  //! \param theParent [in] - the parent of this widget.
  ColorItemDialog(QWidget* theParent) : QColorDialog(theParent)
  {
  }

signals:

  void ColorChanged(const QColor& theColor);

protected:

  virtual void done(int theResult)
  {
    if ( theResult == QDialog::Accepted )
      emit ColorChanged(currentColor());

    QColorDialog::done(theResult);
  }
};

#endif
