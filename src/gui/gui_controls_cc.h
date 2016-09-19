//-----------------------------------------------------------------------------
// Created on: 16 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_cc_h
#define gui_controls_cc_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for contour capture controls.
class gui_controls_cc : public QWidget
{
  Q_OBJECT

public:

  gui_controls_cc(QWidget* parent = NULL);
  virtual ~gui_controls_cc();

public slots:

  void onPickContour();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pPickContour; //!< Allows selection of a contour.

    t_widgets() : pPickContour (NULL)
    {}

    void Release()
    {
      delete pPickContour; pPickContour = NULL;
    }
  };

  t_widgets m_widgets;      //!< Involved widgets.
  int       m_iPrevSelMode; //!< Previous selection mode.

};

#endif
