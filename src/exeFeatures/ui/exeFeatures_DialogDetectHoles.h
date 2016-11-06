//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeFeatures_DialogDetectHoles_h
#define exeFeatures_DialogDetectHoles_h

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for configuring holes identification.
class exeFeatures_DialogDetectHoles : public QDialog
{
  Q_OBJECT

public:

  exeFeatures_DialogDetectHoles(QWidget* parent = NULL);

  virtual ~exeFeatures_DialogDetectHoles();

public slots:

  void onIdentify();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pIdentify;          //!< Identify features.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pRadius;            //!< Radius of interest.
    QCheckBox*      pIsHardFeatureMode; //!< Hard/soft feature detection mode.
  //---------------------------------------------------------------------------

    t_widgets() : pIdentify          (NULL),
                  pRadius            (NULL),
                  pIsHardFeatureMode (NULL)
    {}

    void Release()
    {
      delete pIdentify;          pIdentify          = NULL;
      delete pRadius;            pRadius            = NULL;
      delete pIsHardFeatureMode; pIsHardFeatureMode = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
