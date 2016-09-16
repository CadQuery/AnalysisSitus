//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_welcome_window_h
#define gui_welcome_window_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#include <QPushButton>
#pragma warning(pop)

//! Welcome window.
class gui_welcome_window : public QMainWindow
{
  Q_OBJECT

public:

  gui_welcome_window();
  virtual ~gui_welcome_window();

public slots:

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------
  void onDMU        ();
  void onMeshEdit   ();
  void onAnalysis   ();
  void onCAFBrowser ();
  void onPointCloud ();

  //---------------------------------------------------------------------------
  // Features
  //---------------------------------------------------------------------------
  void onFeatures   ();
  void onMorphology ();

  //---------------------------------------------------------------------------
  // Samples (skinning)
  //---------------------------------------------------------------------------
  void onSkinning        ();
  void onUBend           ();
  void onHullReconstruct ();
  void onContourCapture  ();

private:

  void createControls();

private:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QWidget*     pMainPane;        //!< Central widget.
  //---------------------------------------------------------------------------
    QPushButton* pDMU;             //!< dMU composition.
    QPushButton* pMeshEdit;        //!< Mesh Editor composition.
    QPushButton* pAnalysis;        //!< Analysis composition.
    QPushButton* pCAFBrowser;      //!< CAF Browser composition.
    QPushButton* pPointCloud;      //!< Point Cloud composition.
  //-------------------------------------------------------------------------
    QPushButton* pFeatures;        //!< Features composition.
    QPushButton* pMorphology;      //!< Shape morphology.
  //-------------------------------------------------------------------------
    QPushButton* pSkinning;        //!< Skinning composition.
    QPushButton* pUBend;           //!< U-bend composition.
    QPushButton* pHullReconstruct; //!< Hull reconstruction composition.
    QPushButton* pContourCapture;  //!< Contour capture composition.
  //---------------------------------------------------------------------------
    t_widgets() : pMainPane        (NULL),
                  pDMU             (NULL),
                  pMeshEdit        (NULL),
                  pAnalysis        (NULL),
                  pCAFBrowser      (NULL),
                  pPointCloud      (NULL),
                  //
                  pFeatures        (NULL),
                  pMorphology      (NULL),
                  //
                  pSkinning        (NULL),
                  pUBend           (NULL),
                  pHullReconstruct (NULL),
                  pContourCapture  (NULL)
    {}

    //! Kills all widgets.
    void Release()
    {
      delete pDMU;             pDMU             = NULL;
      delete pMeshEdit;        pMeshEdit        = NULL;
      delete pAnalysis;        pAnalysis        = NULL;
      delete pCAFBrowser;      pCAFBrowser      = NULL;
      delete pPointCloud;      pPointCloud      = NULL;
      //
      delete pFeatures;        pFeatures        = NULL;
      delete pMorphology;      pMorphology      = NULL;
      //
      delete pSkinning;        pSkinning        = NULL;
      delete pUBend;           pUBend           = NULL;
      delete pHullReconstruct; pHullReconstruct = NULL;
      delete pContourCapture;  pContourCapture  = NULL;
      //
      delete pMainPane;        pMainPane        = NULL;
    }
  };

  t_widgets m_widgets; //!< Widgets composing the welcome screen.

};

#endif
