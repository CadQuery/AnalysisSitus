//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
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

  void onLoadPly();
  void onLoadBRep();
  void onLoadSTEP();
  void onShowGraph();

private:

  QString selectPlyFile()  const;
  QString selectBRepFile() const;
  QString selectSTEPFile() const;

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadPly;   //!< Button for ply loading.
    QPushButton* pLoadBRep;  //!< Button for brep loading.
    QPushButton* pLoadSTEP;  //!< Button for STEP loading.
    QPushButton* pShowGraph; //!< Show topology graph.

    t_widgets() : pLoadPly(NULL),
                  pLoadBRep(NULL),
                  pLoadSTEP(NULL),
                  pShowGraph(NULL)
    {}

    void Release()
    {
      delete pLoadPly;   pLoadPly   = NULL;
      delete pLoadBRep;  pLoadBRep  = NULL;
      delete pLoadSTEP;  pLoadSTEP  = NULL;
      delete pShowGraph; pShowGraph = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
