//-----------------------------------------------------------------------------
// Created on: 25 November 2015
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

#ifndef asiUI_ControlsPart_h
#define asiUI_ControlsPart_h

// asiUI includes
#include <asiUI_ViewerPart.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for main controls in Part viewer.
class asiUI_ControlsPart : public QWidget
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ControlsPart(const Handle(asiEngine_Model)& model,
                       asiUI_ViewerPart*              pPartViewer,
                       ActAPI_ProgressEntry           notifier,
                       ActAPI_PlotterEntry            plotter,
                       QWidget*                       parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ControlsPart();

public slots:

  void onLoadBRep       ();
  void onAddBRep        ();
  void onLoadSTEP       ();
  void onSaveSTEP       ();
  void onSavePly        ();
  void onSaveBRep       ();
  //
  void onCheckShape     ();
  void onTolerance      ();
  //
  void onSewing         ();
  void onMaximizeFaces  ();
  void onFillGap        ();
  //
  void onShowVertices   ();
  void onSelectFaces    ();
  void onSelectEdges    ();
  void onSelectVertices ();

signals:

  void partLoaded          (const QString&);
  void partAdded           (const QString&);
  void partSaved           ();
  void partModified        ();
  void verticesOn          ();
  void verticesOff         ();
  void selectionFacesOn    ();
  void selectionEdgesOn    ();
  void selectionVerticesOn ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;       //!< Button for BREP loading.
    QPushButton* pAddBRep;        //!< Button for adding BREP.
    QPushButton* pLoadSTEP;       //!< Button for STEP loading.
    QPushButton* pSaveSTEP;       //!< Save shape to STEP.
    QPushButton* pSavePly;        //!< Save triangulation to ply.
    QPushButton* pSaveBRep;       //!< Save shape to BREP.
    //
    QPushButton* pCheckShape;     //!< Runs shape validity checker.
    QPushButton* pTolerance;      //!< Checks tolerance.
    //
    QPushButton* pSewing;         //!< Performs sewing.
    QPushButton* pMaximizeFaces;  //!< Performs face maximization.
    QPushButton* pFillGap;        //!< Performs gap filling.
    //
    QPushButton* pShowVertices;   //!< Shows vertices of the model.
    QPushButton* pSelectFaces;    //!< Enables selection by faces.
    QPushButton* pSelectEdges;    //!< Enables selection by edges.
    QPushButton* pSelectVertices; //!< Enables selection by vertices.

    t_widgets() : pLoadBRep       (NULL),
                  pAddBRep        (NULL),
                  pLoadSTEP       (NULL),
                  pSaveSTEP       (NULL),
                  pSavePly        (NULL),
                  pSaveBRep       (NULL),
                  //
                  pCheckShape     (NULL),
                  pTolerance      (NULL),
                  //
                  pSewing         (NULL),
                  pMaximizeFaces  (NULL),
                  pFillGap        (NULL),
                  //
                  pShowVertices   (NULL),
                  pSelectFaces    (NULL),
                  pSelectEdges    (NULL),
                  pSelectVertices (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;       pLoadBRep      = NULL;
      delete pAddBRep;        pAddBRep       = NULL;
      delete pLoadSTEP;       pLoadSTEP      = NULL;
      delete pSaveSTEP;       pSaveSTEP      = NULL;
      delete pSavePly;        pSavePly       = NULL;
      delete pSaveBRep;       pSaveBRep      = NULL;
      //
      delete pCheckShape;     pCheckShape    = NULL;
      delete pTolerance;      pTolerance     = NULL;
      //
      delete pSewing;         pSewing        = NULL;
      delete pMaximizeFaces;  pMaximizeFaces = NULL;
      delete pFillGap;        pFillGap       = NULL;
      //
      delete pShowVertices;   pShowVertices  = NULL;
      delete pSelectFaces;    pSelectFaces   = NULL;
      delete pSelectEdges;    pSelectEdges   = NULL;
      delete pSelectVertices; pSelectEdges   = NULL;
    }
  };

  t_widgets               m_widgets;      //!< Involved widgets.
  Handle(asiEngine_Model) m_model;        //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer;   //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;     //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;      //!< Imperative Plotter.
  int                     m_iPrevSelMode; //!< Previous selection mode.

};

#endif
