//-----------------------------------------------------------------------------
// Created on: 16 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_STEP_h
#define gui_dialog_STEP_h

// Common includes
#include <common_variable.h>

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
class gui_dialog_STEP : public QDialog
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

  gui_dialog_STEP(const Mode mode,
                  QWidget*   parent = NULL);

  virtual ~gui_dialog_STEP();

public slots:

  void onVarChanged(QTableWidgetItem*);
  void onProceed();

protected:

  void initialize();
  void initialize_Read(NCollection_Sequence<common_var>& vars);
  void initialize_Write(NCollection_Sequence<common_var>& vars);
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
