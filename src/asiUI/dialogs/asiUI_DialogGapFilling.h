//-----------------------------------------------------------------------------
// Created on: 10 March 2017
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

#ifndef asiUI_DialogGapFilling_h
#define asiUI_DialogGapFilling_h

// asiUI includes
#include <asiUI_LineEdit.h>
#include <asiUI_ViewerPart.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for gap filling.
class asiUI_DialogGapFilling : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogGapFilling(const Handle(asiEngine_Model)&             model,
                         const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                         asiUI_ViewerPart*                          pViewer,
                         ActAPI_ProgressEntry                       progress,
                         ActAPI_PlotterEntry                        plotter,
                         QWidget*                                   parent = nullptr);

  virtual ~asiUI_DialogGapFilling();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton* pPerform;  //!< Button to perform.
  //---------------------------------------------------------------------------
    QComboBox*   pOrder;        //!< Order of continuity to constraint pinpoints.
    QCheckBox*   pAutoComplete; //!< Indicates whether to complete edge loop.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform      (nullptr),
                  pOrder        (nullptr),
                  pAutoComplete (nullptr)
    {}

    void Release()
    {
      delete pPerform;      pPerform      = nullptr;
      delete pOrder;        pOrder        = nullptr;
      delete pAutoComplete; pAutoComplete = nullptr;
    }
  };

  t_widgets                           m_widgets;     //!< UI controls.
  QVBoxLayout*                        m_pMainLayout; //!< Layout of the widget.
  Handle(asiEngine_Model)             m_model;       //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;      //!< Presentation Manager.
  asiUI_ViewerPart*                   m_pViewer;     //!< External reference to viewer.
  ActAPI_ProgressEntry                m_notifier;    //!< Progress Notifier.
  ActAPI_PlotterEntry                 m_plotter;     //!< Imperative plotter.

};

#endif
