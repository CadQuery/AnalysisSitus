//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_morphology_h
#define gui_controls_morphology_h

// GUI includes
#include <gui_decomposition_browser.h>

// Geometry includes
#include <geom_volume_node.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for morphology controls.
class gui_controls_morphology : public QWidget
{
  Q_OBJECT

public:

  gui_controls_morphology(QWidget* parent = NULL);
  virtual ~gui_controls_morphology();

public slots:

  void onSetAsPart();
  //
  void onBuildMajorant();
  void onBuildMajorantOBB();
  void onBuildDelta();
  void onFindSeedFaces();
  void onFindExtrusions();
  void onClassifyFeatures();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    gui_decomposition_browser* wDecompBrowser;    //!< Features decomposition browser.
    //
    QPushButton*               pSetAsPart;        //!< Button to set selected item as part.
    QPushButton*               pBuildMajorant;    //!< Button to build a stock shape.
    QPushButton*               pBuildMajorantOBB; //!< Button to build a stock shape as OBB.
    QPushButton*               pBuildDelta;       //!< Button to build delta volume.
    QPushButton*               pFindSeedFaces;    //!< Button to find seed faces.
    QPushButton*               pFindExtrusions;   //!< Button to find extrusions.
    QPushButton*               pClassifyFeatures; //!< Button to classify features.

    t_widgets() : wDecompBrowser    (NULL),
                  pSetAsPart        (NULL),
                  pBuildMajorant    (NULL),
                  pBuildMajorantOBB (NULL),
                  pBuildDelta       (NULL),
                  pFindSeedFaces    (NULL),
                  pFindExtrusions   (NULL),
                  pClassifyFeatures (NULL)
    {}

    void Release()
    {
      delete pSetAsPart;        pSetAsPart        = NULL;
      delete wDecompBrowser;    wDecompBrowser    = NULL;
      delete pBuildMajorant;    pBuildMajorant    = NULL;
      delete pBuildMajorantOBB; pBuildMajorantOBB = NULL;
      delete pBuildDelta;       pBuildDelta       = NULL;
      delete pFindSeedFaces;    pFindSeedFaces    = NULL;
      delete pFindExtrusions;   pFindExtrusions   = NULL;
      delete pClassifyFeatures; pClassifyFeatures = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.
  //
  Handle(geom_volume_node) m_stock_node; //!< Stock volume.
  Handle(geom_volume_node) m_delta_node; //!< Delta volume.
  Handle(geom_volume_node) m_part_node;  //!< Part volume.
  //
  double m_fFuzzy; //!< Fuzzy tolerance for Boolean cut.

};

#endif
