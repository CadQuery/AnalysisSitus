//-----------------------------------------------------------------------------
// Created on: 13 July 2016
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

#ifndef asiUI_DialogFindFace_h
#define asiUI_DialogFindFace_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>
#include <asiUI_ViewerPart.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>

// Qt includes
#pragma warning(push, 0)
#include <QComboBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Dialog for finding face of interest.
class asiUI_DialogFindFace : public QDialog
{
  Q_OBJECT

public:

  //! Identification types.
  enum FaceIdType
  {
    FaceIdType_Serial = 0, //!< Exploration index (1-based).
    FaceIdType_Transient,  //!< Raw pointer.
    FaceIdType_Persistent  //!< Through naming service.
  };

public:

  //! Ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] prsMgr   presentation manager.
  //! \param[in] pViewer  connected viewer.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  //! \param[in] parent   parent widget.
  asiUI_DialogFindFace(const Handle(asiEngine_Model)&             model,
                       const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                       asiUI_ViewerPart*                          pViewer,
                       ActAPI_ProgressEntry                       progress,
                       ActAPI_PlotterEntry                        plotter,
                       QWidget*                                   parent = nullptr);

  //! Dtor.
  virtual ~asiUI_DialogFindFace();

public slots:

  //! Reaction on changing "ID type" combobox.
  //! \param[in] typeIdx new 0-based type index.
  void onChangeIdType(const int typeIdx);

  //! Reaction on clicking "Find" button.
  void onFind();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pFind;        //!< Find face of interest.
  //---------------------------------------------------------------------------
    QComboBox*      pFaceIdSel;   //!< Selector for face ID (serial, transient, persistent).
    asiUI_LineEdit* pIdSerial;    //!< Face index.
    asiUI_LineEdit* pIdTransient; //!< Face address.
    asiUI_LineEdit* pIdName;      //!< Face name.
  //---------------------------------------------------------------------------

    t_widgets() : pFind        (nullptr),
                  pFaceIdSel   (nullptr),
                  pIdSerial    (nullptr),
                  pIdTransient (nullptr),
                  pIdName      (nullptr)
    {}

    void Release()
    {
      delete pFind;        pFind        = nullptr;
      delete pFaceIdSel;   pFaceIdSel   = nullptr;
      delete pIdSerial;    pIdSerial    = nullptr;
      delete pIdTransient; pIdTransient = nullptr;
      delete pIdName;      pIdName      = nullptr;
    }
  };

  t_widgets                           m_widgets;     //!< UI controls.
  QVBoxLayout*                        m_pMainLayout; //!< Layout of the widget.
  asiUI_ViewerPart*                   m_pViewer;     //!< External reference to viewer.
  Handle(asiEngine_Model)             m_model;       //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;      //!< Presentation Manager.

  /* Diagnostics */

  ActAPI_ProgressEntry m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
