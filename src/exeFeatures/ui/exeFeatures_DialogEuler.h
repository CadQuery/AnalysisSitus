//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_DialogEuler_h
#define exeFeatures_DialogEuler_h

// A-Situs includes
#include <asiUI.h>

// Engine includes
#include <asiEngine_Model.h>

// GUI includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for calculation of Euler characteristics.
class exeFeatures_DialogEuler : public QDialog
{
  Q_OBJECT

public:

  exeFeatures_DialogEuler(const Handle(asiEngine_Model)& model,
                          QWidget*                       parent = NULL);

  virtual ~exeFeatures_DialogEuler();

public slots:

  void onCheck();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pCheck; //!< Check Euler-Poincare formula.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pGenus; //!< Genus of the solid model.
  //---------------------------------------------------------------------------

    t_widgets() : pCheck (NULL),
                  pGenus (NULL)
    {}

    void Release()
    {
      delete pCheck; pCheck = NULL;
      delete pGenus; pGenus = NULL;
    }
  };

  t_widgets               m_widgets;     //!< UI controls.
  QVBoxLayout*            m_pMainLayout; //!< Layout of the widget.
  Handle(asiEngine_Model) m_model;       //!< Data Model instance.

};

#endif
