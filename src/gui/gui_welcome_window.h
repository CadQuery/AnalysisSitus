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
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
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

  //---------------------------------------------------------------------------
  // Features
  //---------------------------------------------------------------------------
  void onFeatures ();

  //---------------------------------------------------------------------------
  // Samples (skinning)
  //---------------------------------------------------------------------------
  void onSkinning        ();
  void onUBend           ();
  void onHullReconstruct ();

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
    //-------------------------------------------------------------------------
    QPushButton* pFeatures;        //!< Features composition.
    //-------------------------------------------------------------------------
    QPushButton* pSkinning;        //!< Skinning composition.
    QPushButton* pUBend;           //!< U-bend composition.
    QPushButton* pHullReconstruct; //!< Hull reconstruction composition.
  //---------------------------------------------------------------------------
    t_widgets() : pMainPane        (NULL),
                  pDMU             (NULL),
                  pMeshEdit        (NULL),
                  pAnalysis        (NULL),
                  pCAFBrowser      (NULL),
                  //
                  pFeatures        (NULL),
                  //
                  pSkinning        (NULL),
                  pUBend           (NULL),
                  pHullReconstruct (NULL)
    {}

    //! Kills all widgets.
    void Release()
    {
      delete pDMU;             pDMU             = NULL;
      delete pMeshEdit;        pMeshEdit        = NULL;
      delete pAnalysis;        pAnalysis        = NULL;
      delete pCAFBrowser;      pCAFBrowser      = NULL;
      //
      delete pFeatures;        pFeatures        = NULL;
      //
      delete pSkinning;        pSkinning        = NULL;
      delete pUBend;           pUBend           = NULL;
      delete pHullReconstruct; pHullReconstruct = NULL;
      //
      delete pMainPane;        pMainPane        = NULL;
    }
  };

  t_widgets m_widgets; //!< Widgets composing the welcome screen.

};

#endif
