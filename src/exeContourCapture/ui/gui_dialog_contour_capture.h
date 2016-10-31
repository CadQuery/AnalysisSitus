//-----------------------------------------------------------------------------
// Created on: 25 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_contour_capture_h
#define gui_dialog_contour_capture_h

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

//! Dialog for configuring contour capture tool.
class gui_dialog_contour_capture : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_contour_capture(QWidget* parent = NULL);

  virtual ~gui_dialog_contour_capture();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pPerform;     //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    gui_line_edit* pPrecision;   //!< Precision to use.
    QCheckBox*     pHealContour; //!< Whether to heal contour automatically.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform     (NULL),
                  pPrecision   (NULL),
                  pHealContour (NULL)
    {}

    void Release()
    {
      delete pPerform;     pPerform     = NULL;
      delete pPrecision;   pPrecision   = NULL;
      delete pHealContour; pHealContour = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
