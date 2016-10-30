//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_controls_pcloud_h
#define gui_controls_pcloud_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for point cloud controls.
class gui_controls_pcloud : public QWidget
{
  Q_OBJECT

public:

  gui_controls_pcloud(QWidget* parent = NULL);
  virtual ~gui_controls_pcloud();

public slots:

  void onLoadPoints();
  void onEstimNormals();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadPoints;   //!< Button to load points.
    QPushButton* pEstimNormals; //!< Button to calculate normals.

    t_widgets() : pLoadPoints   (NULL),
                  pEstimNormals (NULL)
    {}

    void Release()
    {
      delete pLoadPoints;   pLoadPoints   = NULL;
      delete pEstimNormals; pEstimNormals = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
