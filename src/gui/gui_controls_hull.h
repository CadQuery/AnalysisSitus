//-----------------------------------------------------------------------------
// Created on: 03 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_hull_h
#define gui_controls_hull_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#include <QPushButton>
#pragma warning(pop)

//! Controls for hull reconstruction.
class gui_controls_hull : public QWidget
{
  Q_OBJECT

public:

  gui_controls_hull(QWidget* parent = NULL);
  virtual ~gui_controls_hull();

public slots:

  void onLoadPoints     ();
  void onInterpSections ();
  void onInterpColumns  ();
  void onSkinSurface    ();
  void onSaveSTEP       ();
  void onGaussCurvature ();

private:

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadPoints;     //!< Load points from file.
    QPushButton* pInterpSections; //!< Interpolate sections.
    QPushButton* pInterpColumns;  //!< Interpolate columns.
    QPushButton* pSkinSurface;    //!< Skin surface.
    QPushButton* pSaveSTEP;       //!< Save geometry to STEP file.
    QPushButton* pGaussCurvature; //!< Shown Gauss curvature field.

    //! ctor.
    t_widgets() : pLoadPoints     (NULL),
                  pInterpSections (NULL),
                  pInterpColumns  (NULL),
                  pSkinSurface    (NULL),
                  pSaveSTEP       (NULL),
                  pGaussCurvature (NULL)
    {}

    void Release()
    {
      delete pLoadPoints;     pLoadPoints     = NULL;
      delete pInterpSections; pInterpSections = NULL;
      delete pInterpColumns;  pInterpColumns  = NULL;
      delete pSkinSurface;    pSkinSurface    = NULL;
      delete pSaveSTEP;       pSaveSTEP       = NULL;
      delete pGaussCurvature; pGaussCurvature = NULL;
    }
  };

  //---------------------------------------------------------------------------

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.
  t_widgets    m_widgets;     //!< Involved widgets.

};

#endif
