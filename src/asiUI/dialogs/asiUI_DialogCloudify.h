//-----------------------------------------------------------------------------
// Created on: 19 June 2016 (nice airport in Munich)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogCloudify_h
#define asiUI_DialogCloudify_h

// A-Situs includes
#include <asiUI_Dialog.h>

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for cloudification (that is, conversion from B-Rep to point cloud).
class asiUI_DialogCloudify : public asiUI_Dialog
{
  Q_OBJECT

public:

  asiUI_DialogCloudify(ActAPI_ProgressEntry notifier,
                       ActAPI_PlotterEntry  plotter,
                       QWidget*             parent = NULL);

  virtual ~asiUI_DialogCloudify();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pPerform;    //!< Perform sewing.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pLinearStep; //!< Digitizing step.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform    (NULL),
                  pLinearStep (NULL)
    {}

    void Release()
    {
      delete pPerform;    pPerform    = NULL;
      delete pLinearStep; pLinearStep = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
