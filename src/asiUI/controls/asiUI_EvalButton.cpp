//-----------------------------------------------------------------------------
// Created on: 22 February 2019
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
#include <asiUI_EvalButton.h>

//! Default constructor.
//! @param theParent [in] Parent takes control of the widget and
//! provides a layout for its display.
asiUI_EvalButton::asiUI_EvalButton(QWidget* theParent)
: QPushButton(theParent)
{
  QString aEvalButtonText = tr("EVALUATED_BUTTON_TEXT");
  setText( aEvalButtonText );

  QFont aStyledFont = this->font();
  aStyledFont.setBold( true );
  this->setFont( aStyledFont );

  QPalette aFontPalette = this->palette();
  aFontPalette.setColor( QPalette::ButtonText, QColor( Qt::blue ) );
  this->setPalette( aFontPalette );

  this->setCheckable(true);
}

//! \return square button size hint.
QSize asiUI_EvalButton::sizeHint () const
{
  QSize aBase2SquareSize = QPushButton::sizeHint();
  aBase2SquareSize.setWidth(aBase2SquareSize.height());
  return aBase2SquareSize;
}
