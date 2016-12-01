//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_ControlsEuler_h
#define exeFeatures_ControlsEuler_h

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for Euler operators.
class exeFeatures_ControlsEuler : public QWidget
{
  Q_OBJECT

public:

  exeFeatures_ControlsEuler(QWidget* parent = NULL);
  virtual ~exeFeatures_ControlsEuler();

public slots:

  void onCheckEulerPoincare ();
  void onKEV                ();
  void onKEF                ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pCheckEulerPoincare; //!< Checks Euler-Poincare equation.
    QPushButton* pKEV;                //!< KEV operation.
    QPushButton* pKEF;                //!< KEF operation.

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
