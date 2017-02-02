//-----------------------------------------------------------------------------
// Created on: 02 February 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogSmoothAngleTol_h
#define asiUI_DialogSmoothAngleTol_h

// asiUI includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for specification of smooth angle tolerance.
class asiUI_DialogSmoothAngleTol : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogSmoothAngleTol(QWidget* parent = NULL);

  virtual ~asiUI_DialogSmoothAngleTol();

public slots:

  void onPerform();

public:

  double Tolerance; //!< User-specified angular tolerance.

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
    QPushButton*    pPerform;   //!< Perform sewing.
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
