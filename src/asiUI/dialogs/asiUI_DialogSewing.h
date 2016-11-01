//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogSewing_h
#define asiUI_DialogSewing_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

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
class asiUI_DialogSewing : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogSewing(QWidget* parent = NULL);

  virtual ~asiUI_DialogSewing();

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
    asiUI_LineEdit* pTolerance; //!< Sewing tolerance.
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
