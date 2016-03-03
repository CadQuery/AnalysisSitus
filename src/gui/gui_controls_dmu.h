//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_dmu_h
#define gui_controls_dmu_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for main controls in dMU viewer.
class gui_controls_dmu : public QWidget
{
  Q_OBJECT

public:

  gui_controls_dmu(QWidget* parent = NULL);
  virtual ~gui_controls_dmu();

public slots:

  void onLoadIGES      ();
  void onLoadSTEP      ();
  void onSewing        ();
  void onMergeFaces    ();
  void onIdentifyHoles ();
  void onRemoveHoles   ();
  void onMesh          ();
  void onSaveXBF       ();
  void onSaveBRep      ();
  void onShowTopoGraph ();
  void onShowAAG       ();
  void onShowOCAF      ();

private:

  void adjustUI();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    //-------------------------------------------------------------------------
    // IMPORT
    //-------------------------------------------------------------------------
    QPushButton* pLoadIGES;      //!< Button for IGES import.
    QPushButton* pLoadSTEP;      //!< Button for STEP import.
    //
    //-------------------------------------------------------------------------
    // GEOMETRY
    //-------------------------------------------------------------------------
    QToolButton* pSewing;        //!< Performs sewing.
    QToolButton* pMergeFaces;    //!< Performs face merging.
    QToolButton* pIdentifyHoles; //!< Performs holes identification.
    QToolButton* pRemoveHoles;   //!< Performs holes suppression.
    //
    //-------------------------------------------------------------------------
    // MESH
    //-------------------------------------------------------------------------
    QToolButton* pMesh;          //!< Performs meshing.
    //
    //-------------------------------------------------------------------------
    // EXPORT
    //-------------------------------------------------------------------------
    QPushButton* pSaveXBF;       //!< Saves model as binary XDE.
    QPushButton* pSaveBRep;      //!< Saves geometry (only) to B-Rep format.
    //
    //-------------------------------------------------------------------------
    // ANALYSIS
    //-------------------------------------------------------------------------
    QToolButton* pShowTopoGraph; //!< Shows topological graph.
    QToolButton* pShowAAG;       //!< Shows AAG.
    QToolButton* pShowOCAF;      //!< Shown internal OCAF.

    t_widgets() : pLoadIGES      (NULL),
                  pLoadSTEP      (NULL),
                  pSewing        (NULL),
                  pMergeFaces    (NULL),
                  pIdentifyHoles (NULL),
                  pRemoveHoles   (NULL),
                  pMesh          (NULL),
                  pSaveXBF       (NULL),
                  pShowTopoGraph (NULL),
                  pShowAAG       (NULL),
                  pShowOCAF      (NULL)
    {}

    void Release()
    {
      delete pLoadIGES;      pLoadIGES      = NULL;
      delete pLoadSTEP;      pLoadSTEP      = NULL;
      delete pSewing;        pSewing        = NULL;
      delete pMergeFaces;    pMergeFaces    = NULL;
      delete pIdentifyHoles; pIdentifyHoles = NULL;
      delete pRemoveHoles;   pRemoveHoles   = NULL;
      delete pMesh;          pMesh          = NULL;
      delete pSaveXBF;       pSaveXBF       = NULL;
      delete pShowTopoGraph; pShowTopoGraph = NULL;
      delete pShowAAG;       pShowAAG       = NULL;
      delete pShowOCAF;      pShowOCAF      = NULL;
    }
  };

  t_widgets m_widgets;  //!< Involved widgets.
};

#endif
