//-----------------------------------------------------------------------------
// Created on: 18 December 2017
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

#ifndef asiUI_DialogPipelines_h
#define asiUI_DialogPipelines_h

// asiUI includes
#include <asiUI.h>

// asiVisu includes
#include <asiVisu_Prs.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog to configure pipelines for the given 3D presentation.
class asiUI_EXPORT asiUI_DialogPipelines : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogPipelines(const Handle(asiVisu_Prs)& prs,
                        ActAPI_ProgressEntry       notifier,
                        QWidget*                   parent = NULL);

  virtual ~asiUI_DialogPipelines();

protected:

  void initialize();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QTableWidget* pPipelines; //!< Table with pipelines.

    t_widgets() : pPipelines(NULL) {}

    void Release()
    {
      delete pPipelines; pPipelines = NULL;
    }
  };

  t_widgets            m_widgets;  //!< Involved widgets.
  ActAPI_ProgressEntry m_notifier; //!< Progress Notifier.
  //
  Handle(asiVisu_Prs)  m_prs;      //!< Presentation to analyze.

};

#endif
