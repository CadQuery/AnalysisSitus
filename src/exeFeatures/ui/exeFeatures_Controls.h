//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_Controls_h
#define exeFeatures_Controls_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for feature controls.
class exeFeatures_Controls : public QWidget
{
  Q_OBJECT

public:

  exeFeatures_Controls(QWidget* parent = NULL);
  virtual ~exeFeatures_Controls();

public slots:

  void onShowTOPOGraph       ();
  void onShowAAG             ();
  void onElimSelected        ();
  //
  void onNonManifoldEdges    ();
  void onCheckSolidAngles    ();
  void onFindSmoothEdges     ();
  void onFindConvexOnly      ();
  void onFindSlots           ();
  void onFindHoles           ();
  void onSuppressHoles       ();
  void onFindPlanarHoles     ();
  void onFindFillets         ();
  //
  void onDetachSelected      ();
  void onDeleteSelected      ();
  void onDeleteSelectedFull  ();
  void onFindSameHosts       ();
  void onRehostFaces         ();
  void onUnperiodizeSelected ();
  void onCheckAlongCurvature ();
  void onOBB                 ();
  void onCR                  ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pShowTOPOGraph;       //!< Show topology graph.
    QPushButton* pShowAAG;             //!< Show Attributed Adjacency Graph.
    QPushButton* pElimSelected;        //!< Removes convex-only faces from AAG.
    //
    QPushButton* pNonManifoldEdges;    //!< Finds non-manifold edges.
    QPushButton* pCheckSolidAngles;    //!< Checks solid angles on convexity / concavity.
    QPushButton* pFindSmoothEdges;     //!< Finds C1 smooth edges.
    QPushButton* pFindConvexOnly;      //!< Finds convex-only faces.
    QPushButton* pFindSlots;           //!< Finds slot features.
    QPushButton* pFindHoles;           //!< Finds hole features.
    QPushButton* pSuppressHoles;       //!< Performs hole suppression.
    QPushButton* pFindPlanarHoles;     //!< Finds planar hole features.
    QPushButton* pFindFillets;         //!< Finds fillets.
    //
    QPushButton* pDetachSelected;      //!< Detaches selected faces from B-Rep.
    QPushButton* pDeleteSelected;      //!< Deletes selected faces (only).
    QPushButton* pDeleteSelectedFull;  //!< Deletes selected faces with all contours.
    QPushButton* pFindSameHosts;       //!< Attempts to find same-host faces.
    QPushButton* pRehostFaces;         //!< Merges the host geometry of selected faces.
    QPushButton* pUnperiodizeSelected; //!< Performs unperiodization of selected faces.
    QPushButton* pCheckAlongCurvature; //!< Evaluates along curvature for the selected edge.

    t_widgets() : pShowTOPOGraph       (NULL),
                  pShowAAG             (NULL),
                  pElimSelected        (NULL),
                  //
                  pNonManifoldEdges    (NULL),
                  pCheckSolidAngles    (NULL),
                  pFindSmoothEdges     (NULL),
                  pFindConvexOnly      (NULL),
                  pFindSlots           (NULL),
                  pFindHoles           (NULL),
                  pSuppressHoles       (NULL),
                  pFindPlanarHoles     (NULL),
                  pFindFillets         (NULL),
                  //
                  pDetachSelected      (NULL),
                  pDeleteSelected      (NULL),
                  pDeleteSelectedFull  (NULL),
                  pFindSameHosts       (NULL),
                  pRehostFaces         (NULL),
                  pUnperiodizeSelected (NULL),
                  pCheckAlongCurvature (NULL)
    {}

    void Release()
    {
      delete pShowTOPOGraph;       pShowTOPOGraph       = NULL;
      delete pShowAAG;             pShowAAG             = NULL;
      delete pElimSelected;        pElimSelected        = NULL;
      //
      delete pNonManifoldEdges;    pNonManifoldEdges    = NULL;
      delete pCheckSolidAngles;    pCheckSolidAngles    = NULL;
      delete pFindSmoothEdges;     pFindSmoothEdges     = NULL;
      delete pFindConvexOnly;      pFindConvexOnly      = NULL;
      delete pFindSlots;           pFindSlots           = NULL;
      delete pFindHoles;           pFindHoles           = NULL;
      delete pSuppressHoles;       pSuppressHoles       = NULL;
      delete pFindPlanarHoles;     pFindPlanarHoles     = NULL;
      delete pFindFillets;         pFindFillets         = NULL;
      //
      delete pDetachSelected;      pDetachSelected      = NULL;
      delete pDeleteSelected;      pDeleteSelected      = NULL;
      delete pDeleteSelectedFull;  pDeleteSelectedFull  = NULL;
      delete pFindSameHosts;       pFindSameHosts       = NULL;
      delete pRehostFaces;         pRehostFaces         = NULL;
      delete pUnperiodizeSelected; pUnperiodizeSelected = NULL;
      delete pCheckAlongCurvature; pCheckAlongCurvature = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
