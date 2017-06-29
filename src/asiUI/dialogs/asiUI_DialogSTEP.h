//-----------------------------------------------------------------------------
// Created on: 16 May 2016
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

  asiUI_DialogSTEP(const Handle(ActAPI_IModel)&    model,
                   const Handle(asiData_PartNode)& part_n,
                   const Mode                      mode,
                   ActAPI_ProgressEntry            notifier,
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
  ActAPI_ProgressEntry     m_notifier; //!< Progress Notifier.

};

#endif
