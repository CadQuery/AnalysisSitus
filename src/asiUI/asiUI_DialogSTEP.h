//-----------------------------------------------------------------------------
// Created on: 16 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogSTEP_h
#define asiUI_DialogSTEP_h

// Common includes
#include <asiAlgo_Variableiable.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Sequence.hxx>

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

  asiUI_DialogSTEP(const Mode mode,
                  QWidget*   parent = NULL);

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

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
