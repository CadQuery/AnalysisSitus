//-----------------------------------------------------------------------------
// Created on: 13 July 2017
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

#ifndef asiUI_StyledTextEdit_h
#define asiUI_StyledTextEdit_h

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QTextEdit>
#pragma warning(pop)

//! Editor with some cool styling.
class asiUI_EXPORT asiUI_StyledTextEdit : public QTextEdit
{
  Q_OBJECT

public:

  asiUI_StyledTextEdit(const QString& value,
                       QWidget*       parent = 0)
  : QTextEdit(value, parent) { this->stylize(); }

  asiUI_StyledTextEdit(QWidget* parent = 0)
  : QTextEdit(parent) { this->stylize(); }

protected:

  void stylize();

};

#endif
