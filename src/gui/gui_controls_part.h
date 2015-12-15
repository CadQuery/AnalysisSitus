//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
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

  void onLoadBRep();
  void onLoadSTEP();
  void onShowGraph();
  void onSavePly();

private:

  QString selectBRepFile() const;
  QString selectSTEPFile() const;
  QString selectPlyFile()  const;

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadBRep;  //!< Button for brep loading.
    QPushButton* pLoadSTEP;  //!< Button for STEP loading.
    QPushButton* pShowGraph; //!< Show topology graph.
    QPushButton* pSavePly;   //!< Save triangulation to ply.

    t_widgets() : pLoadBRep  (NULL),
                  pLoadSTEP  (NULL),
                  pShowGraph (NULL),
                  pSavePly   (NULL)
    {}

    void Release()
    {
      delete pLoadBRep;  pLoadBRep  = NULL;
      delete pLoadSTEP;  pLoadSTEP  = NULL;
      delete pShowGraph; pShowGraph = NULL;
      delete pSavePly;   pSavePly   = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
