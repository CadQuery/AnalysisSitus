//-----------------------------------------------------------------------------
// Created on: 12 July 2017
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

#ifndef asiUI_DialogInfoBase_h
#define asiUI_DialogInfoBase_h

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

//! Base class for dialogs to dump some text. Provides some stylization.
class asiUI_EXPORT asiUI_DialogInfoBase : public QWidget
{
  Q_OBJECT

public:

  asiUI_DialogInfoBase(const Handle(ActAPI_IModel)&            model,
                       const Handle(ActAPI_IProgressNotifier)& notifier,
                       QWidget*                                parent = NULL);

  virtual
    ~asiUI_DialogInfoBase();

public slots:

  void onClose();

protected:

  virtual void
    initialize() {}

  virtual QString
    groupBoxTitle() const { return "Noname"; }

  virtual QString
    dialogTitle() const { return "Noname"; }

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
