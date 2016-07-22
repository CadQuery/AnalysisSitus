//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_euler_h
#define gui_controls_euler_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for Euler operators.
class gui_controls_euler : public QWidget
{
  Q_OBJECT

public:

  gui_controls_euler(QWidget* parent = NULL);
  virtual ~gui_controls_euler();

public slots:

  void onCheckEulerPoincare();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pCheckEulerPoincare; //!< Checks Euler-Poincare equation.
    QPushButton* pKEV;
    QPushButton* pKEF;

    t_widgets() : pCheckEulerPoincare (NULL),
                  pKEV                (NULL),
                  pKEF                (NULL)
    {}

    void Release()
    {
      delete pCheckEulerPoincare; pCheckEulerPoincare = NULL;
      delete pKEV;                pKEV                = NULL;
      delete pKEF;                pKEF                = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
