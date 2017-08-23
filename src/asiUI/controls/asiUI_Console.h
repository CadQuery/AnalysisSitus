//-----------------------------------------------------------------------------
// Created on: 23 August 2017
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

#ifndef asiUI_Console_h
#define asiUI_Console_h

// asiUI includes
#include <asiUI_StyledTextEdit.h>

// asiTcl includes
#include <asiTcl_Interp.h>

// Qt includes
#pragma warning(push, 0)
#include <QKeyEvent>
#pragma warning(pop)

//! Console window.
class asiUI_EXPORT asiUI_Console : public asiUI_StyledTextEdit
{
  Q_OBJECT

public:

  //! Constructs UI console and initializes it with Tcl interpreter.
  //! \param[in] interp Tcl interpreter to use.
  //! \param[in] parent parent Qt widget.
  asiUI_Console(const Handle(asiTcl_Interp)& interp,
                QWidget*                     parent = 0);

protected:

  virtual void keyPressEvent(QKeyEvent* e);

protected:

  bool eval(const TCollection_AsciiString& cmd);

  TCollection_AsciiString
    currentCommand(const QTextCursor& cursor) const;

  QTextLine
    currentTextLine(const QTextCursor& cursor) const;

protected:

  Handle(asiTcl_Interp) m_interp; //!< Tcl interpreter.

};

#endif
