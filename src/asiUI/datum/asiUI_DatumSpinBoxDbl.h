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

#ifndef asiUI_DatumSpinBoxDbl_HeaderFile
#define asiUI_DatumSpinBoxDbl_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_SpinBoxDbl.h>
#include <QtxDoubleSpinBox.h>
#pragma warning(pop)

class QLineEdit;

//! Datum with control corresponding to spin box.
//! This control is suitable for double numbers.
//! User can input data directly in the spin box or can modify
//! current value by clicking arrow (+/-) buttons.
class asiUI_EXPORT asiUI_DatumSpinBoxDbl : public asiUI_Datum
{
  Q_OBJECT

public:

  class SpinBoxImpl;
  class SpinBoxWidget;

public:

  asiUI_DatumSpinBoxDbl(const QString& theDictId, 
                         QWidget* theParent = 0,
                         const int theFlags = All);

  double GetStep() const;

  void SetStep(const double theStepSize);

// Value range:
public:

  void UserRangeModeOn(const double theMin, const double theMax);

  void UserRangeModeOff();

signals:

  //! This signal is emitted when the Return/Enter
  //! key is pressed or the editor loses focus.
  void EditingFinished();

protected:

  virtual QDS_Datum* getDatum() const;

private:

  SpinBoxImpl* m_pDatumImpl;
};

// ============================================================================
//                         Datum spin box implementation
// ============================================================================

//! \ingroup GUI
//!
//! Double spin box that supports precision,
//! loaded from xml data dictionary.
class asiUI_DatumSpinBoxDbl::SpinBoxImpl : public asiUI_Datum::DatumImpl<QDS_SpinBoxDbl>
{
  Q_OBJECT

public:

  SpinBoxImpl(const QString& theDictId,
              QWidget* theParent = 0,
              const int theFlags = All);

  QtxDoubleSpinBox* GetSpinBox() { return spinBox(); }

// Value range:
public:

  //! Enables user-defined min/max range mode.
  //! \param theMin [in] minimal value to use.
  //! \param theMax [in] maximal value to use.
  inline void UserRangeModeOn(const double theMin, const double theMax)
  {
    m_bUserRangeMode = true;
    m_fMin = theMin;
    m_fMax = theMax;

    this->onParamChanged();
    emit EditingFinished();
  }

  //! Disables user-defined min/max range mode.
  inline void UserRangeModeOff()
  {
    m_bUserRangeMode = false;

    this->onParamChanged();
    emit EditingFinished();
  }

  //! Returns minimal value.
  //! \return minimal value acceptable by this Editor.
  inline virtual QString minValue() const
  {
    return m_bUserRangeMode ? QString::number(m_fMin) : QDS_SpinBoxDbl::minValue();
  }

  //! Returns maximal value.
  //! \return maximal value acceptable by this Editor.
  virtual QString maxValue() const
  {
    return m_bUserRangeMode ? QString::number(m_fMax) : QDS_SpinBoxDbl::maxValue();
  }

signals:

  //! This signal is emitted when the Return or Enter 
  //! key is pressed or the editor loses focus.
  void EditingFinished();

protected:

  virtual QWidget* createControl(QWidget* theParent);

  virtual void unitSystemChanged(const QString& theSystemId);

protected slots:

  void onChanged();

protected:

  bool m_bUserRangeMode; //!< User-defined value range mode.
  double m_fMin, m_fMax; //!< Minimal/maximal values.
};

// ============================================================================
//                         Generic spin box widget
// ============================================================================

//! \ingroup GUI
//!
//! Generic spin box widget that enables installation of validators.
//! Common functionality inherited from QtxDoubleSpinBox, while
//! all validation redefined for validators approach.
class asiUI_DatumSpinBoxDbl::SpinBoxWidget : public QtxDoubleSpinBox
{
  Q_OBJECT

public:

  SpinBoxWidget(QWidget* theParent);

  virtual void fixup(QString& theInput) const;

  virtual QValidator::State validate(QString& theInput, int& thePos) const;

  void SetValidator(QValidator* theValidator);

  QValidator* GetValidator();

  virtual QString textFromValue(double theValue) const;

  void SetLineEdit(QLineEdit* theEditor) { setLineEdit(theEditor); }

  virtual void stepBy(int theCount);

private:

  QValidator* m_pValidator;
};

#endif
