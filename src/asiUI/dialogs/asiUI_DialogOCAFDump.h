//-----------------------------------------------------------------------------
// Created on: 23 June 2017
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

#ifndef asiUI_DialogOCAFDump_h
#define asiUI_DialogOCAFDump_h

// asiUI includes
#include <asiUI_StyledTextEdit.h>

// Active Data includes
#include <ActAPI_IModel.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Text view for OCAF dumps.
class asiUI_DialogOCAFDump : public QDialog
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_DialogOCAFDump(const Handle(ActAPI_IModel)& model,
                         ActAPI_ProgressEntry         progress,
                         QWidget*                     parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_DialogOCAFDump();

protected:

  virtual void initialize();

public slots:

  void onClose();

protected:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_base_widgets
  {
    QTextEdit*    pEditor; //!< Text editor.
    QPushButton*  pClose;  //!< Close button.

    t_base_widgets() : pEditor (NULL),
                       pClose  (NULL)
    {}

    void Release()
    {
      delete pEditor; pEditor = NULL;
      delete pClose;  pClose = NULL;
    }
  };

  t_base_widgets m_widgets; //!< Involved widgets.

protected:

  Handle(ActAPI_IModel) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry  m_notifier; //!< Progress Notifier.

};

#endif
