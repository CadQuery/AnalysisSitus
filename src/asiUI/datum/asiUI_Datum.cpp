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
#include <asiUI_Datum.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_DatumFormat.h>

// asiAlgo includes
#include <asiAlgo_Dictionary.h>
#include <asiAlgo_DictionaryUnitSystem.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#include <QDS_LineEdit.h>
#include <QDS_CheckBox.h>
#include <QDS_SpinBox.h>
#include <QDS_SpinBoxDbl.h>
#pragma warning(pop)

// DDS includes
#pragma warning(push, 0)
#include <DDS_DicItem.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QValidator>
#include <QVariant>
#include <QLayout>
#include <QPainter>
#include <QStyleOptionFrameV2>
#pragma warning(pop)

//! Constructor.
//! The appropriate datums can be created by a widget factory.
//!
//! However, if the datum is not instantiated by the widget factory, then
//! its might be necessary to refresh its content when units changed by
//! calling OnUnitsSystemChanged(...) slot.
//!
//! \param theParent [in] parent object for the datum.
asiUI_Datum::asiUI_Datum(QWidget* theParent) 
: QObject(theParent),
  m_bBlocked(false)
{
  //
}

//! Get the datum ID.
//! \return datum dictionary identifier.
QString asiUI_Datum::GetDictionaryID() const
{
  return getDatum()->id();
}

//! Get value type of data dictionary item.
//! This type is used to select appropriate editor
//! widget and to perform correct data conversion.
int asiUI_Datum::GetDataType() const
{
  Handle(asiAlgo_DictionaryItem) aDicItem =
    asiAlgo_Dictionary::GetDictionaryItem(QStr2AsciiStr(GetDictionaryID()));

  return ( !aDicItem.IsNull() ) ? aDicItem->GetDataType() :
    asiAlgo_DictionaryItem::Unknown;
}

//! Get the datum label text.
//! \return label text.
QString asiUI_Datum::GetLabel() const
{
  return getDatum()->label();
}

//! Get the datum units text.
//! \return units text.
QString asiUI_Datum::GetUnits() const
{
  return getDatum()->units();
}

//! Get displayable units string for specified data dictionary item.
//! \param theDicID [in] data dictionary item ID.
//! \return units string.
QString asiUI_Datum::GetUnits(const QString& theDicID)
{
  return QDS_Datum::units(theDicID);
}

//! Get the datum's filter string.
//! \return filter string. 
QString asiUI_Datum::GetFilter() const
{
  return getDatum()->filter();
}

//! Get the datum format string.
//! \return format string.
QString asiUI_Datum::GetFormat() const
{
  return getDatum()->format();
}

//! Get the datum's description.
//! \return datum's description.
QString asiUI_Datum::GetDescription() const
{
  return getDatum()->shortDescription();
}

//! Get the datum default value.
//! \return default value.
QString asiUI_Datum::GetDefaultValue() const
{
  return getDatum()->defaultValue();
}

//! Get the datum minimum value.
//! \return minimum value.
QString asiUI_Datum::GetMinimumValue() const
{
  return getDatum()->minimumValue();
}

//! Get the datum maximum value.
//! \return maximum value.
QString asiUI_Datum::GetMaximumValue() const
{
  return getDatum()->maximumValue();
}

//! Get the datum value as QVariant.
//! \return datum value.
QVariant asiUI_Datum::GetValue() const
{
  return getDatum()->value();
}

//! Get the datum value as string.
//! \return datum value converted to string.
QString asiUI_Datum::GetString() const
{
  return getDatum()->stringValue();
}

//! Get string displayed in editor's field.
//! \param theLongFmt [in] if True return long formatted string if supported.
//! \return displayed string.
QString asiUI_Datum::GetDisplayedString(const bool theLongFmt) const
{
  DatumImplInterface* aFmwkImpl = dynamic_cast<DatumImplInterface*>( getDatum() );

  return (aFmwkImpl != nullptr)
    ? aFmwkImpl->GetFormatted(theLongFmt)
    : getDatum()->getString();
}

//! Get the datum value as double.
//! The value is converted according to the measure units in the active units system
//! to measure units in "SI" units system.
//! \return datum value converted to double.
double asiUI_Datum::GetDouble() const
{
  return getDatum()->doubleValue();
}

//! Get the datum value as integer.
//! The value is converted according to the measure units in the active units system
//! to measure units in "SI" units system.
//! \return datum value converted to integer.
int asiUI_Datum::GetInteger() const
{
  return getDatum()->integerValue();
}

