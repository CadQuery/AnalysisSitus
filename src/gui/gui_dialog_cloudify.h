//-----------------------------------------------------------------------------
// Created on: 19 June 2016 (nice airport in Munich)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_cloudify_h
#define gui_dialog_cloudify_h

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

//! Dialog for cloudification (that is, conversion from B-Rep to point cloud).
class gui_dialog_cloudify : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_cloudify(QWidget* parent = NULL);

  virtual ~gui_dialog_cloudify();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pPerform;    //!< Perform sewing.
  //---------------------------------------------------------------------------
    gui_line_edit* pLinearStep; //!< Digitizing step.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform    (NULL),
                  pLinearStep (NULL)
    {}

    void Release()
    {
      delete pPerform;    pPerform     = NULL;
      delete pLinearStep; pLinearStep  = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
