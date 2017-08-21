//-----------------------------------------------------------------------------
// Created on: 25 November 2015
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

  void onLoadBRep      ();
  void onAddBRep       ();
  void onLoadSTEP      ();
  void onSaveSTEP      ();
  void onSavePly       ();
  void onSaveBRep      ();
  //
  void onCheckShape    ();
  void onTolerance     ();
  //
  void onSewing        ();
  void onMaximizeFaces ();
  void onFillGap       ();
  //
  void onShowVertices  ();
  void onSelectFaces   ();
  void onSelectEdges   ();

signals:

  void partLoaded       ();
  void partAdded        ();
  void partSaved        ();
  void partModified     ();
  void verticesOn       ();
  void verticesOff      ();
  void selectionFacesOn ();
  void selectionEdgesOn ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;      //!< Button for BREP loading.
    QPushButton* pAddBRep;       //!< Button for adding BREP.
    QPushButton* pLoadSTEP;      //!< Button for STEP loading.
    QPushButton* pSaveSTEP;      //!< Save shape to STEP.
    QPushButton* pSavePly;       //!< Save triangulation to ply.
    QPushButton* pSaveBRep;      //!< Save shape to BREP.
    //
    QPushButton* pCheckShape;    //!< Runs shape validity checker.
    QPushButton* pTolerance;     //!< Checks tolerance.
    //
    QPushButton* pSewing;        //!< Performs sewing.
    QPushButton* pMaximizeFaces; //!< Performs face maximization.
    QPushButton* pFillGap;       //!< Performs gap filling.
    //
    QPushButton* pShowVertices;  //!< Shows vertices of the model.
    QPushButton* pSelectFaces;   //!< Enables selection by faces.
    QPushButton* pSelectEdges;   //!< Enables selection by edges.

    t_widgets() : pLoadBRep      (NULL),
                  pAddBRep       (NULL),
                  pLoadSTEP      (NULL),
                  pSaveSTEP      (NULL),
                  pSavePly       (NULL),
                  pSaveBRep      (NULL),
                  //
                  pCheckShape    (NULL),
                  pTolerance     (NULL),
                  //
                  pSewing        (NULL),
                  pMaximizeFaces (NULL),
                  pFillGap       (NULL),
                  //
                  pShowVertices  (NULL),
                  pSelectFaces   (NULL),
                  pSelectEdges   (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;      pLoadBRep      = NULL;
      delete pAddBRep;       pAddBRep       = NULL;
      delete pLoadSTEP;      pLoadSTEP      = NULL;
      delete pSaveSTEP;      pSaveSTEP      = NULL;
      delete pSavePly;       pSavePly       = NULL;
      delete pSaveBRep;      pSaveBRep      = NULL;
      //
      delete pCheckShape;    pCheckShape    = NULL;
      delete pTolerance;     pTolerance     = NULL;
      //
      delete pSewing;        pSewing        = NULL;
      delete pMaximizeFaces; pMaximizeFaces = NULL;
      delete pFillGap;       pFillGap       = NULL;
      //
      delete pShowVertices;  pShowVertices  = NULL;
      delete pSelectFaces;   pSelectFaces   = NULL;
      delete pSelectEdges;   pSelectEdges   = NULL;
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
