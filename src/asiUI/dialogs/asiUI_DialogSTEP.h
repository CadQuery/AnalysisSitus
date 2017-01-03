//-----------------------------------------------------------------------------
// Created on: 16 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogSTEP_h
#define asiUI_DialogSTEP_h

// asiAlgo includes
#include <asiAlgo_Variable.h>

// asiData includes
#include <asiData_PartNode.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Sequence.hxx>

// Active Data includes
#include <ActAPI_IModel.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#pragma warning(pop)

//! Controls for STEP interoperability.
class asiUI_DialogSTEP : public QDialog
{
  Q_OBJECT

public:

  //! Exchange direction.
  enum Mode
  {
    Mode_Read,
    Mode_Write
  };

public:

  asiUI_DialogSTEP(const Handle(ActAPI_IModel)&    model,
                   const Handle(asiData_PartNode)& part_n,
                   const Mode                      mode,
                   ActAPI_ProgressEntry            notifier,
                   QWidget*                        parent = NULL);

  virtual ~asiUI_DialogSTEP();

public slots:

  void onVarChanged(QTableWidgetItem*);
  void onProceed();

protected:

  void initialize();
  void initialize_Read(NCollection_Sequence<asiAlgo_Variable>& vars);
  void initialize_Write(NCollection_Sequence<asiAlgo_Variable>& vars);
  void saveVars();
  void proceed_Write();
  void proceed_Read();

private:

  Mode         m_mode;        //!< Translation mode.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QTableWidget* pOptions; //!< Data exchange options.
    QPushButton*  pProceed; //!< Button to proceed.

    t_widgets() : pOptions (NULL),
                  pProceed (NULL)
    {}

    void Release()
    {
      delete pOptions; pOptions = NULL;
      delete pProceed; pProceed = NULL;
    }
  };

  t_widgets                m_widgets;  //!< Involved widgets.
  //
  Handle(ActAPI_IModel)    m_model;    //!< Data Model instance.
  Handle(asiData_PartNode) m_part;     //!< Part Node.
  ActAPI_ProgressEntry     m_notifier; //!< Progress Notifier.

};

#endif