//! Check if the datum is empty.
//! \return false if datum control has value entered.
bool asiUI_Datum::IsEmpty() const
{
  return getDatum()->isEmpty();
}

//! Reset datum state and set default value as current.
void asiUI_Datum::Reset()
{
  getDatum()->reset();
}

//! Clear the control.
void asiUI_Datum::Clear()
{
  getDatum()->clear();
}

//! Set datum value from QVariant object.
//! \param theValue [in] new value.
void asiUI_Datum::SetValue(const QVariant& theValue)
{
  getDatum()->setValue(theValue);
}

//! Set datum value from string data.
//! \param theValue [in] value to be set.
void asiUI_Datum::SetString(const QString& theValue)
{
  getDatum()->setStringValue(theValue);
}

//! Set datum value from double data.
//! The value is converted from measure units in "SI" units system
//! to the measure units in the local units system.
//! Format the value using datum format internal if it is required.
//! \param theValue [in] value to be set.
void asiUI_Datum::SetDouble(const double theValue)
{
  getDatum()->setDoubleValue(theValue);
}

//! Set datum value from integer data.
//! The value is converted from measure units in "SI" units system
//! to the measure units in the local units system.
//! Format the value using datum format if it is required.
//! \param theValue [in] value to be set.
void asiUI_Datum::SetInteger(const int theValue)
{
  getDatum()->setIntegerValue(theValue);
}

//! Get 'enabled' state of the specified subwidget.
//! Possible values for theWidget: Label, Control, Units
//! \param theWidget [in] subwidget.
//! \return 'enabled' state.
bool asiUI_Datum::IsEnabled(const int theWidget) const
{
  return getDatum()->isEnabled(convertWidgets(theWidget));
}

//! Enable/disable subwidgets specified by theWidgets.
//! Possible values of theWidgets: Label, Control, Units
//! or their bitwise summed value.
//! \param theIsEnabled [in] 'enabled' state.
//! \param theWidgets [in] subwidgets flags.
void asiUI_Datum::SetEnabled(const bool theIsEnabled,
                              const int theWidgets)
{
  getDatum()->setEnabled(theIsEnabled, convertWidgets(theWidgets));
}

//! Show subwidgets specified by theWidgets.
//! Possible values of theWidgets: Label, Control, Units
//! or their bitwise summed value.
//! \param theWidgets [in] subwidgets flags.
void asiUI_Datum::Show(const int theWidgets)
{
  getDatum()->show(convertWidgets(theWidgets));
}

//! Hide subwidgets specified by theWidgets.
//! Possible values of theWidgets: Label, Control, Units
//! or their bitwise summed value.
//! \param theWidgets [in] subwidgets flags.
void asiUI_Datum::Hide(const int theWidgets)
{
  getDatum()->hide(convertWidgets(theWidgets));
}

//! Hide subwidgets specified by theWidgets flags.
//! Possible values of theWidgets: Label, Control, Units
//! or their bitwise summed value.
//! \param theIsShown [in] show if true, hide if false.
//! \param theWidgets [in] subwidgets flags.
void asiUI_Datum::SetShown(const bool theIsShown, 
                            const int  theWidgets)
{
  getDatum()->setShown(theIsShown, convertWidgets(theWidgets));
}

//! Get subwidget specified by element.
//! \param theWidgets [in] subwidget elements flags.
//!        (Label | Control | Unit)
//! \return widget
QWidget* asiUI_Datum::GetWidget(const int theWidgets) const
{
  return getDatum()->widget(convertWidgets(theWidgets));
}

//! Set the input focus to the control widget.
void asiUI_Datum::SetFocus()
{
  getDatum()->setFocus();
}

//! Check if input data is valid.
//! If data is invalid and theShowMessage is true, the warning message box is shown.
//! \param theShowMessage [in] if true, show warning message box if input is invalid.
//! \param theExtMsg [in] warning message.
//! \param theExtLabel [in] optional name of the variable (if Label subwidget is not used).
//! \return true if input data is valid.
bool asiUI_Datum::IsValid(const bool theShowMessage,
                           const QString& theExtMsg,
                           const QString& theExtLabel) const
{
  return getDatum()->isValid(theShowMessage, theExtMsg, theExtLabel);
}

