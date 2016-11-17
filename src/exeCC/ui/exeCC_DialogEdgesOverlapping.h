//-----------------------------------------------------------------------------
// Created on: 26 October 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_DialogEdgesOverlapping_h
#define exeCC_DialogEdgesOverlapping_h

// asiUI includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for edges overlapping checker.
class exeCC_DialogEdgesOverlapping : public QDialog
{
  Q_OBJECT

public:

  exeCC_DialogEdgesOverlapping(QWidget* parent = NULL);

  virtual ~exeCC_DialogEdgesOverlapping();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pPerform;       //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pPrecision;     //!< Precision to use.
    asiUI_LineEdit* pMaxElevation;  //!< Max allowed elevation.
    asiUI_LineEdit* pMinLaidLength; //!< Min allowed laid length.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform       (NULL),
                  pPrecision     (NULL),
                  pMaxElevation  (NULL),
                  pMinLaidLength (NULL)
    {}

    void Release()
    {
      delete pPerform;       pPerform       = NULL;
      delete pPrecision;     pPrecision     = NULL;
      delete pMaxElevation;  pMaxElevation  = NULL;
      delete pMinLaidLength; pMinLaidLength = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
