//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_tess_h
#define gui_controls_tess_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for main controls in Mesh viewer.
class gui_controls_mesh : public QWidget
{
  Q_OBJECT

public:

  gui_controls_mesh(QWidget* parent = NULL);
  virtual ~gui_controls_mesh();

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
