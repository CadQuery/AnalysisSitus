//-----------------------------------------------------------------------------
// Created on: 25 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_DialogCapture_h
#define exeCC_DialogCapture_h

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for configuring contour capture tool.
class exeCC_DialogCapture : public QDialog
{
  Q_OBJECT

public:

  exeCC_DialogCapture(QWidget* parent = NULL);

  virtual ~exeCC_DialogCapture();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pPerform;     //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pPrecision;   //!< Precision to use.
    QCheckBox*      pHealContour; //!< Whether to heal contour automatically.
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
