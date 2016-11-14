//-----------------------------------------------------------------------------
// Created on: 26 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_DialogHealing_h
#define exeCC_DialogHealing_h

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for configuring contour healing tool.
class exeCC_DialogHealing : public QDialog
{
  Q_OBJECT

public:

  exeCC_DialogHealing(QWidget* parent = NULL);

  virtual ~exeCC_DialogHealing();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pPerform;   //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pPrecision; //!< Precision to use.
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
