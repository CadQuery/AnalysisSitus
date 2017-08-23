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

// Own include
#include <asiUI_Console.h>

// asiUI includes
#include <asiUI_Common.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

// Tcl includes
#include <tcl.h>

// Qt includes
#pragma warning(push, 0)
#include <QTextBlock>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_Console::asiUI_Console(const Handle(asiTcl_Interp)& interp,
                             QWidget*                     parent)
//
: m_interp(interp),
  asiUI_StyledTextEdit(parent)
{
  this->append( QString("Tcl version: %1 [%2.%3.%4.%5]\n***\n")
               .arg(TCL_VERSION)
               .arg(TCL_MAJOR_VERSION)
               .arg(TCL_MINOR_VERSION)
               .arg(TCL_RELEASE_LEVEL)
               .arg(TCL_RELEASE_SERIAL) );
}

//-----------------------------------------------------------------------------

void asiUI_Console::keyPressEvent(QKeyEvent* e)
{
  QTextCursor c = this->textCursor();

  switch ( e->key() )
  {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
      TCollection_AsciiString cmdName = this->currentCommand(c);
      std::cout << "Command entered: " << cmdName.ToCString() << std::endl;
      //
      if ( !this->eval(cmdName) )
        std::cout << "\t ... NOK" << std::endl;
      else
        std::cout << "\t ... OK" << std::endl;

      break;
    }
    default:
      break;
  }

  asiUI_StyledTextEdit::keyPressEvent(e);
}

//-----------------------------------------------------------------------------

bool asiUI_Console::eval(const TCollection_AsciiString& cmd)
{
  if ( m_interp->Eval(cmd) == TCL_OK )
    return true;

  return false;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString asiUI_Console::currentCommand(const QTextCursor& cursor) const
{
  return QStr2AsciiStr( cursor.block().text().trimmed() );
}

//-----------------------------------------------------------------------------

QTextLine asiUI_Console::currentTextLine(const QTextCursor& cursor) const
{
  const QTextBlock block = cursor.block();
  if ( !block.isValid() )
    return QTextLine();

  const QTextLayout* layout = block.layout();
  if ( !layout )
    return QTextLine();

  const int relativePos = cursor.position() - block.position();
  return layout->lineForTextPosition(relativePos);
}
