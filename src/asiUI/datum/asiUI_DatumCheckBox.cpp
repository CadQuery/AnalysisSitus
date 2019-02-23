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
#include <asiUI_DatumCheckBox.h>

// Qt includes
#pragma warning(push, 0)
#include <QVariant>
#pragma warning(pop)

//! Constructor.
//! \param theDictId [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theFlags [in] datum subwidget and option flags.
asiUI_DatumCheckBox::asiUI_DatumCheckBox(const QString& theDictId, 
                                           QWidget* theParent,
                                           const int theFlags)
: asiUI_Datum(theParent)
{
  m_pDatumImpl = new CheckBoxImpl( theDictId, theParent, convertFlags(theFlags) );
  init(m_pDatumImpl);

  connect( m_pDatumImpl, SIGNAL( toggled(bool) ), this, SIGNAL( Toggled(bool) ) );
}

//! Get current check box state.
//! \return check box state.
bool asiUI_DatumCheckBox::IsChecked() const
{
  return m_pDatumImpl->isChecked();
}

//! Set the check box state to theIsChecked.
//! \param theIsChecked [in] set checked state if true, and unchecked if
//!        otherwise.
void asiUI_DatumCheckBox::SetChecked(const bool theIsChecked)
{
  m_pDatumImpl->setChecked(theIsChecked);
}

//! Clear checkbox and switches its state to "PartiallyChecked" (undefined)
void asiUI_DatumCheckBox::Clear()
{
  m_pDatumImpl->clear();
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumCheckBox::getDatum() const
{
  return m_pDatumImpl;
}

// ============================================================================
//                         Extended check box editor
// ============================================================================

//! Default constructor.
//! Is simply void.
asiUI_DatumCheckBox::CheckBoxImpl::CheckBoxImpl(const QString& theDictId,
                                                 QWidget* theParent,
                                                 const int theFlags)
: asiUI_Datum::DatumImpl<QDS_CheckBox>(theDictId, theParent, theFlags)
{
  //
}

//! Process input value and set it to the control.
//! \param theValue [in] the value to set for the control
void asiUI_DatumCheckBox::CheckBoxImpl::setValue(const QVariant& theValue)
{
  if ( theValue.type() == QVariant::Bool )
  {
    bool isCheck = theValue.toBool();
    QDS_CheckBox::setValue( QVariant( isCheck ? 1 : 0 ) );
  }
  else if ( theValue.type() == QVariant::String )
  {
    QString checkStr = theValue.toString();
    QDS_CheckBox::setValue( QVariant( checkStr.contains("true") ? 1 : 0 ) );
  }
  else
  {
    QDS_CheckBox::setValue(theValue);
  }
}

//! Transform control's state into relevant value.
//! \return value from the control's state.
QVariant asiUI_DatumCheckBox::CheckBoxImpl::value() const
{
  QVariant aValue = QDS_CheckBox::value();
  if ( aValue.type() == QVariant::Int )
  {
    return QVariant( aValue.toInt() == 1 ? true : false );
  }
  
  if ( !aValue.convert(QVariant::Bool) )
    return false;

  return aValue;
}
