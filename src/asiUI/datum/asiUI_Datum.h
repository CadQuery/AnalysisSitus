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

#ifndef asiUI_Datum_HeaderFile
#define asiUI_Datum_HeaderFile

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_DatumFormat.h>

// Qt includes
#pragma warning(push, 0)
#include <QColor>
#include <QMap>
#include <QSize>
#include <QRegExp>
#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QRegExpValidator>
#pragma warning(pop)

// QDS includes
#pragma warning(push, 0)
#include <QDS_Validator.h>
#include <QDS_LineEdit.h>
#pragma warning(pop)

// Forward declaration
class QDS_Datum;
class QValidator;
class QLayout;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class asiUI_DataDicItem;
class asiAlgo_DictionaryUnitSystem;

//! \ingroup GUI
//!
//! This is the base wrapper class for all datum controls.
//! Datum controls is the set of editor widgets for
//! modifying the datums described in data dictionary.
//! The base class provides access to datum's value.
//! init() method should be called in constructor to
//! correctly initialize the descendant datum class.
class asiUI_EXPORT asiUI_Datum : public QObject
{
  Q_OBJECT

public:

  template <typename SuitDatumClass>
  class DatumImpl;            //!< generic base extension class for datum editors
  class DatumImplInterface;   //!< base interface to extension class used by asiUI_Datum
  class DoubleValidator;      //!< extended double validator
  class ExactRegExpValidator; //!< regexp validator without intermediate (fixable) state
  class FormattedLineEdit;    //!< line edit with custom formatting for displayed values

public:

  enum DatumFlags
  {
    None           = 0x00, // No flags
    Label          = 0x01, // Create subwidget for datum label
    Control        = 0x02, // Create subwidget for datum input control
    Units          = 0x04, // Create subwidget for datum units of measure
    NotFormat      = 0x08, // Don't format initial value
    NotAccel       = 0x10, // Not support accelerators in datum label
    NotConvert     = 0x20, // Don't convert numeric value
    UnitsWithLabel = 0x40, // Display units of measure in label like "<label_text> (<units_text>) instead separate text"
    All = Label | Control | Units  // Create all subwidgets
  };

public:

  asiUI_Datum(QWidget* theParent = 0);

  virtual QString GetDictionaryID() const;

  virtual int GetDataType() const;

  virtual QString GetLabel() const;
  
  virtual QString GetUnits() const;

  static QString GetUnits(const QString& theDicID);

  virtual QString GetFilter() const;

  virtual QString GetFormat() const;

  virtual QString GetDescription() const;

  virtual QString GetDefaultValue() const;

  virtual QString GetMinimumValue() const;

  virtual QString GetMaximumValue() const;

  virtual QVariant GetValue() const;

  virtual QString GetString() const;

  virtual QString GetDisplayedString(const bool theLongFmt) const;

  virtual double GetDouble() const;

  virtual int GetInteger() const;

  virtual bool IsEmpty() const;

  virtual void Reset();

  virtual void Clear();

  virtual void SetValue(const QVariant& theValue);

  virtual void SetString(const QString& theValue);

  virtual void SetDouble(const double theValue);

  virtual void SetInteger(const int theValue);

  virtual bool IsEnabled(const int theWidget = Control) const;

  virtual void SetEnabled(const bool theIsEnabled,
                          const int  theWidgets = Control);

  virtual void Show(const int theWidgets = All);

  virtual void Hide(const int theWidgets = All);

  virtual void SetShown(const bool theIsEnabled, 
                        const int  theWidgetFlags = All);

  virtual QWidget* GetWidget(const int theWidgets) const;

  virtual void SetFocus();

  virtual bool IsValid(const bool     theShowMessage = true, 
                       const QString& theExtMsg   = QString(),
                       const QString& theExtLabel = QString()) const;

  virtual void AddTo(QVBoxLayout* theLayout);

  virtual void AddTo(QHBoxLayout* theLayout);

  virtual void AddTo(QGridLayout* theLayout, 
                     const int    theRow,
                     const int    theCol,
                     const bool   theIsVertical = false);

  virtual void RemoveFrom(QLayout* theLayout);

  virtual void SetAlignment(const int theAlign,
                            const int theSubWidgets);

  virtual operator QWidget*() const;

public slots:

  virtual void
    OnUnitSystemChanged(const asiAlgo_DictionaryUnitSystem& thePrevious,
                        const asiAlgo_DictionaryUnitSystem& theNew);

signals:
  
  //! Notify when value changes.
  //! This notification is sent when value is changed
  //! by the user or programmatically.
  void ValueChanged();

  //! Notify when value changes.
  //! This notification is sent when value is changed
  //! by the user or programmatically.
  void ValueChanged(const QString& theValue);

protected:

  void init(QDS_Datum* theDatum);

  int convertFlags(const int theDatumFlags) const;