//! Add widgets to the vertical box layout.
//! \param theLayout [in] layout to insert the widgets.
void asiUI_Datum::AddTo(QVBoxLayout* theLayout)
{
  getDatum()->addTo(theLayout);
}

//! Add widgets to the horizontal box layout.
//! \param theLayout [in] layout to insert the widgets.
void asiUI_Datum::AddTo(QHBoxLayout* theLayout)
{
  getDatum()->addTo(theLayout);
}

//! Add widgets to the grid layout.
//! \param theLayout [in] layout to insert the widgets.
//! \param theRow [in] layout row index.
//! \param theCol [in] layout column index.
//! \param theIsVertical [in] if true subwidgets are layouted vertically,
//!        otherwise horizontally.
void asiUI_Datum::AddTo(QGridLayout* theLayout,
                         const int theRow,
                         const int theCol,
                         const bool theIsVertical)
{
  getDatum()->addTo(theLayout, theRow, theCol, theIsVertical);
}

//! Remove datum from layout.
//! \param theLayout [in] the layout.
void asiUI_Datum::RemoveFrom(QLayout* theLayout)
{
  QDS_Datum* aDatum = getDatum();

  QWidget* aLabWidget = aDatum->widget(QDS::Label);
  if ( aLabWidget != 0 )
  {
    ASSERT_RAISE(theLayout->indexOf(aLabWidget) != -1,
      "Layout doesn't contain datum's controls");

    theLayout->removeWidget(aLabWidget);
    aLabWidget->setParent((QWidget*)aDatum->parent());
  }

  QWidget* aCntrWidget = aDatum->widget(QDS::Control);
  if ( aCntrWidget != 0 )
  {
    ASSERT_RAISE(theLayout->indexOf(aCntrWidget) != -1,
      "Layout doesn't contain datum's controls");

    theLayout->removeWidget(aCntrWidget);
    aCntrWidget->setParent((QWidget*)aDatum->parent());
  }

  QWidget* aUnitWidget = aDatum->widget(QDS::Units);
  if ( aUnitWidget != 0 )
  {
    ASSERT_RAISE(theLayout->indexOf(aUnitWidget) != -1,
      "Layout doesn't contain datum's controls");

    theLayout->removeWidget(aUnitWidget);
    aUnitWidget->setParent((QWidget*)aDatum->parent());
  }
}

//! Set alignment for label and/or units subwidgets.
//! \param theAlign [in] the alignment type (Qt::Alignment)
//! \param theSubWidgets [in] the subwidget mask (Label and/or Units)
void asiUI_Datum::SetAlignment(const int theAlign, const int theSubWidgets)
{
  getDatum()->setAlignment(theAlign, convertWidgets(theSubWidgets));
}

//! The slot is called when the datum's contents should
//! be refreshed after the unit system changed.
void asiUI_Datum::OnUnitSystemChanged(const asiAlgo_DictionaryUnitSystem& thePrevious,
                                       const asiAlgo_DictionaryUnitSystem& theNew)
{
  // TODO: think on how to improve unit change handling
  bool isBlocked = setChangedBlocked(true);
  asiAlgo_Dictionary::SetLocalUnitSystem(thePrevious);
  QVariant aValue = GetValue();
  
  asiAlgo_Dictionary::SetLocalUnitSystem(theNew);
  onUnitsUpdated();
  SetValue(aValue);
  setChangedBlocked(isBlocked);
}

//! This operation returns editor subwidget.
//! \return editor widget.
asiUI_Datum::operator QWidget*() const
{
  return getDatum()->widget(QDS::Control);
}

//! Initialize self, connect value changed signals 
//! to re-emit them when value changes and send deletion signal for
//! QDS_Datum entity when the asiUI_Datum instance is destroyed.
void asiUI_Datum::init(QDS_Datum* theDatum)
{
  connect(this, SIGNAL(destroyed(QObject*)), theDatum, SLOT(deleteLater()));
  QWidget* aLabWidget = theDatum->widget(QDS::Label);
  if ( aLabWidget )
    connect(this, SIGNAL(destroyed(QObject*)), aLabWidget, SLOT(deleteLater()));

  QWidget* aCntrWidget = theDatum->widget(QDS::Control);
  if ( aCntrWidget )
    connect(this, SIGNAL(destroyed(QObject*)), aCntrWidget, SLOT(deleteLater()));

  QWidget* aUnitWidget = theDatum->widget(QDS::Units);
  if ( aUnitWidget )
    connect(this, SIGNAL(destroyed(QObject*)), aUnitWidget, SLOT(deleteLater()));

  connect(theDatum, SIGNAL(paramChanged()), this, SLOT(onValueChanged()));
  connect(theDatum, SIGNAL(paramChanged(QString&)), this, SLOT(onValueChanged(QString&)));
}

