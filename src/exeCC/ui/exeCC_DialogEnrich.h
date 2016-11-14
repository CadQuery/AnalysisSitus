//-----------------------------------------------------------------------------
// Created on: 07 October 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_DialogEnrich_h
#define exeCC_DialogEnrich_h

// Geometry includes
#include <asiAlgo_BVHFacets.h>

// GUI includes
#include <asiUI_LineEdit.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for enriching contour with additional points.
class exeCC_DialogEnrich : public QDialog
{
  Q_OBJECT

public:

  exeCC_DialogEnrich(const Handle(asiAlgo_BVHFacets)& bvh,
                     QWidget*                         parent = NULL);

  virtual ~exeCC_DialogEnrich();

public slots:

  void onPerform();

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QPushButton*    pPerform;   //!< Runs the algorithm.
  //---------------------------------------------------------------------------
    asiUI_LineEdit* pNumPoints; //!< Number of additional points on each edge.
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

  t_widgets                 m_widgets;     //!< UI controls.
  QVBoxLayout*              m_pMainLayout; //!< Layout of the widget.
  Handle(asiAlgo_BVHFacets) m_bvh;         //!< Accelerating structure for picking.

};

#endif