  int convertWidgets(const int theWidgets) const;

  virtual QDS_Datum* getDatum() const = 0;

  virtual void colorizeText(const QColor& theColor);

protected:

  bool setChangedBlocked(const bool theIsBlocked);

  bool IsChangedBlocked() const;

  virtual void onUnitsUpdated();

private slots:

  void onValueChanged();

  void onValueChanged(QString& theValue);

private:

  bool m_bBlocked;
};

// ============================================================================
//                 Base class layer for extending custom editors
// ============================================================================

//! \ingroup GUI
//!
//! Interface used by asiUI_Datum to invoke methods of extended editors.
class asiUI_EXPORT asiUI_Datum::DatumImplInterface
{
public:

  virtual void UpdateUnits() = 0;

  virtual void ColorizeText(const QColor& theColor) = 0;

  virtual QString GetFormatted(const bool theLongFmt) = 0;
};

//! \ingroup GUI
//!
//! Generic extensional layer introduced between suit datums and asiUI
//! wrapped datum editors. This extension improves datums to suit the framework's
//! needs. Usage of templates adds some restrictions to use of the class, e.g.
//! asiUI_Datum exploits an interface to invoke its methods.
//! Inheritance from the class avoids code duplication for putting common functionality
//! over suit datums.
template <typename SuitDatumClass>
class asiUI_Datum::DatumImpl : public SuitDatumClass, public asiUI_Datum::DatumImplInterface
{
public:

  DatumImpl(const QString& theDictId,
            QWidget* theParent = 0,
            const int theFlags = All,
            const bool theLongFmt = false);

public:

  virtual void UpdateUnits() { unitSystemChanged( QString() ); }

  virtual void ColorizeText(const QColor& theColor);

  virtual QString GetFormatted(const bool theLongFmt);

  virtual QValidator* validator(const bool theLimits = false) const;

  virtual bool validate(const QString& theString) const;

protected:

  virtual QString formatValue(const int theValue) const;

  virtual QString formatValue(const double theValue) const;

  virtual QString formatValue(const QString& theValue) const;

  bool isLongFmt() const { return m_bLongFmt; }

protected:

  //! Property indicates whether decimal fractions should be truncated according to
  //! datum format or not. This flag is used for display/edit value displaying approach.
  //! Datums that supports this feature can use long formatting for edit values and
  //! use short formatting in non-editing mode (e.g. override painting for no focus).
  bool m_bLongFmt;

  //! Datum value formatter
  asiUI_DatumFormatPtr m_Fmt;
};

// ============================================================================
//                 Generic extensions
// ============================================================================

//! \ingroup GUI
//!
//! Generic double value validator.
//! Fixes up invalid values with too small precision defined.
//! The precision is the value defined in format attribute of data dictionary item.
//! If the exponential or decimal part is too small (e.g. too much decimal numbers,
//! then the value is round up to the precision from data dictionary.
class asiUI_Datum::DoubleValidator : public QDS_DoubleValidator
{
  Q_OBJECT

public:

  DoubleValidator(const QString& theFilter,
                  const bool theCheckFormatting,
                  QObject* theParent = 0);

  virtual State validate(QString& theInput, int& thePos) const;

  State validateFormat(const QString& theInput) const;

  void SetFixupFormat(const asiUI_DatumFormatPtr& theFmt, const bool theLongFmt);

  virtual void fixup(QString& theInput) const;

private:

  bool m_bLongFmt;
  bool m_bCheckFormatting;
  asiUI_DatumFormatPtr m_fixupFmt;

private:

  struct Cache
  {
    QString StrBefore;
    QString StrFixup;
  };
  
  mutable Cache m_fixupCache;
};

//! \ingroup GUI
//!
//! RegExp validator for strict validation, without
//! intermediate state.
class asiUI_Datum::ExactRegExpValidator : public QRegExpValidator
{
  Q_OBJECT

public:

  ExactRegExpValidator(const QRegExp& theExp, QObject* theParent = 0);

  virtual State validate(QString& theInput, int& thePos) const;
};

//! \ingroup GUI
//!
//! Editor providing additional short form formatting of value
//! if no focus or selection is made at the editor.
class asiUI_Datum::FormattedLineEdit : public QDS_LineEdit::Editor
{
  Q_OBJECT

public:

  FormattedLineEdit(const asiUI_DatumFormatPtr& theFmt,
                    const int theType,
                    QWidget* theParent = 0);

public:
  virtual QSize sizeHint() const { return QLineEdit::sizeHint(); }

protected:
  virtual void paintEvent(QPaintEvent* theEvent);

private:

  int                   m_type;        //!< data type
  asiUI_DatumFormatPtr m_fmt;         //!< formatter for short representation
  QString               m_prvString;   //!< previous value to be stored for reference
  QString               m_shortString; //!< short string representation to be displayed
};

#endif
