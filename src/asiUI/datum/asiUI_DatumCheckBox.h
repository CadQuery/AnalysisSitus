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

#ifndef asiUI_DatumCheckBox_HeaderFile
#define asiUI_DatumCheckBox_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_CheckBox.h>
#pragma warning(pop)

//! \ingroup GUI
//!
//! Datum with control corresponding to check box.
//! This control can have three states:
//! 1 (on/true)
//! 0 (off/false).
//! -1 (PartiallyChecked).
//!
//! CheckBox don't take into account standard parameter properties
//! (minimum, maximum, filter)
//! CheckBox can set and get following values for access methods (SetString(),
//! SetInteger(), SetDouble(), SetString(), GetInteger(), GetDouble()):
//! "1"  - check box state is switched on.
//! "0"  - check box state is switched off.
//! "-1" - check box state is "PartiallyChecked" (undefined).
//! PartiallyChecked state can be tested using methods Clear()
//! and this state can be checked by IsEmpty() method.
//! 
//! GetValue() method returns QVariant of boolean type which corresponds
//! to checked/unchecked state. GetInteger() returns -1, 0, 1 values
//! corresponding to the three states.
class asiUI_EXPORT asiUI_DatumCheckBox : public asiUI_Datum
{
  Q_OBJECT

  class CheckBoxImpl;

public:

  asiUI_DatumCheckBox(const QString& theDicID, 
                       QWidget*       theParent = 0,
                       const int      theDatumFlags = All);

  bool IsChecked() const;

  void SetChecked(const bool theIsChecked);

  void Clear();

signals:

  //! Notify when checkbox has been toggled.
  void Toggled(bool theIsChecked);

protected:

  virtual QDS_Datum* getDatum() const;

private:

  CheckBoxImpl* m_pDatumImpl;
};

// ============================================================================
//                         Extended check box editor
// ============================================================================

//! \ingroup GUI
//!
//! Check Box editor with boolean value type result.
class asiUI_DatumCheckBox::CheckBoxImpl : public asiUI_Datum::DatumImpl<QDS_CheckBox>
{
  Q_OBJECT

public:

  CheckBoxImpl(const QString& theDictId,
               QWidget* theParent = 0,
               const int theFlags = All);

  virtual void setValue(const QVariant& theValue);

  virtual QVariant value() const;
};

#endif