//! Convert asiUI_Datum flags to QDS_Datum flags.
//! \param theDatumFlags [in] flags to be converted.
//! \return converted flags for QDS_Datum
int asiUI_Datum::convertFlags(const int theDatumFlags) const
{
  // convert flags to QDS definition.
  int aFlags = convertWidgets(theDatumFlags);
  if ( theDatumFlags & NotFormat )
    aFlags |= QDS::NotFormat;
  if ( theDatumFlags & NotAccel )
    aFlags |= QDS::NotAccel;
  if ( theDatumFlags & NotConvert )
    aFlags |= QDS::NotConvert;
  if ( theDatumFlags & UnitsWithLabel )
    aFlags |= QDS::UnitsWithLabel;

  return aFlags;
}

//! Convert asiUI_Datum widget flags to QDS_Datum widget flags.
//! \param theDatumFlags [in] flags to be converted.
//! \return converted flags for QDS_Datum
int asiUI_Datum::convertWidgets(const int theWidgets) const
{
  int aFlags = 0;
  if ( theWidgets & Label )
    aFlags |= QDS::Label;
  if ( theWidgets & Control )
    aFlags |= QDS::Control;
  if ( theWidgets & Units )
    aFlags |= QDS::Units;

  return aFlags;
}

//! Hilight edited string with the specified color.
//! This method should be redefined in order to
//! provide automatic text highlight on validation.
//! Default implementation suit only for classes that extends
//! from asiUI_Datum::DatumImplInterface.
//! \param theColor [in] the color to hilight.
void asiUI_Datum::colorizeText(const QColor& theColor)
{
  DatumImplInterface* anIntf = dynamic_cast<DatumImplInterface*>( getDatum() );
  if ( anIntf )
    anIntf->ColorizeText(theColor);
}

//! Block "ValueChanged" signals.
//! \param theIsBlocked [in] blocking flag.
bool asiUI_Datum::setChangedBlocked(const bool theIsBlocked)
{
  bool anOld = m_bBlocked;
  m_bBlocked = theIsBlocked;
  return anOld;
}

//! Check whether the "ValueChanged" signals
//! are blocked.
//! \return Boolean blocking flag.
bool asiUI_Datum::IsChangedBlocked() const
{
  return m_bBlocked;
}

//! Process unit change callback. Default implementation suits only if
//! datum extends asiUI_Datum::DatumImplInterface.
void asiUI_Datum::onUnitsUpdated()
{
  DatumImplInterface* anIntf = dynamic_cast<DatumImplInterface*>( getDatum() );
  if ( anIntf )
    anIntf->UpdateUnits();
}

//! Re-emit signal when editor value is changed.
//! Provides value fixup and color validation callback.
void asiUI_Datum::onValueChanged()
{
  if ( !IsChangedBlocked() )
    emit ValueChanged();

  colorizeText( IsValid(false) ? Qt::black : Qt::red );
}

//! Re-emit signal when editor value is changed.
//! Provides value fixup and color validation callback.
void asiUI_Datum::onValueChanged(QString& theValue)
{
  if ( !IsChangedBlocked() )
    emit ValueChanged(theValue);

  colorizeText( IsValid(false) ? Qt::black : Qt::red );
}

// ============================================================================
//                 Base class layer for extending custom editors
// ============================================================================

//! Constructor.
//! Creates asiUI specific formatter instance.
//! \param theDictId [in] identifier of dictionary item being used by datum editor.
//! \param theParent [in] the application object that takes ownership of the datum.
//! \param theLongFmt [in] the flag indicating whether the datum should truncate
//! or keep decimal digits if format specification states so. This option is used to
//! support double formatting "display-editing" approach, internally the value is
//! formatted without truncating decimals (edit mode), but editor on displaying paints
//! the truncated value (display mode) if the editor has no focus or selection.
//! \param theFlags [in] the qds datum specific flags.
template <typename base_t>
asiUI_Datum::DatumImpl<base_t>::DatumImpl(const QString& theDictId,
                                           QWidget* theParent,
                                           const int theFlags,
                                           const bool theLongFmt)
