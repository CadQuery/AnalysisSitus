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

public:

  QSize sizeHint() const;

protected:

  virtual void keyPressEvent(QKeyEvent* e);

protected:

  void addText(const QString& str,
               const bool     newBlock,
               const bool     isError);

  bool eval(const TCollection_AsciiString& cmd);

  TCollection_AsciiString
    currentCommand(const QTextCursor& cursor) const;

  QTextLine
    currentTextLine(const QTextCursor& cursor) const;

protected:

  int promptSize() const { return m_prompt.size(); }

protected:

  QString               m_prompt; //!< Prompt prefix.
  Handle(asiTcl_Interp) m_interp; //!< Tcl interpreter.

};

#endif
