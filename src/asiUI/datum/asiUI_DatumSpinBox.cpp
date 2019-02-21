///-----------------------------------------------------------------------------
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

// asiUI includes
#include <asiUI_DatumSpinBox.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>

// Qtx includes
#pragma warning(push, 0)
#include <QtxIntSpinBox.h>
#pragma warning(pop)

//! Constructor.
//! \param theDicID [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theDatumFlags [in] datum subwidget and option flags.
asiUI_DatumSpinBox::asiUI_DatumSpinBox(const QString& theDicID, 
                                         QWidget* theParent,
                                         const int theDatumFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDicID, theParent, convertFlags(theDatumFlags));
  
  init(m_pEditor);

  connect( m_pEditor, SIGNAL( EditingFinished() ),
           this, SIGNAL( EditingFinished() ) );
}

//! Get the spin box increment value.
//! \return current increment value.
int asiUI_DatumSpinBox::GetStep() const
{
  return m_pEditor->step();
}

//! Set the spin box increment value.
//! \param theStepSize [in] new increment value.
void asiUI_DatumSpinBox::SetStep(const int theStepSize)
{
  m_pEditor->setStep(theStepSize);
}

//! Update unit system.
void asiUI_DatumSpinBox::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumSpinBox::getDatum() const
{
  return m_pEditor;
}

//! Hilight edited string with the specified color.
//! \param theColor [in] the color to hilight.
void asiUI_DatumSpinBox::colorizeText(const QColor& theColor)
{
  QtxIntSpinBox* aSpinBox = m_pEditor->GetSpinBox();
  if ( !aSpinBox )
    return;

  QPalette aColorizedPalette = aSpinBox->palette();

  aColorizedPalette.setColor(QPalette::Active, QPalette::Text, theColor);

  aSpinBox->setPalette(aColorizedPalette);
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Constructor. Initializes controls and dialog mode
asiUI_DatumSpinBox::Editor::Editor(const QString& theDicID,
                                       QWidget* theParent,
                                       const int theFlags)
: QDS_SpinBox(theDicID, theParent, theFlags)
{
  // set spin box options
  Handle(asiAlgo_DictionaryItem) aDicItem =
    Handle(asiAlgo_DictionaryItem)::DownCast( dicItem() );

  // step option
  if ( aDicItem.IsNull() )
    return;

  const asiUI_WidgetParameterMap& aParamMap = aDicItem->GetWidgetParams();

  TCollection_AsciiString aStepOption("step");
  if ( !aParamMap.IsBound(aStepOption) )
    return;

  TCollection_AsciiString aValue = aParamMap.Find(aStepOption);
  if ( aValue.IsRealValue() )
  {
    setStep( aValue.RealValue() );
  }

  QtxIntSpinBox* aSpinBox = spinBox();
  connect( aSpinBox, SIGNAL( editingFinished() ),
           this, SIGNAL( EditingFinished() ) );
}

//! Update system of units.
void asiUI_DatumSpinBox::Editor::UpdateUnits()
{
  unitSystemChanged( QString() );
}

//! update values when unit system changes.
void asiUI_DatumSpinBox::Editor::unitSystemChanged(const QString& theSystem)
{
  QDS_Datum::unitSystemChanged(theSystem);

  QtxIntSpinBox* aSpinBox = spinBox();
  if ( aSpinBox )
  {
    aSpinBox->setSuffix( suffix() );
    aSpinBox->setPrefix( prefix() );
  }

  int aPreci = 1;
  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aPreci = aDicItem->GetPrecision();

  aSpinBox->setMinimum( INT_MIN );
  aSpinBox->setMaximum( INT_MAX );
}
