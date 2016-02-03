//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
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

  void onUBend    ();
  void onDMU      ();
  void onMeshEdit ();
  void onAnalysis ();
  void onSkinning ();

private:

  void createControls();

private:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QWidget*     pMainPane; //!< Central widget.
  //---------------------------------------------------------------------------
    QPushButton* pDMU;      //!< dMU composition.
    QPushButton* pMeshEdit; //!< Mesh Editor composition.
    QPushButton* pAnalysis; //!< Analysis composition.
    QPushButton* pSkinning; //!< Skinning composition.
    QPushButton* pUBend;    //!< U-bend composition.
  //---------------------------------------------------------------------------
    t_widgets() : pMainPane (NULL),
                  pDMU      (NULL),
                  pMeshEdit (NULL),
                  pAnalysis (NULL),
                  pSkinning (NULL),
                  pUBend    (NULL)
    {}

    void Release()
    {
      delete pUBend;    pUBend    = NULL;
      delete pDMU;      pDMU      = NULL;
      delete pMeshEdit; pMeshEdit = NULL;
      delete pAnalysis; pAnalysis = NULL;
      delete pSkinning; pSkinning = NULL;
      delete pMainPane; pMainPane = NULL;
    }
  };

  t_widgets m_widgets; //!< Widgets composing the welcome screen.

};

#endif
