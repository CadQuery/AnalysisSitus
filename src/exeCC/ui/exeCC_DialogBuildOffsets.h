//-----------------------------------------------------------------------------
// Created on: 25 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_DialogBuildOffsets_h
#define exeCC_DialogBuildOffsets_h

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

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
class exeCC_DialogBuildOffsets : public QDialog
{
  Q_OBJECT

public:

  exeCC_DialogBuildOffsets(const Handle(SpeCore_Plate)& plate,
                           QWidget*                     parent = NULL);

  virtual ~exeCC_DialogBuildOffsets();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pPerform;   //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pOffset;    //!< Offset value.
    asiUI_LineEdit* pThickness; //!< Precision to use.
    asiUI_LineEdit* pShift;     //!< Precision to use.
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
