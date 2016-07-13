//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_detect_holes_h
#define gui_dialog_detect_holes_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_line_edit.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for configuring holes identification.
class gui_dialog_detect_holes : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_detect_holes(QWidget* parent = NULL);

  virtual ~gui_dialog_detect_holes();

public slots:

  void onIdentify();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pIdentify;          //!< Identify features.
  //---------------------------------------------------------------------------
    gui_line_edit* pRadius;            //!< Radius of interest.
    QCheckBox*     pIsHardFeatureMode; //!< Hard/soft feature detection mode.
  //---------------------------------------------------------------------------

    t_widgets() : pIdentify          (NULL),
                  pRadius            (NULL),
                  pIsHardFeatureMode (NULL)
    {}

    void Release()
    {
      delete pIdentify;          pIdentify          = NULL;
      delete pRadius;            pRadius            = NULL;
      delete pIsHardFeatureMode; pIsHardFeatureMode = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
