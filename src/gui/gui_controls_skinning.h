//-----------------------------------------------------------------------------
// Created on: 11 (* thanks to my mother, Irina) December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_controls_skinning_h
#define gui_controls_skinning_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#include <QPushButton>
#pragma warning(pop)

//! Skinning controls.
class gui_controls_skinning : public QWidget
{
  Q_OBJECT

public:

  gui_controls_skinning(QWidget* parent = NULL);
  virtual ~gui_controls_skinning();

public slots:

  void onDoTheMagic();

private:

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pDoSkinning; //!< Run skinning.

    t_widgets() : pDoSkinning(NULL) {} //!< ctor.

    void Release()
    {
      delete pDoSkinning; pDoSkinning = NULL;
    }
  };

  //---------------------------------------------------------------------------

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.
  t_widgets    m_widgets;     //!< Involved widgets.

};

#endif
