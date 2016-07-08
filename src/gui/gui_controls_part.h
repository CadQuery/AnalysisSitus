//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_part_h
#define gui_controls_part_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for main controls in Part viewer.
class gui_controls_part : public QWidget
{
  Q_OBJECT

public:

  gui_controls_part(QWidget* parent = NULL);
  virtual ~gui_controls_part();

public slots:

  void onLoadBRep         ();
  void onLoadSTEP         ();
  void onSaveSTEP         ();
  void onSavePly          ();
  void onSaveBRep         ();
  //
  void onShowTOPOGraph    ();
  void onShowAAG          ();
  void onElimSelected     ();
  void onCheckShape       ();
  void onTolerance        ();
  void onNonManifoldEdges ();
  //
  void onSewing           ();
  void onMaximizeFaces    ();
  void onOBB              ();
  void onCR               ();
  void onCloudify         ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;         //!< Button for BREP loading.
    QPushButton* pLoadSTEP;         //!< Button for STEP loading.
    QPushButton* pSaveSTEP;         //!< Save shape to STEP.
    QPushButton* pSavePly;          //!< Save triangulation to ply.
    QPushButton* pSaveBRep;         //!< Save shape to BREP.
    //
    QPushButton* pShowTOPOGraph;    //!< Show topology graph.
    QPushButton* pShowAAG;          //!< Show Attributed Adjacency Graph.
    QPushButton* pElimSelected;     //!< Removes convex-only faces from AAG.
    QPushButton* pCheckShape;       //!< Runs shape validity checker.
    QPushButton* pTolerance;        //!< Checks tolerance.
    QPushButton* pNonManifoldEdges; //!< Finds non-manifold edges.
    //
    QPushButton* pSewing;           //!< Performs sewing.
    QPushButton* pMaximizeFaces;    //!< Performs face maximization.
    QPushButton* pOBB;              //!< Builds OBB.
    QPushButton* pCR;               //!< Runs canonical recognition.
    QPushButton* pCloudify;         //!< Builds a point cloud from the model.

    t_widgets() : pLoadBRep         (NULL),
                  pLoadSTEP         (NULL),
                  pSaveSTEP         (NULL),
                  pSavePly          (NULL),
                  pSaveBRep         (NULL),
                  //
                  pShowTOPOGraph    (NULL),
                  pShowAAG          (NULL),
                  pElimSelected     (NULL),
                  pCheckShape       (NULL),
                  pTolerance        (NULL),
                  pNonManifoldEdges (NULL),
                  //
                  pSewing           (NULL),
                  pMaximizeFaces    (NULL),
                  pOBB              (NULL),
                  pCR               (NULL),
                  pCloudify         (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;         pLoadBRep         = NULL;
      delete pLoadSTEP;         pLoadSTEP         = NULL;
      delete pSaveSTEP;         pSaveSTEP         = NULL;
      delete pSavePly;          pSavePly          = NULL;
      delete pSaveBRep;         pSaveBRep         = NULL;
      //
      delete pShowTOPOGraph;    pShowTOPOGraph    = NULL;
      delete pShowAAG;          pShowAAG          = NULL;
      delete pElimSelected;     pElimSelected     = NULL;
      delete pCheckShape;       pCheckShape       = NULL;
      delete pTolerance;        pTolerance        = NULL;
      delete pNonManifoldEdges; pNonManifoldEdges = NULL;
      //
      delete pSewing;           pSewing           = NULL;
      delete pMaximizeFaces;    pMaximizeFaces    = NULL;
      delete pOBB;              pOBB              = NULL;
      delete pCR;               pCR               = NULL;
      delete pCloudify;         pCloudify         = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
