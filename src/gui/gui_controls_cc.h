//-----------------------------------------------------------------------------
// Created on: 16 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_cc_h
#define gui_controls_cc_h

// Geometry includes
#include <geom_bvh_facets.h>

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

  void onPickContour         ();
  void onLoadContour         ();
  //
  void onCheckVertexDistance ();
  void onProjectVertices     ();
  //
  void onCapture             ();
  void onValidateResult      ();
  //
  void onBVH_SAH             ();
  void onBVH_Linear          ();
  void onPickFacet           ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pPickContour;         //!< Allows selection of a contour.
    QPushButton* pLoadContour;         //!< Allows loading a contour from file.
    //
    QPushButton* pCheckVertexDistance; //!< Checks distance from the contour's vertices to a part.
    QPushButton* pProjectVertices;     //!< Projects vertices of the contour to the exact body.
    //
    QPushButton* pCapture;             //!< Performs contour capturing.
    QPushButton* pValidateResult;      //!< Validates capture result.
    //
    QPushButton* pBVH_SAH;             //!< Builds BVH with SAH algorithm.
    QPushButton* pBVH_Linear;          //!< Builds BVH with linear algorithm.
    QPushButton* pPickFacet;           //!< Allows to pick up a single facet.

    t_widgets() : pPickContour         (NULL),
                  pLoadContour         (NULL),
                  //
                  pCheckVertexDistance (NULL),
                  pProjectVertices     (NULL),
                  //
                  pCapture             (NULL),
                  pValidateResult      (NULL),
                  //
                  pBVH_SAH             (NULL),
                  pBVH_Linear          (NULL),
                  pPickFacet           (NULL)
    {}

    void Release()
    {
      delete pPickContour;         pPickContour         = NULL;
      delete pLoadContour;         pLoadContour         = NULL;
      //
      delete pCheckVertexDistance; pCheckVertexDistance = NULL;
      delete pProjectVertices;     pProjectVertices     = NULL;
      //
      delete pCapture;             pCapture             = NULL;
      delete pValidateResult;      pValidateResult      = NULL;
      //
      delete pBVH_SAH;             pBVH_SAH             = NULL;
      delete pBVH_Linear;          pBVH_Linear          = NULL;
      delete pPickFacet;           pPickFacet           = NULL;
    }
  };

  t_widgets               m_widgets;      //!< Involved widgets.
  int                     m_iPrevSelMode; //!< Previous selection mode.
  Handle(geom_bvh_facets) m_bvh;          //!< Accelerating structure for picking.

};

#endif
