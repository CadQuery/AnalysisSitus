//-----------------------------------------------------------------------------
// Created on: 11 December(*) 2017
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

#ifndef asiUI_DialogTopoGraphDepth_h
#define asiUI_DialogTopoGraphDepth_h

// asiUI includes
#include <asiUI_LineEdit.h>
#include <asiUI_ViewerPart.h>

// asiEngine includes
#include <asiEngine_Model.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for selection of depth of a topology graph.
class asiUI_DialogTopoGraphDepth : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogTopoGraphDepth(const Handle(asiEngine_Model)& model,
                             const TopoDS_Shape&            partShape,
                             asiUI_ViewerPart*              partViewer,
                             ActAPI_ProgressEntry           progress,
                             QWidget*                       parent = NULL);

  virtual ~asiUI_DialogTopoGraphDepth();

public slots:

  void onOpen();
  void onColorizeLocations();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton* pOpen;              //!< Button to open topology graph.
  //---------------------------------------------------------------------------
    QComboBox*   pDepth;             //!< Depth of traversal.
    QCheckBox*   pColorizeLocations; //!< Whether to colorize locations.
  //---------------------------------------------------------------------------

    t_widgets() : pOpen              (NULL),
                  pDepth             (NULL),
                  pColorizeLocations (NULL)
    {}

    void Release()
    {
      delete pOpen;              pOpen              = NULL;
      delete pDepth;             pDepth             = NULL;
      delete pColorizeLocations; pColorizeLocations = NULL;
    }
  };

  t_widgets               m_widgets;      //!< UI controls.
  QVBoxLayout*            m_pMainLayout;  //!< Layout of the widget.
  Handle(asiEngine_Model) m_model;        //!< Data Model instance.
  TopoDS_Shape            m_partShape;    //!< Part shape.
  bool                    m_bColorizeLoc; //!< Indicates whether to colorize locations.
  asiUI_ViewerPart*       m_pPartViewer;  //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;     //!< Progress Notifier.

};

#endif
