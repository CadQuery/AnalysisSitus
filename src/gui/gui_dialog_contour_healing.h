//-----------------------------------------------------------------------------
// Created on: 26 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_contour_healing_h
#define gui_dialog_contour_healing_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_line_edit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for configuring contour healing tool.
class gui_dialog_contour_healing : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_contour_healing(QWidget* parent = NULL);

  virtual ~gui_dialog_contour_healing();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pPerform;   //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    gui_line_edit* pPrecision; //!< Precision to use.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform   (NULL),
                  pPrecision (NULL)
    {}

    void Release()
    {
      delete pPerform;   pPerform   = NULL;
      delete pPrecision; pPrecision = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
