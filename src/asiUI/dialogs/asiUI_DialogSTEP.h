//-----------------------------------------------------------------------------
// Created on: 16 May 2016
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

#ifndef asiUI_DialogSTEP_h
#define asiUI_DialogSTEP_h

// asiAlgo includes
#include <asiAlgo_Variable.h>

// asiData includes
#include <asiData_PartNode.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Sequence.hxx>

// Active Data includes
#include <ActAPI_IModel.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#pragma warning(pop)

//! Controls for STEP interoperability.
class asiUI_DialogSTEP : public QDialog
{
  Q_OBJECT

public:

  //! Exchange direction.
  enum Mode
  {
    Mode_Read,
    Mode_Write
  };

public:

  QString Filename; //!< Selected filename.

public:

  asiUI_DialogSTEP(const Handle(ActAPI_IModel)&    model,
                   const Handle(asiData_PartNode)& part_n,
                   const Mode                      mode,
                   ActAPI_ProgressEntry            notifier,
                   ActAPI_PlotterEntry             plotter,
                   QWidget*                        parent = NULL);

  virtual ~asiUI_DialogSTEP();

public slots:

  void onVarChanged(QTableWidgetItem*);
  void onProceed();

protected:

  void initialize();
  void initialize_Read(NCollection_Sequence<asiAlgo_Variable>& vars);
  void initialize_Write(NCollection_Sequence<asiAlgo_Variable>& vars);
  void saveVars();
  void proceed_Write();
  void proceed_Read();

private:

  Mode         m_mode;        //!< Translation mode.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QTableWidget* pOptions; //!< Data exchange options.
    QPushButton*  pProceed; //!< Button to proceed.

    t_widgets() : pOptions (NULL),
                  pProceed (NULL)
    {}

    void Release()
    {
      delete pOptions; pOptions = NULL;
      delete pProceed; pProceed = NULL;
    }
  };

  t_widgets                m_widgets;  //!< Involved widgets.
  //
  Handle(ActAPI_IModel)    m_model;    //!< Data Model instance.
  Handle(asiData_PartNode) m_part;     //!< Part Node.
  ActAPI_ProgressEntry     m_notifier; //!< Progress notifier.
  ActAPI_PlotterEntry      m_plotter;  //!< Imperative plotter.

};

#endif
