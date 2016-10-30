//-----------------------------------------------------------------------------
// Created on: 07 October 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_contour_enrich_h
#define gui_dialog_contour_enrich_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_bvh_facets.h>

// GUI includes
#include <gui_line_edit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for enriching contour with additional points.
class gui_dialog_contour_enrich : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_contour_enrich(const Handle(geom_bvh_facets)& bvh,
                            QWidget* parent = NULL);

  virtual ~gui_dialog_contour_enrich();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*   pPerform;   //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    gui_line_edit* pNumPoints; //!< Number of additional points on each edge.
  //---------------------------------------------------------------------------

    t_widgets() : pPerform   (NULL),
                  pNumPoints (NULL)
    {}

    void Release()
    {
      delete pPerform;   pPerform   = NULL;
      delete pNumPoints; pNumPoints = NULL;
    }
  };

  t_widgets               m_widgets;     //!< UI controls.
  QVBoxLayout*            m_pMainLayout; //!< Layout of the widget.
  Handle(geom_bvh_facets) m_bvh;         //!< Accelerating structure for picking.

};

#endif
