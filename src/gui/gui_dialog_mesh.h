//-----------------------------------------------------------------------------
// Created on: 16 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_mesh_h
#define gui_dialog_mesh_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_line_edit.h>

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
class gui_dialog_mesh : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_mesh(QWidget* parent = NULL);

  virtual ~gui_dialog_mesh();

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
    gui_line_edit* pLinearDeflection;  //!< Linear mesh-surface deflection.
    gui_line_edit* pAngularDeflection; //!< Angular mesh-surface deflection.
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
