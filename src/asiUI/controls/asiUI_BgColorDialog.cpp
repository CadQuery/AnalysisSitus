//-----------------------------------------------------------------------------
// Created on: 20 September 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_BgColorDialog.h>

// Qt includes
#pragma warning(push, 0)
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_BgColorDialog::asiUI_BgColorDialog(QWidget* parent) : QDialog(parent)
{
  QVBoxLayout* aMainLay = new QVBoxLayout();
  aMainLay->setMargin(5);

  m_pSelectGroup          = new QButtonGroup(this);
  QRadioButton* aSolid    = new QRadioButton("Solid", this);
  QRadioButton* aGradient = new QRadioButton("Gradient", this);

  m_pSelectGroup->addButton(aSolid, FT_Solid);
  m_pSelectGroup->addButton(aGradient, FT_Gradient);

  QHBoxLayout* aHLay = new QHBoxLayout();

  // add to layout
  aHLay->addWidget(aSolid);
  aHLay->addWidget(aGradient);
  aHLay->addSpacing(100);

  aMainLay->addLayout(aHLay);

  m_pSolidColor    = new asiUI_ColorButton(this);
  m_pGradientStart = new asiUI_ColorButton(this);
  m_pGradientEnd   = new asiUI_ColorButton(this);

  m_pSolidColor    ->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed) );
  m_pGradientStart ->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed) );
  m_pGradientEnd   ->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed) );

  connect( m_pSelectGroup, SIGNAL ( buttonClicked(int) ),
           this,           SLOT   ( onButtonSelected(int) ) );

  // add to layout
  aHLay = new QHBoxLayout();
  aHLay->addWidget(m_pSolidColor);
  aHLay->addWidget(m_pGradientStart);
  aHLay->addWidget(m_pGradientEnd);
  aMainLay->addLayout(aHLay);

  QPushButton* aBtnOk     = new QPushButton("OK", this);
  QPushButton* aBtnCancel = new QPushButton("Cancel", this);

  connect(aBtnOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(aBtnCancel, SIGNAL(clicked()), this, SLOT(reject()));

  aBtnOk->setDefault(true);

  aHLay = new QHBoxLayout();
  aHLay->addWidget(aBtnOk);
  aHLay->addWidget(aBtnCancel);
  aMainLay->addLayout(aHLay);

  // ok/cancel button
  setLayout(aMainLay);

  setFixedSize(minimumSizeHint());

  // set window title and hide context help
  setWindowTitle("Change background color");
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  // initialize colors
  m_pSelectGroup->button(FT_Solid)->click();

  QColor aBlack(Qt::black);
  this->SetColor(CLR_Solid, aBlack);
  this->SetColor(CLR_GradientStart, aBlack);
  this->SetColor(CLR_GradientEnd, aBlack);
}

//-----------------------------------------------------------------------------

void asiUI_BgColorDialog::onButtonSelected(const int theId)
{
  m_pSolidColor    ->setVisible(theId == FT_Solid);
  m_pGradientStart ->setVisible(theId == FT_Gradient);
  m_pGradientEnd   ->setVisible(theId == FT_Gradient);
}

//-----------------------------------------------------------------------------

void asiUI_BgColorDialog::SetColor(const ColorID theID,
                                   const QColor& theColor)
{
  switch ( theID )
  {
    case CLR_Solid         : m_pSolidColor->setColor(theColor); break;
    case CLR_GradientStart : m_pGradientStart->setColor(theColor); break;
    case CLR_GradientEnd   : m_pGradientEnd->setColor(theColor); break;
  }
}

//-----------------------------------------------------------------------------

QColor asiUI_BgColorDialog::GetColor(const ColorID theID)
{
  QColor aRet;
  switch ( theID )
  {
    case CLR_Solid         : aRet = m_pSolidColor->color();    break;
    case CLR_GradientStart : aRet = m_pGradientStart->color(); break;
    case CLR_GradientEnd   : aRet = m_pGradientEnd->color();   break;
  }
  return aRet;
}

//-----------------------------------------------------------------------------

int asiUI_BgColorDialog::GetFillType() const
{
  return m_pSelectGroup->checkedId();
}

//-----------------------------------------------------------------------------

void asiUI_BgColorDialog::SetFillType(const int theId) const
{
  m_pSelectGroup->button(theId)->click();
}
