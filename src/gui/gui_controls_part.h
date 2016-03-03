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

  void onLoadBRep      ();
  void onLoadSTEP      ();
  void onShowTOPOGraph ();
  void onShowAAG       ();
  void onSavePly       ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;      //!< Button for brep loading.
    QPushButton* pLoadSTEP;      //!< Button for STEP loading.
    QPushButton* pShowTOPOGraph; //!< Show topology graph.
    QPushButton* pShowAAG;       //!< Show Attributed Adjacency Graph.
    QPushButton* pSavePly;       //!< Save triangulation to ply.

    t_widgets() : pLoadBRep      (NULL),
                  pLoadSTEP      (NULL),
                  pShowTOPOGraph (NULL),
                  pShowAAG       (NULL),
                  pSavePly       (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;      pLoadBRep      = NULL;
      delete pLoadSTEP;      pLoadSTEP      = NULL;
      delete pShowTOPOGraph; pShowTOPOGraph = NULL;
      delete pShowAAG;       pShowAAG       = NULL;
      delete pSavePly;       pSavePly       = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
