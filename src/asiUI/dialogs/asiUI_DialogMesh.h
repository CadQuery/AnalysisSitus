//-----------------------------------------------------------------------------
// Created on: 16 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_tess_h
#define gui_dialog_tess_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#pragma warning(push, 0)
#include <AIS_Shape.hxx>
#pragma warning(pop)

//! Dialog for meshing.
class asiUI_DialogMesh : public QDialog
{
  Q_OBJECT

public:

  asiUI_DialogMesh(QWidget* parent = NULL);

  virtual ~asiUI_DialogMesh();

public slots:

  void onGenerate();

protected:

  //! Widgets.
  struct t_widgets
  {
    //! Editor IDs.
    enum editor
    {
      edit_LinearDeflection = 0,
      edit_AngularDeflection,
      //
      LastEditor
    };

  //---------------------------------------------------------------------------
    QPushButton*   pGenerate; //!< Generate mesh.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pLinearDeflection;  //!< Linear mesh-surface deflection.
    asiUI_LineEdit* pAngularDeflection; //!< Angular mesh-surface deflection.
  //---------------------------------------------------------------------------

    t_widgets() : pGenerate          (NULL),
                  pLinearDeflection  (NULL),
                  pAngularDeflection (NULL)
    {}

    void Release()
    {
      delete pGenerate;          pGenerate          = NULL;
      delete pLinearDeflection;  pLinearDeflection  = NULL;
      delete pAngularDeflection; pAngularDeflection = NULL;
    }
  };

  t_widgets    m_widgets;     //!< UI controls.
  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

};

#endif