: base_t(theDictId, theParent, theFlags),
  m_bLongFmt(theLongFmt)
{
  Handle(asiAlgo_DictionaryItem) aDictItem =
    asiAlgo_Dictionary::GetDictionaryItem( QStr2AsciiStr(theDictId) );

  QString aFmt = !aDictItem.IsNull() 
    ? AsciiStr2QStr( aDictItem->GetFormat(false) )
    : QString();

  m_Fmt = asiUI_DatumFormat::Make( aFmt );
}

//! Process colorize text callback.
//! Changes palette for control widgets.
//! Override if control widgets contains components.
template <typename base_t>
void asiUI_Datum::DatumImpl<base_t>::ColorizeText(const QColor& theColor)
{
  QWidget* aControl = base_t::controlWidget();
  if ( !aControl )
    return;

  QPalette aColorizedPalette = aControl->palette();
  aColorizedPalette.setColor(QPalette::Active, QPalette::Text, theColor);
  aControl->setPalette(aColorizedPalette);
}

//! Get display value formatted according to long or short form.
//! \param theLongFmt [in] the long or short form to be used?
//! \return formatted value string.
template <typename base_t>
QString asiUI_Datum::DatumImpl<base_t>::GetFormatted(const bool theLongFmt)
{
  QString aValue = base_t::getString();
  if ( m_Fmt && !(base_t::flags() & NotFormat) )
  {
    switch ( base_t::type() )
    {
      case DDS_DicItem::Integer: return m_Fmt->Format(aValue.toInt(),    theLongFmt);
      case DDS_DicItem::Float:   return m_Fmt->Format(aValue.toDouble(), theLongFmt);
      case DDS_DicItem::String:  return m_Fmt->Format(aValue,            theLongFmt);
      default:
        return aValue;
    }
  }

  return aValue;
}

//! Override suit basics validator with generic ones.
//! \param theLimits [in] boolean flag indicating whether the minimum and maximum 
//! limits should be controlled by validator or not.
//! \return validator instance.
template <typename base_t>
QValidator* asiUI_Datum::DatumImpl<base_t>::validator(const bool theLimits) const
{
  if ( base_t::type() == DDS_DicItem::Float )
  {
    asiUI_Datum::DoubleValidator* aDoubleVal =
      new asiUI_Datum::DoubleValidator( base_t::filter(), false, (QObject*)this );

    bool isOk;
    double aLimit;

    aLimit = base_t::minValue().toDouble(&isOk);
    if (isOk && theLimits)
      aDoubleVal->setBottom(aLimit);

    aLimit = base_t::maxValue().toDouble(&isOk);
    if (isOk && theLimits)
      aDoubleVal->setTop(aLimit);

    Handle(DDS_DicItem) anItem = base_t::dicItem();
    if ( !anItem.IsNull() ) 
    {
      aDoubleVal->SetFixupFormat( m_Fmt, isLongFmt() );
    }

    return aDoubleVal;
  }

  return base_t::validator(theLimits);
}

//! validate that the input string, at least, is acceptable.
//! \param theString [in] the string to validate.
template <typename base_t>
bool asiUI_Datum::DatumImpl<base_t>::validate(const QString& theString) const
{
  QValidator* aValidator = validator( true );
  if ( !aValidator )
  {
    return base_t::validate(theString);
  }

  int aPos = 0;
  QString aStr = theString;

  bool isOk = aValidator->validate(aStr, aPos) != QValidator::Invalid;
  aValidator->deleteLater();
  return isOk;
}

//! Format integer value according to datum formatting rules.
//! \param theValue [in] the value being formatted.
//! \return formatted string to be sent to editor.
template <typename base_t>
QString asiUI_Datum::DatumImpl<base_t>::formatValue(const int theValue) const
{
  if ( m_Fmt == nullptr || base_t::flags() & QDS_Datum::NotFormat )
  {
    switch ( base_t::type() )
    {
      case DDS_DicItem::Float:
        return QString::number( (double) theValue, 'g', 16 );

      default:
        return QString::number(theValue);
    }
  }

  switch ( base_t::type() )
  {
    case DDS_DicItem::Integer:
    case DDS_DicItem::Float:
    case DDS_DicItem::String:
      return m_Fmt->Format( (double) theValue, isLongFmt() );

    default:
      return QString::number(theValue);
  }
}

