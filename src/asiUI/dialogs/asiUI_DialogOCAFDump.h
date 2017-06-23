//-----------------------------------------------------------------------------
// Created on: 23 June 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogOCAFDump_h
#define asiUI_DialogOCAFDump_h

// asiUI includes
#include <asiUI.h>

// Active Data includes
#include <ActAPI_IModel.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#pragma warning(pop)

//! Text view for OCAF dumps.
class asiUI_DialogOCAFDump : public QDialog
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_DialogOCAFDump(const Handle(ActAPI_IModel)&            model,
                         const Handle(ActAPI_IProgressNotifier)& notifier,
                         QWidget*                                parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_DialogOCAFDump();

public slots:

  void onClose();

protected:

  void initialize();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QTextEdit*    pEditor; //!< Text editor.
    QPushButton*  pClose;  //!< Close button.

    t_widgets() : pEditor (NULL),
                  pClose  (NULL)
    {}

    void Release()
    {
      delete pEditor; pEditor = NULL;
      delete pClose;  pClose = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

private:

  Handle(ActAPI_IModel) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry  m_notifier; //!< Progress Notifier.

};

#endif
