//-----------------------------------------------------------------------------
// Created on: 19 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_dmu_visu_h
#define gui_controls_dmu_visu_h

// Qt includes
#pragma warning(push, 0)
#include <QToolButton>
#include <QGridLayout>
#pragma warning(pop)

//! Visualization controls in dMU viewer.
class gui_controls_dmu_visu : public QWidget
{
  Q_OBJECT

public:

  gui_controls_dmu_visu(QWidget* parent = NULL);
  virtual ~gui_controls_dmu_visu();

public slots:

  void onToggleLinks        ();
  void onToggleEdges        ();
  void onToggleTransparency ();

private:

  QGridLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QToolButton* pToggleLinks;        //!< Show/hide links.
    QToolButton* pToggleEdges;        //!< Show/hide topological edges.
    QToolButton* pToggleTransparency; //!< Enable/disable transparency.

    t_widgets() : pToggleLinks        (NULL),
                  pToggleEdges        (NULL),
                  pToggleTransparency (NULL)
    {}

    void Release()
    {
      delete pToggleLinks;        pToggleLinks        = NULL;
      delete pToggleEdges;        pToggleEdges        = NULL;
      delete pToggleTransparency; pToggleTransparency = NULL;
    }
  };

  t_widgets m_widgets;  //!< Involved widgets.
};

#endif
