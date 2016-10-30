//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_euler_h
#define gui_dialog_euler_h

// A-Situs includes
#include <analysis_situs.h>

// GUI includes
#include <gui_line_edit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for calculation of Euler characteristics.
class gui_dialog_euler : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_euler(QWidget* parent = NULL);

  virtual ~gui_dialog_euler();

public slots:

  void onCheck();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pCheck; //!< Check Euler-Poincare formula.
  //---------------------------------------------------------------------------
    gui_line_edit* pGenus; //!< Genus of the solid model.
  //---------------------------------------------------------------------------

    t_widgets() : pCheck (NULL),
                  pGenus (NULL)
    {}

    void Release()
    {
      delete pCheck; pCheck = NULL;
      delete pGenus; pGenus = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
