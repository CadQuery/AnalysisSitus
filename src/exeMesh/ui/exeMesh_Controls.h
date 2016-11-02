//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ControlsMesh_h
#define asiUI_ControlsMesh_h

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for main controls in Mesh viewer.
class asiUI_ControlsMesh : public QWidget
{
  Q_OBJECT

public:

  asiUI_ControlsMesh(QWidget* parent = NULL);
  virtual ~asiUI_ControlsMesh();

public slots:

  void onLoadPly();
  //void onSavePly();
  void onDecimate();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadPly;  //!< Button for ply loading.
    QPushButton* pSavePly;  //!< Button for ply saving.
    QPushButton* pDecimate; //!< Button to run decimation.

    t_widgets() : pLoadPly  (NULL),
                  pSavePly  (NULL),
                  pDecimate (NULL)
    {}

    void Release()
    {
      delete pLoadPly;  pLoadPly  = NULL;
      delete pSavePly;  pSavePly  = NULL;
      delete pDecimate; pDecimate = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
