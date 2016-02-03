//-----------------------------------------------------------------------------
// Created on: 02 February 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_controls_ubend_h
#define gui_controls_ubend_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#include <QPushButton>
#pragma warning(pop)

//! U-bend controls.
class gui_controls_ubend : public QWidget
{
  Q_OBJECT

public:

  gui_controls_ubend(QWidget* parent = NULL);
  virtual ~gui_controls_ubend();

public slots:

  void onDoTheMagic();

private:

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pBuildUBend; //!< Build u-bend.

    t_widgets() : pBuildUBend(NULL) {} //!< ctor.

    void Release()
    {
      delete pBuildUBend; pBuildUBend = NULL;
    }
  };

  //---------------------------------------------------------------------------

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.
  t_widgets    m_widgets;     //!< Involved widgets.

};

#endif
