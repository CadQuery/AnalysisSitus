//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_sewing_h
#define gui_dialog_sewing_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_line_edit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#pragma warning(push, 0)
#include <AIS_Shape.hxx>
#pragma warning(pop)

//! Dialog for sewing.
class gui_dialog_sewing : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_sewing(QWidget* parent = NULL);

  virtual ~gui_dialog_sewing();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
    //! Editor IDs.
    enum editor
    {
      edit_Tolerance = 0,
      //
      LastEditor
    };

  //---------------------------------------------------------------------------
    QPushButton*   pPerform;   //!< Perform sewing.
  //---------------------------------------------------------------------------
    gui_line_edit* pTolerance; //!< Sewing tolerance.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform   (NULL),
                  pTolerance (NULL)
    {}

    void Release()
    {
      delete pPerform;   pPerform    = NULL;
      delete pTolerance; pTolerance  = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
