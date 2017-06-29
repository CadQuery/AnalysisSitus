//-----------------------------------------------------------------------------
// Created on: 13 July 2016
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
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for finding face of interest.
class asiUI_DialogFindFace : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogFindFace(const Handle(asiEngine_Model)&             model,
                       const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                       asiUI_ViewerPart*                          pViewer,
                       QWidget*                                   parent = NULL);

  virtual ~asiUI_DialogFindFace();

public slots:

  void onUseAddress();
  void onFind();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pFind;       //!< Find face of interest.
  //---------------------------------------------------------------------------
    QCheckBox*      pUseAddress; //!< Indicates whether to use address.
    asiUI_LineEdit* pIndex;      //!< Face index.
    asiUI_LineEdit* pAddress;    //!< Face address.
  //---------------------------------------------------------------------------

    t_widgets() : pFind       (NULL),
                  pUseAddress (NULL),
                  pIndex      (NULL),
                  pAddress    (NULL)
    {}

    void Release()
    {
      delete pFind;       pFind       = NULL;
      delete pUseAddress; pUseAddress = NULL;
      delete pIndex;      pIndex      = NULL;
      delete pAddress;    pAddress    = NULL;
    }
  };

  t_widgets                           m_widgets;     //!< UI controls.
  QVBoxLayout*                        m_pMainLayout; //!< Layout of the widget.
  asiUI_ViewerPart*                   m_pViewer;     //!< External reference to viewer.
  Handle(asiEngine_Model)             m_model;       //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;      //!< Presentation Manager.

};

#endif
