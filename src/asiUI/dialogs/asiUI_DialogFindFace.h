//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_DialogFindFace_h
#define asiUI_DialogFindFace_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>
#include <asiUI_ViewerPart.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>

// Qt includes
#pragma warning(push, 0)
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for finding face of interest.
class asiUI_DialogFindFace : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogFindFace(const Handle(asiEngine_Model)&             model,
                       const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                       asiUI_ViewerPart*                          pViewer,
                       QWidget*                                   parent = NULL);

  virtual ~asiUI_DialogFindFace();

public slots:

  void onUseAddress();
  void onFind();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pFind;       //!< Find face of interest.
  //---------------------------------------------------------------------------
    QCheckBox*      pUseAddress; //!< Indicates whether to use address.
    asiUI_LineEdit* pIndex;      //!< Face index.
    asiUI_LineEdit* pAddress;    //!< Face address.
  //---------------------------------------------------------------------------

    t_widgets() : pFind       (NULL),
                  pUseAddress (NULL),
                  pIndex      (NULL),
                  pAddress    (NULL)
    {}

    void Release()
    {
      delete pFind;       pFind       = NULL;
      delete pUseAddress; pUseAddress = NULL;
      delete pIndex;      pIndex      = NULL;
      delete pAddress;    pAddress    = NULL;
    }
  };

  t_widgets                           m_widgets;     //!< UI controls.
  QVBoxLayout*                        m_pMainLayout; //!< Layout of the widget.
  asiUI_ViewerPart*                   m_pViewer;     //!< External reference to viewer.
  Handle(asiEngine_Model)             m_model;       //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;      //!< Presentation Manager.

};

#endif
