//-----------------------------------------------------------------------------
// Created on: 16 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_Controls_h
#define exeCC_Controls_h

// Geometry includes
#include <asiAlgo_BVHFacets.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for contour capture controls.
class exeCC_Controls : public QWidget
{
  Q_OBJECT

public:

  exeCC_Controls(QWidget* parent = NULL);
  virtual ~exeCC_Controls();

public slots:

  void onPickContour          ();
  void onLoadContour          ();
  void onSaveContour          ();
  //
  void onCheckContourDistance ();
  void onCheckVertexDistance  ();
  void onHealedVSOriginal     ();
  void onProjectVertices      ();
  void onEnrichContour        ();
  void onHealContour          ();
  //
  void onCapture              ();
  void onValidateResult       ();
  //
  void onBVH_SAH              ();
  void onBVH_Linear           ();
  void onPickFacet            ();
  //
  void onBuildOffsets         ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pPickContour;          //!< Allows selection of a contour.
    QPushButton* pLoadContour;          //!< Allows loading a contour from file.
    QPushButton* pSaveContour;          //!< Saves contour to file.
    //
    QPushButton* pCheckContourDistance; //!< Checks distance from the contour to a part.
    QPushButton* pCheckVertexDistance;  //!< Checks distance from the contour's vertices to a part.
    QPushButton* pHealedVSOriginal;     //!< Checks healed contour wrt original one.
    QPushButton* pProjectVertices;      //!< Projects vertices of the contour to the exact body.
    QPushButton* pEnrichContour;        //!< Adds more points to the contour.
    QPushButton* pHealContour;          //!< Performs contour healing.
    //
    QPushButton* pCapture;              //!< Performs contour capturing.
    QPushButton* pValidateResult;       //!< Validates capture result.
    //
    QPushButton* pBVH_SAH;              //!< Builds BVH with SAH algorithm.
    QPushButton* pBVH_Linear;           //!< Builds BVH with linear algorithm.
    QPushButton* pPickFacet;            //!< Allows to pick up a single facet.
    //
    QPushButton* pBuildOffsets;         //!< Builds offset surfaces.

    t_widgets() : pPickContour          (NULL),
                  pLoadContour          (NULL),
                  pSaveContour          (NULL),
                  //
                  pCheckContourDistance (NULL),
                  pCheckVertexDistance  (NULL),
                  pHealedVSOriginal     (NULL),
                  pProjectVertices      (NULL),
                  pEnrichContour        (NULL),
                  pHealContour          (NULL),
                  //
                  pCapture              (NULL),
                  pValidateResult       (NULL),
                  //
                  pBVH_SAH              (NULL),
                  pBVH_Linear           (NULL),
                  pPickFacet            (NULL),
                  //
                  pBuildOffsets         (NULL)
    {}

    void Release()
    {
      delete pPickContour;          pPickContour         = NULL;
      delete pLoadContour;          pLoadContour         = NULL;
      delete pSaveContour;          pSaveContour         = NULL;
      //
      delete pCheckContourDistance; pCheckVertexDistance = NULL;
      delete pCheckVertexDistance;  pCheckVertexDistance = NULL;
      delete pHealedVSOriginal;     pHealedVSOriginal    = NULL;
      delete pProjectVertices;      pProjectVertices     = NULL;
      delete pEnrichContour;        pEnrichContour       = NULL;
      delete pHealContour;          pHealContour         = NULL;
      //
      delete pCapture;              pCapture             = NULL;
      delete pValidateResult;       pValidateResult      = NULL;
      //
      delete pBVH_SAH;              pBVH_SAH             = NULL;
      delete pBVH_Linear;           pBVH_Linear          = NULL;
      delete pPickFacet;            pPickFacet           = NULL;
      //
      delete pBuildOffsets;         pBuildOffsets        = NULL;
    }
  };

  t_widgets                 m_widgets;      //!< Involved widgets.
  int                       m_iPrevSelMode; //!< Previous selection mode.
  Handle(asiAlgo_BVHFacets) m_bvh;          //!< Accelerating structure for picking.

};

#endif
