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
#include <asiUI_DatumSpinBoxDbl.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>

// Qt includes
#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

//! Constructor.
//! \param theDictId [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theFlags [in] datum subwidget and option flags.
asiUI_DatumSpinBoxDbl::asiUI_DatumSpinBoxDbl(const QString& theDictId, 
                                               QWidget* theParent,
                                               const int theFlags)
: asiUI_Datum(theParent)
{
  m_pDatumImpl = new SpinBoxImpl( theDictId, theParent, convertFlags(theFlags) );

  this->UserRangeModeOff(); // Use Data Dictionary range by default

  init(m_pDatumImpl);

  connect( m_pDatumImpl, SIGNAL( EditingFinished() ),
    this, SIGNAL( EditingFinished() ) );
}

//! Get the spin box increment value.
//! \return current increment value.
double asiUI_DatumSpinBoxDbl::GetStep() const
{
  return m_pDatumImpl->step();
}

//! Set the spin box increment value.
//! \param theStepSize [in] new increment value.
void asiUI_DatumSpinBoxDbl::SetStep(const double theStepSize)
{
  m_pDatumImpl->setStep(theStepSize);
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumSpinBoxDbl::getDatum() const
{
  return m_pDatumImpl;
}

//! Enables user-defined min/max range mode.
//! \param theMin [in] minimal value to use.
//! \param theMax [in] maximal value to use.
void asiUI_DatumSpinBoxDbl::UserRangeModeOn(const double theMin, const double theMax)
{
  m_pDatumImpl->UserRangeModeOn(theMin, theMax);
}

//! Disables user-defined min/max range mode.
void asiUI_DatumSpinBoxDbl::UserRangeModeOff()
{
  m_pDatumImpl->UserRangeModeOff();
}

// ============================================================================
//                         Datum spin box implementation
// ============================================================================

//! Default constructor.
//! Creates controls, connects signals, reads properties
//! from data dictionary.
//! \param theDictId [in] the dictionary item record identifier.
//! \param theParent [in] the parent widget for the datum.
//! \param theFlags [in] the widget flags.
asiUI_DatumSpinBoxDbl::SpinBoxImpl::SpinBoxImpl(const QString& theDictId,
                                                 QWidget* theParent,
                                                 const int theFlags)
: asiUI_Datum::DatumImpl<QDS_SpinBoxDbl>(theDictId, theParent, theFlags, true),
  m_bUserRangeMode(false),
  m_fMin(0.0),
  m_fMax(0.0)
{
  asiUI_DatumSpinBoxDbl::SpinBoxWidget* aSpinBox = 
    qobject_cast<asiUI_DatumSpinBoxDbl::SpinBoxWidget*>( spinBox() );

  if ( !aSpinBox )
    return;

  aSpinBox->setMinimum(-1.0e14);
  aSpinBox->setMaximum(1.0e14);

  aSpinBox->SetValidator( validator() );

  connect( aSpinBox, SIGNAL( editingFinished() ), this, SIGNAL( EditingFinished() ) );

  Handle(asiAlgo_DictionaryItem) aDictItem =
    Handle(asiAlgo_DictionaryItem)::DownCast( dicItem() );

  if ( aDictItem.IsNull() )
    return;

  const asiUI_WidgetParameterMap& aParamMap = aDictItem->GetWidgetParams();

  TCollection_AsciiString aStepOption("step");
  if ( !aParamMap.IsBound(aStepOption) )
    return;
  
  TCollection_AsciiString aValue = aParamMap.Find(aStepOption);
  if ( aValue.IsRealValue() )
  {
    setStep( aValue.RealValue() );
  }
}

//! Create redefined spin box control that enables data validation
//! with validator.
//! \param theParent [in] the parent widget for the spin box.
QWidget* asiUI_DatumSpinBoxDbl::SpinBoxImpl::createControl(QWidget* theParent)
{
  asiUI_DatumSpinBoxDbl::SpinBoxWidget* aSpinBox =
    new asiUI_DatumSpinBoxDbl::SpinBoxWidget(theParent);
  QLineEdit* anEdit =
    new asiUI_Datum::FormattedLineEdit( m_Fmt, type(), aSpinBox );

  aSpinBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  aSpinBox->SetLineEdit(anEdit);

  connect( aSpinBox, SIGNAL( valueChanged(const QString&) ), this, SLOT( onChanged() ) );
  connect( anEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( onChanged() ) );
  connect( anEdit, SIGNAL( textChanged(const QString&) ),
         aSpinBox, SLOT( onTextChanged(const QString&) ) );
  return aSpinBox;
}

//! Update values when unit system changes.
//! \param theSystemId [in] unit system identifier (deprecated).
void asiUI_DatumSpinBoxDbl::SpinBoxImpl::unitSystemChanged(const QString& theSystemId)
{
  QDS_Datum::unitSystemChanged( theSystemId ); // avoid default spin box implementation

  asiUI_DatumSpinBoxDbl::SpinBoxWidget* aSpinBox =
    qobject_cast<asiUI_DatumSpinBoxDbl::SpinBoxWidget*>( spinBox() );
  if ( !aSpinBox )
    return;

  if ( aSpinBox->GetValidator() )
  {
    aSpinBox->GetValidator()->deleteLater();
  }

  aSpinBox->SetValidator( new asiUI_Datum::DoubleValidator( filter(), true, (QObject*)this ) );
  aSpinBox->setPrecision(-ASI_DATUM_DOUBLE_PRECISION);
  aSpinBox->setDecimals(DBL_MAX_10_EXP);
  aSpinBox->setMinimum(-1.0e14);
  aSpinBox->setMaximum( 1.0e14);
}

//! Callback invoked when edit text changes.
//! \param theValue [in] the value.
void asiUI_DatumSpinBoxDbl::SpinBoxImpl::onChanged()
{
  onParamChanged();
  QString aText = getString();

  emit paramChanged();
  emit paramChanged(aText);
}

// ============================================================================
//                         Generic spin box widget
// ============================================================================

//! Default constructor.
asiUI_DatumSpinBoxDbl::SpinBoxWidget::SpinBoxWidget(QWidget* theParent)
: QtxDoubleSpinBox(theParent),
  m_pValidator(NULL)
{
  setPrecision(-ASI_DATUM_DOUBLE_PRECISION);
  setDecimals(DBL_MAX_10_EXP);
  setMinimum(-1.0e14);
  setMaximum( 1.0e14);

  connect( lineEdit(), SIGNAL( textChanged(const QString&) ),
    this, SIGNAL( valueChanged(const QString&) ) );
}

//! Fixup input string if result is intermediate.
//! \param theInput [in] the input string.
void asiUI_DatumSpinBoxDbl::SpinBoxWidget::fixup(QString& theInput) const
{
  if ( m_pValidator != NULL )
  {
    m_pValidator->fixup(theInput);
    return;
  }

  QDoubleSpinBox::fixup(theInput);
  return;
}

//! Validate input string.
//! \param theInput [in] the input string.
//! \param thePos [in] the position of recent changes.
//! \return validation status.
QValidator::State asiUI_DatumSpinBoxDbl::SpinBoxWidget::validate(QString& theInput, int& thePos) const
{
  if ( m_pValidator != NULL )
  {
    return m_pValidator->validate(theInput, thePos);
  }

  return QDoubleSpinBox::validate(theInput, thePos);
}

//! Set new validator for the control.
//! \param theValidator [in] the validator.
void asiUI_DatumSpinBoxDbl::SpinBoxWidget::SetValidator(QValidator* theValidator)
{
  m_pValidator = theValidator;
}

//! \return validator used by the control.
QValidator* asiUI_DatumSpinBoxDbl::SpinBoxWidget::GetValidator()
{
  return m_pValidator;
}

//! Convert value to spinbox taking using validator's fixup method.
//! \param theValue [in] the value to convert.
QString asiUI_DatumSpinBoxDbl::SpinBoxWidget::textFromValue(double theValue) const
{
  QString aText = QDoubleSpinBox::textFromValue(theValue);

  if ( m_pValidator != NULL )
  {
    m_pValidator->fixup(aText);
  }

  return aText;
}

//! Take care of value precision near zero point.
//! \param theCount [in] the number of steps to be done.
void asiUI_DatumSpinBoxDbl::SpinBoxWidget::stepBy(int theCount)
{
  setCleared(false);

  QDoubleSpinBox::stepBy(theCount);

  double aCurrValue = value();
  if ( qAbs(aCurrValue) < DBL_EPSILON ) 
  {
    aCurrValue = 0.0;
  }

  if ( aCurrValue < minimum() )
  {
    aCurrValue = minimum();
  }
  else if ( aCurrValue > maximum() )
  {
    aCurrValue = maximum();
  }

  setValue(aCurrValue);
}
