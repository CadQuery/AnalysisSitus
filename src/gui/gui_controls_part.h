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

  void onLoadBRep           ();
  void onLoadSTEP           ();
  void onSavePly            ();
  void onSaveBRep           ();
  //
  void onShowTOPOGraph      ();
  void onShowAAG            ();
  void onElimSelected       ();
  void onCheckShape         ();
  void onSewing             ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;      //!< Button for brep loading.
    QPushButton* pLoadSTEP;      //!< Button for STEP loading.
    QPushButton* pSavePly;       //!< Save triangulation to ply.
    QPushButton* pSaveBRep;      //!< Save shape to brep.
    //
    QPushButton* pShowTOPOGraph; //!< Show topology graph.
    QPushButton* pShowAAG;       //!< Show Attributed Adjacency Graph.
    QPushButton* pElimSelected;  //!< Removes convex-only faces from AAG.
    QPushButton* pCheckShape;    //!< Runs shape validity checker.
    QPushButton* pSewing;        //!< Performs sewing.

    t_widgets() : pLoadBRep      (NULL),
                  pLoadSTEP      (NULL),
                  pSavePly       (NULL),
                  pSaveBRep      (NULL),
                  //
                  pShowTOPOGraph (NULL),
                  pShowAAG       (NULL),
                  pElimSelected  (NULL),
                  pCheckShape    (NULL),
                  pSewing        (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;      pLoadBRep      = NULL;
      delete pLoadSTEP;      pLoadSTEP      = NULL;
      delete pSavePly;       pSavePly       = NULL;
      delete pSaveBRep;      pSaveBRep      = NULL;
      //
      delete pShowTOPOGraph; pShowTOPOGraph = NULL;
      delete pShowAAG;       pShowAAG       = NULL;
      delete pElimSelected;  pElimSelected  = NULL;
      delete pCheckShape;    pCheckShape    = NULL;
      delete pSewing;        pSewing        = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
