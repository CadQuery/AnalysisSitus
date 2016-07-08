//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_features_h
#define gui_controls_features_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for feature controls.
class gui_controls_features : public QWidget
{
  Q_OBJECT

public:

  gui_controls_features(QWidget* parent = NULL);
  virtual ~gui_controls_features();

public slots:

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

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
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

    t_widgets() : pCheckSolidAngles    (NULL),
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
                  pUnperiodizeSelected (NULL)
    {}

    void Release()
    {
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
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
