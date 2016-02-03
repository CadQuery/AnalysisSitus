//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_controls_dmu_h
#define gui_controls_dmu_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for main controls in dMU viewer.
class gui_controls_dmu : public QWidget
{
  Q_OBJECT

public:

  gui_controls_dmu(QWidget* parent = NULL);
  virtual ~gui_controls_dmu();

public slots:

  void onLoadPly();
  //void onSavePly();
  void onDecimate();

private:

  QString selectPlyFile() const;

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
