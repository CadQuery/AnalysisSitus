//-----------------------------------------------------------------------------
// Created on: 19 June 2016 (nice airport in Munich)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_DialogCloudify_h
#define exeAsBuilt_DialogCloudify_h

// exeAsBuilt includes
#include <exeAsBuilt_Model.h>

// asiUI includes
#include <asiUI_LineEdit.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for cloudification (that is, conversion from B-Rep to point cloud).
class exeAsBuilt_DialogCloudify : public QDialog
{
  Q_OBJECT

public:

  exeAsBuilt_DialogCloudify(const Handle(exeAsBuilt_Model)&         model,
                            const Handle(ActAPI_IProgressNotifier)& notifier,
                            const Handle(ActAPI_IPlotter)&          plotter,
                            QWidget*                                parent = NULL);

  virtual ~exeAsBuilt_DialogCloudify();

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

  t_widgets                        m_widgets;     //!< UI controls.
  QVBoxLayout*                     m_pMainLayout; //!< Layout of the widget.
  Handle(exeAsBuilt_Model)         m_model;       //!< Data Model instance.
  Handle(ActAPI_IProgressNotifier) m_progress;    //!< Progress entry.
  Handle(ActAPI_IPlotter)          m_plotter;     //!< Plotter entry.

};

#endif