//! Format double value according to datum formatting rules.
//! \param theValue [in] the value being formatted.
//! \return formatted string to be sent to editor.
template <typename base_t>
QString asiUI_Datum::DatumImpl<base_t>::formatValue(const double theValue) const
{
  if ( m_Fmt == nullptr || base_t::flags() & QDS_Datum::NotFormat )
  {
    switch ( base_t::type() )
    {
      case DDS_DicItem::List:
      case DDS_DicItem::Integer:
        return QString::number( (int) theValue );

      default:
        return QString::number(theValue, 'g', 16);
    }
  }

  switch ( base_t::type() )
  {
    case DDS_DicItem::Float:
    case DDS_DicItem::Integer:
    case DDS_DicItem::String:
      return m_Fmt->Format( theValue, isLongFmt() );

    case DDS_DicItem::List:
      return QString::number( (int) theValue );

    default:
      return QString::number(theValue, 'g', 16);
  }
}

//! Format string value according to datum formatting rules.
//! \param theValue [in] the value being formatted.
//! \return formatted string to be sent to editor.
template <typename base_t>
QString asiUI_Datum::DatumImpl<base_t>::formatValue(const QString& theValue) const
{
  if ( m_Fmt == nullptr || base_t::flags() & QDS_Datum::NotFormat )
  {
    return theValue;
  }

  int aType = base_t::type();
  switch (aType)
  {
    case DDS_DicItem::Float:
    case DDS_DicItem::Integer:
    {
      bool isOk = false;
      double aValue = theValue.toDouble(&isOk);
      if (!isOk) 
        return "";

      bool isDouble = (aType == DDS_DicItem::Float);
      if (m_Fmt != nullptr) 
        return m_Fmt->Format( aValue, isLongFmt() );

      return isDouble
        ? QString::number(aValue, 'g', 16)
        : QString::number((int)aValue);
    }

    case DDS_DicItem::List:
    {
      bool isOk = false;
      int aValue = theValue.toInt(&isOk);
      return isOk ? QString::number(aValue) : "";
    }

    default:
      return theValue;
  }
}

// ============================================================================
//                 Generic extensions
// ============================================================================

//! Default constructor for generic double validator class.
//! \param theFilter [in] additional filter string.
//! \param theCheckFormatting [in] boolean flag notifying whether the validator
//! needs to provide intermediate state if formatting is insufficient.
//! required for calling "fixup" in spin box editors to fix formatting.
//! \param theParent [in] the parent object that owns the validator.
asiUI_Datum::DoubleValidator::DoubleValidator(const QString& theFilter,
                                               const bool theCheckFormatting,
                                               QObject* theParent)
: QDS_DoubleValidator(theFilter, theParent),
  m_bLongFmt(false),
  m_bCheckFormatting(theCheckFormatting)
{
  m_fixupCache.StrBefore = QString();
  m_fixupCache.StrFixup  = QString();
}

//! Validate the specified string.
//! \param theString [in] the string to validate.
//! \param thePos [in] the position of recent changes.
//! \return status or validation result.
QValidator::State asiUI_Datum::DoubleValidator::validate(QString& theInput, int& thePos) const
{
  State aFormatState = Acceptable;
  State aValueState = QDS_DoubleValidator::validate(theInput, thePos);
  if ( aValueState != Invalid )
  {
    // force check for limits
    bool isConverted = false;
    double aValue = theInput.toDouble(&isConverted);
    if ( !isConverted )
    {
      // check exponent limits
      QChar aLowerExp = QLocale::c().exponential().toLower();
      QChar aUpperExp = aLowerExp.toUpper();
      QString aExpReg = QString("[%1%2][+-]{0,1}([0-9]*)$").arg(aLowerExp).arg(aUpperExp);
      QRegExp aExp(aExpReg);
      aExp.indexIn(theInput);
      int anExponent = aExp.cap(1).toInt();
      if (anExponent < DBL_MIN_10_EXP || anExponent > DBL_MAX_10_EXP)
      {
        aValueState = Invalid;
      }
    }
    else if ( aValue < bottom() || aValue > top() )
    {
      aValueState = Invalid;
    }
  }

  if ( m_bCheckFormatting && aValueState != Invalid )
  {
    aFormatState = validateFormat(theInput);
  }
  
  return qMin(aValueState, aFormatState);
}

