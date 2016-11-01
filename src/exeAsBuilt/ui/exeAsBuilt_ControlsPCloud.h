//-----------------------------------------------------------------------------
// Created on: 21 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_ControlsPCloud_h
#define exeAsBuilt_ControlsPCloud_h

// A-Situs includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Widget for point cloud controls.
class exeAsBuilt_ControlsPCloud : public QWidget
{
  Q_OBJECT

public:

  exeAsBuilt_ControlsPCloud(QWidget* parent = NULL);
  virtual ~exeAsBuilt_ControlsPCloud();

public slots:

  void onLoadPoints   ();
  void onEstimNormals ();
  void onCloudify     ();

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
