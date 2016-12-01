//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
  //
  void onShowRobust    ();
  void onShowVertices  ();
  void onShowNormals   ();
  void onSelectFaces   ();
  void onSelectEdges   ();
  void onPickEdge      ();

signals:

  void partLoaded       ();
  void partSaved        ();
  void partModified     ();
  void robustOn         ();
  void robustOff        ();
  void verticesOn       ();
  void verticesOff      ();
  void normalsOn        ();
  void normalsOff       ();
  void selectionFacesOn ();
  void selectionEdgesOn ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;      //!< Button for BREP loading.
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
    //
    QPushButton* pShowRobust;    //!< Shows robust presentation.
    QPushButton* pShowVertices;  //!< Shows vertices of the model.
    QPushButton* pShowNormals;   //!< Shows normals for faces.
    QPushButton* pSelectFaces;   //!< Enables selection by faces.
    QPushButton* pSelectEdges;   //!< Enables selection by edges.
    QPushButton* pPickEdge;      //!< Picks a single edge.

    t_widgets() : pLoadBRep      (NULL),
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
                  //
                  pShowRobust    (NULL),
                  pShowVertices  (NULL),
                  pShowNormals   (NULL),
                  pSelectFaces   (NULL),
                  pSelectEdges   (NULL),
                  pPickEdge      (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;      pLoadBRep      = NULL;
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
      //
      delete pShowRobust;    pShowRobust    = NULL;
      delete pShowVertices;  pShowVertices  = NULL;
      delete pShowNormals;   pShowNormals   = NULL;
      delete pSelectFaces;   pSelectFaces   = NULL;
      delete pSelectEdges;   pSelectEdges   = NULL;
      delete pPickEdge;      pPickEdge      = NULL;
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