//! Validate double value precision.
//! If the precision is invalid, the intermediate status is returned to invoke fixup.
//! \param theInput [in] the input string.
//! \return validation status string.
QValidator::State asiUI_Datum::DoubleValidator::validateFormat(const QString& theInput) const
{
  QString aDesiredStr = theInput;
  this->fixup(aDesiredStr);
  return (theInput != aDesiredStr) ? Intermediate : Acceptable;
}

//! Set fixup precision in decimals.
//! \param theFmt [in] the value format.
//! \param theLongFmt [in] short or long formatting used.
void asiUI_Datum::DoubleValidator::SetFixupFormat(const asiUI_DatumFormatPtr& theFmt, const bool theLongFmt)
{
  m_bLongFmt = theLongFmt;
  m_fixupFmt = theFmt;
  m_fixupCache.StrBefore = QString();
  m_fixupCache.StrFixup  = QString();
}

//! Produce value fixing up.
//! \param theInput [in/out] the value to fix it up.
void asiUI_Datum::DoubleValidator::fixup(QString& theInput) const
{
  if ( !m_fixupCache.StrBefore.isNull() && m_fixupCache.StrBefore == theInput )
  {
    theInput = m_fixupCache.StrFixup;
    return;
  }

  bool isConverted = false;
  double aDoubleValue = theInput.toDouble(&isConverted);
  if ( !isConverted )
    return;

  m_fixupCache.StrBefore = theInput;
  m_fixupCache.StrFixup  = (m_fixupFmt != nullptr)
    ? m_fixupFmt->Format(aDoubleValue, m_bLongFmt)
    : theInput;

  theInput = m_fixupCache.StrFixup;
}

//! Construct exact reg exp validator for expression.
//! \param theExp [in] the expression.
//! \param theParent [in] the parent object.
asiUI_Datum::ExactRegExpValidator::ExactRegExpValidator(const QRegExp& theExp,
                                                         QObject* theParent)
: QRegExpValidator(theExp, theParent)
{
  //
}

//! Provide strict validation of input string.
//! \param theInput [in] the string to check.
//! \param thePos [in] the cursor position.
QValidator::State asiUI_Datum::ExactRegExpValidator::validate(QString& theInput,
                                                               int& thePos) const
{
  return QRegExpValidator::validate(theInput, thePos) == Acceptable ?
    Acceptable : Invalid;
}

// ============================================================================
//                 Formatted line editor
// ============================================================================

//! Constructor.
//! \param theFmt [in] the short string formatter.
//! \param theType [in] the datum value type.
//! \param theParent [in] the parent of this object.
asiUI_Datum::FormattedLineEdit::FormattedLineEdit(const asiUI_DatumFormatPtr& theFmt,
                                                   const int theType,
                                                   QWidget* theParent)
: QDS_LineEdit::Editor(theParent),
  m_fmt(theFmt),
  m_type(theType) {}

//! Override painting to show short formatted text string
//! if editor has no selection and edit focus.
//! \param theEvent [in] the paint event to be processed.
void asiUI_Datum::FormattedLineEdit::paintEvent(QPaintEvent* theEvent)
{
  if (m_fmt == nullptr || hasSelectedText() || hasFocus())
  {
    return QDS_LineEdit::Editor::paintEvent(theEvent);
  }

  QString aCurrValue = text();
  if ( m_prvString != aCurrValue )
  {
    m_prvString = aCurrValue;
    switch (m_type)
    {
      case DDS_DicItem::Integer: m_shortString = m_fmt->Format(aCurrValue.toInt(), false); break;
      case DDS_DicItem::Float:   m_shortString = m_fmt->Format(aCurrValue.toDouble(), false); break;
      default:
        m_shortString = aCurrValue;
        break;
    }
  }

  // replace text and paint. this approach suits for painting overriden value
  // only in non-focused mode. otherwise, it can screw up undo-redo history.
  bool isBlocked = this->blockSignals(true);
  this->setText(m_shortString);
  QDS_LineEdit::Editor::paintEvent(theEvent);
  this->setText(aCurrValue);
  blockSignals(isBlocked);
}

// ============================================================================
//                 Common template declaration
// ============================================================================

template class asiUI_Datum::DatumImpl<QDS_Datum>;
template class asiUI_Datum::DatumImpl<QDS_LineEdit>;
template class asiUI_Datum::DatumImpl<QDS_CheckBox>;
template class asiUI_Datum::DatumImpl<QDS_SpinBox>;
template class asiUI_Datum::DatumImpl<QDS_SpinBoxDbl>;
