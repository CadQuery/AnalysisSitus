//-----------------------------------------------------------------------------
// Created on: 25 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_build_offsets_h
#define gui_dialog_build_offsets_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_line_edit.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

// SPE includes
#include <SpeCore_Plate.h>

//! Dialog to manage the construction of offsets.
class gui_dialog_build_offsets : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_build_offsets(const Handle(SpeCore_Plate)& plate,
                           QWidget*                     parent = NULL);

  virtual ~gui_dialog_build_offsets();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pPerform;   //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    gui_line_edit* pOffset;    //!< Offset value.
    gui_line_edit* pThickness; //!< Precision to use.
    gui_line_edit* pShift;     //!< Precision to use.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform   (NULL),
                  pOffset    (NULL),
                  pThickness (NULL),
                  pShift     (NULL)
    {}

    void Release()
    {
      delete pPerform;   pPerform   = NULL;
      delete pOffset;    pOffset    = NULL;
      delete pThickness; pThickness = NULL;
      delete pShift;     pShift     = NULL;
    }
  };

  Handle(SpeCore_Plate) m_plate;       //!< Plate base to offset.
  t_widgets             m_widgets;     //!< UI controls.
  QVBoxLayout*          m_pMainLayout; //!< Layout of the widget.

};

#endif
