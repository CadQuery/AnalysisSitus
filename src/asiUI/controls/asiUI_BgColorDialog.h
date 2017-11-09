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

#ifndef asiUI_BgColorDialog_h
#define asiUI_BgColorDialog_h

// asiUI includes
#include <asiUI_ColorButton.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#pragma warning(pop)

//! Dialog to choose background color.
class asiUI_EXPORT asiUI_BgColorDialog : public QDialog
{
  Q_OBJECT

public:

  enum FillType
  {
    FT_Solid,
    FT_Gradient
  };
  
  enum ColorID
  {
    CLR_Solid,
    CLR_GradientStart,
    CLR_GradientEnd
  };

public:

  asiUI_BgColorDialog(QWidget* parent = 0);

public:

  int
    GetFillType() const;
  
  void
    SetFillType(const int id) const;

  void
    SetColor(const ColorID id, const QColor& color);

  QColor
    GetColor(const ColorID id);

public slots:
  
  void onButtonSelected(const int id);

private:

  asiUI_ColorButton* m_pSolidColor;    //!< Button to pick sole color.
  asiUI_ColorButton* m_pGradientStart; //!< Button to pick the first color of gradient.
  asiUI_ColorButton* m_pGradientEnd;   //!< Button to pick the second color of gradient.
  QButtonGroup*      m_pSelectGroup;   //!< Group of buttons.

};

#endif
