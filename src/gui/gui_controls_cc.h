//-----------------------------------------------------------------------------
// Created on: 16 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_cc_h
#define gui_controls_cc_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for contour capture controls.
class gui_controls_cc : public QWidget
{
  Q_OBJECT

public:

  gui_controls_cc(QWidget* parent = NULL);
  virtual ~gui_controls_cc();

public slots:

  void onPickContour ();
  void onBVH_SAH     ();
  void onBVH_Linear  ();
  void onPickFacet   ();
  void onLocateFaces ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pPickContour; //!< Allows selection of a contour.
    QPushButton* pBVH_SAH;     //!< Builds BVH with SAH algorithm.
    QPushButton* pBVH_Linear;  //!< Builds BVH with linear algorithm.
    QPushButton* pPickFacet;   //!< Allows to pick up a single facet.
    QPushButton* pLocateFaces; //!< Finds faces corresponding the the contour's nodes.

    t_widgets() : pPickContour (NULL),
                  pBVH_SAH     (NULL),
                  pBVH_Linear  (NULL),
                  pPickFacet   (NULL),
                  pLocateFaces (NULL)
    {}

    void Release()
    {
      delete pPickContour; pPickContour = NULL;
      delete pBVH_SAH;     pBVH_SAH     = NULL;
      delete pBVH_Linear;  pBVH_Linear  = NULL;
      delete pPickFacet;   pPickFacet   = NULL;
      delete pLocateFaces; pLocateFaces = NULL;
    }
  };

  t_widgets m_widgets;      //!< Involved widgets.
  int       m_iPrevSelMode; //!< Previous selection mode.

};

#endif
