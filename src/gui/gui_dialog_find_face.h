//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_find_face_h
#define gui_dialog_find_face_h

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

//! Dialog for finding face of interest.
class gui_dialog_find_face : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_find_face(QWidget* parent = NULL);

  virtual ~gui_dialog_find_face();

public slots:

  void onFind();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pFind;  //!< Find face of interest.
  //---------------------------------------------------------------------------
    gui_line_edit* pIndex; //!< Face index.
  //---------------------------------------------------------------------------

    t_widgets() : pFind (NULL),
                  pIndex(NULL)
    {}

    void Release()
    {
      delete pFind;  pFind  = NULL;
      delete pIndex; pIndex = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
