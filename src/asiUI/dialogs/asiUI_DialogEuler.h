//-----------------------------------------------------------------------------
// Created on: 17 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiUI_DialogEuler_h
#define asiUI_DialogEuler_h

// asiUI includes
#include <asiUI_LineEdit.h>

// asiEngine includes
#include <asiEngine_Model.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for calculation of Euler-Poincare property.
class asiUI_DialogEuler : public QDialog
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_DialogEuler(const Handle(asiEngine_Model)& model,
                      ActAPI_ProgressEntry           notifier = NULL,
                      QWidget*                       parent   = NULL);

  asiUI_EXPORT virtual
    ~asiUI_DialogEuler();

public slots:

  void onCheck();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pCheck; //!< Check Euler-Poincare formula.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pGenus; //!< Genus of the solid model.
  //---------------------------------------------------------------------------

    t_widgets() : pCheck (NULL),
                  pGenus (NULL)
    {}

    void Release()
    {
      delete pCheck; pCheck = NULL;
      delete pGenus; pGenus = NULL;
    }
  };

  t_widgets               m_widgets;     //!< UI controls.
  QVBoxLayout*            m_pMainLayout; //!< Layout of the widget.
  Handle(asiEngine_Model) m_model;       //!< Data Model instance.
  ActAPI_ProgressEntry    m_progress;    //!< Progress notifier.

};

#endif
