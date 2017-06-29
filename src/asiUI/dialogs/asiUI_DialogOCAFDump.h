//-----------------------------------------------------------------------------
// Created on: 23 June 2017
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

#ifndef asiUI_DialogOCAFDump_h
#define asiUI_DialogOCAFDump_h

// asiUI includes
#include <asiUI.h>

// Active Data includes
#include <ActAPI_IModel.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#pragma warning(pop)

//! Text view for OCAF dumps.
class asiUI_DialogOCAFDump : public QDialog
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_DialogOCAFDump(const Handle(ActAPI_IModel)&            model,
                         const Handle(ActAPI_IProgressNotifier)& notifier,
                         QWidget*                                parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_DialogOCAFDump();

public slots:

  void onClose();

protected:

  void initialize();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QTextEdit*    pEditor; //!< Text editor.
    QPushButton*  pClose;  //!< Close button.

    t_widgets() : pEditor (NULL),
                  pClose  (NULL)
    {}

    void Release()
    {
      delete pEditor; pEditor = NULL;
      delete pClose;  pClose = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

private:

  Handle(ActAPI_IModel) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry  m_notifier; //!< Progress Notifier.

};

#endif
