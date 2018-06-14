//-----------------------------------------------------------------------------
// Created on: 23 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiUI_Console.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

// Tcl includes
#include <tcl.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QTextBlock>
#pragma warning(pop)

//-----------------------------------------------------------------------------

static QString READY_PROMPT = "> ";
static QString DOTS_PROMPT  = "... ";

//-----------------------------------------------------------------------------

asiUI_Console::asiUI_Console(const Handle(asiTcl_Interp)& interp,
                             QWidget*                     parent)
//
: m_interp(interp),
  asiUI_StyledTextEdit(parent)
{
  this->setUndoRedoEnabled( false );
  this->setLineWrapMode( QTextEdit::WidgetWidth );
  this->setWordWrapMode( QTextOption::WrapAnywhere );
  this->setAcceptRichText( false );
  this->setReadOnly( false );

  // unset busy cursor
  this->viewport()->unsetCursor();

  this->addText( QString("Tcl version: %1 [%2.%3.%4.%5]\n***")
                 .arg(TCL_VERSION)
                 .arg(TCL_MAJOR_VERSION)
                 .arg(TCL_MINOR_VERSION)
                 .arg(TCL_RELEASE_LEVEL)
                 .arg(TCL_RELEASE_SERIAL), false, false );

  // Add prompt
  m_prompt = READY_PROMPT;
  this->addText(READY_PROMPT, true, false);
}

//-----------------------------------------------------------------------------

//! \return size hint.
QSize asiUI_Console::sizeHint() const
{
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = (int) (side*0.25);
  const int height = (int) (side*0.1);

  QSize s(width, height);
  return s;
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

      this->adoptSourceCmd(cmdName, cmdName);

      if ( !this->eval(cmdName) )
        m_interp->GetProgress().SendLogMessage(LogErr(Normal) << "\t %1 ... TCL_ERROR" << cmdName);
      else
        m_interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\t %1 ... TCL_OK" << cmdName);

      // The following piece of code realizes "intelligent" movement of cursor.
      // The code checks whether next line is available by consulting block
      // number. If the block number if different after "Down" movement, then
      // it means that another line exists (if it does not, the cursor will not
      // move). If another line exists, we check the text at this line. If
      // the text is nothing but a prompt prefix ("> "), then we do not insert
      // new block, but simply let the cursor move to this prompt line
      // and reuse it so.

      const int bbefore = c.blockNumber();
      c.movePosition(QTextCursor::Down);
      const int bafter = c.blockNumber();
      //
      if ( bbefore == bafter ) // No next block exists, so the cursor did not move
      {
        // To avoid breaking command words if <Enter> is pressed not in the
        // end of line
        c.movePosition(QTextCursor::End);
        this->setTextCursor(c);

        // Add next block with a new prompt
        this->addText(READY_PROMPT, true, false);
      }
      else
      {
        QString nextStr = c.block().text(); // Check text at the next line
        //
        if ( nextStr == READY_PROMPT ) // If that's not a prompt, then work as usually
          this->setTextCursor(c); // If that's new line is a prompt, reuse it
      }

      break;
    }
    default:
      asiUI_StyledTextEdit::keyPressEvent(e);
  }
}

//-----------------------------------------------------------------------------

void asiUI_Console::addText(const QString& str,
                            const bool     newBlock,
                            const bool     isError)
{
  QTextCursor cursor = this->textCursor();
  QTextCharFormat cf = this->currentCharFormat();

  this->moveCursor( QTextCursor::End );
  if ( newBlock )
    cursor.insertBlock();
  if ( isError )
    cf.setForeground( QBrush( Qt::red ) );

  cursor.insertText( str, cf );
  this->moveCursor( QTextCursor::End );
  this->ensureCursorVisible();
}

//-----------------------------------------------------------------------------

bool asiUI_Console::eval(const TCollection_AsciiString& cmd)
{
  const bool isOk = ( m_interp->Eval(cmd) == TCL_OK );

  return isOk;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString asiUI_Console::currentCommand(const QTextCursor& cursor) const
{
  // Get text from text area
  QString cmd = cursor.block().text().trimmed();

  // Cut prompt prefix
  cmd = cmd.remove( 0, this->promptSize() );

  return QStr2AsciiStr(cmd);
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

//-----------------------------------------------------------------------------

bool asiUI_Console::adoptSourceCmd(const TCollection_AsciiString& cmd,
                                   TCollection_AsciiString&       adopted) const
{
  std::vector<std::string> argv;
  asiAlgo_Utils::Str::Split(cmd.ToCString(), " ", argv);

  // Change back slashes with forward slashes.
  if ( argv[0] == "source" )
  {
    if ( argv.size() == 2 )
    {
      std::replace(argv[1].begin(), argv[1].end(), '\\', '/');
      //
      adopted = TCollection_AsciiString( argv[0].c_str() )
              + " "
              + TCollection_AsciiString( argv[1].c_str() );

      return true;
    }
  }

  adopted = cmd; // Keep as-is.
  return false;
}
