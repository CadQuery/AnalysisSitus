//-----------------------------------------------------------------------------
// Created on: 20 September 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
