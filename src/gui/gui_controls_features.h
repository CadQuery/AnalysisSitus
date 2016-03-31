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

  void onCheckSolidAngles   ();
  void onFindConvexOnly     ();
  void onFindSlots          ();
  void onFindSameHosts      ();
  void onRehostFaces        ();
  //
  void onDetachSelected     ();
  void onDeleteSelected     ();
  void onDeleteSelectedFull ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pCheckSolidAngles;   //!< Checks solid angles on convexity / concavity.
    QPushButton* pFindConvexOnly;     //!< Finds convex-only faces.
    QPushButton* pFindSlots;          //!< Finds slot features.
    QPushButton* pFindSameHosts;      //!< Attempts to find same-host faces.
    QPushButton* pRehostFaces;        //!< Merges the host geometry of selected faces.
    //
    QPushButton* pDetachSelected;     //!< Detaches selected faces from B-Rep.
    QPushButton* pDeleteSelected;     //!< Deletes selected faces (only).
    QPushButton* pDeleteSelectedFull; //!< Deletes selected faces with all contours.

    t_widgets() : pCheckSolidAngles   (NULL),
                  pFindConvexOnly     (NULL),
                  pFindSlots          (NULL),
                  pFindSameHosts      (NULL),
                  pRehostFaces        (NULL),
                  //
                  pDetachSelected     (NULL),
                  pDeleteSelected     (NULL),
                  pDeleteSelectedFull (NULL)
    {}

    void Release()
    {
      delete pCheckSolidAngles;   pCheckSolidAngles   = NULL;
      delete pFindConvexOnly;     pFindConvexOnly     = NULL;
      delete pFindSlots;          pFindSlots          = NULL;
      delete pFindSameHosts;      pFindSameHosts      = NULL;
      delete pRehostFaces;        pRehostFaces        = NULL;
      //
      delete pDetachSelected;     pDetachSelected     = NULL;
      delete pDeleteSelected;     pDeleteSelected     = NULL;
      delete pDeleteSelectedFull; pDeleteSelectedFull = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
