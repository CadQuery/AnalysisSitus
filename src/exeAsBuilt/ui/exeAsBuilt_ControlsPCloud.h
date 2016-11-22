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

// VTK includes
#include <vtkPlaneWidget.h>
#include <vtkSmartPointer.h>

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
  void onProjPlane    ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton*                    pLoadPointsBtn;   //!< Button to load points.
    QPushButton*                    pCloudifyBtn;     //!< Button to cloudify CAD part.
    QPushButton*                    pEstimNormalsBtn; //!< Button to calculate normals.
    QPushButton*                    pProjPlaneBtn;    //!< Button to enable/disable a projection plane.
    vtkSmartPointer<vtkPlaneWidget> pProjPlane;       //!< Projection plane widget.

    t_widgets() : pLoadPointsBtn   (NULL),
                  pCloudifyBtn     (NULL),
                  pEstimNormalsBtn (NULL),
                  pProjPlaneBtn    (NULL)
    {}

    void Release()
    {
      delete pLoadPointsBtn;   pLoadPointsBtn   = NULL;
      delete pCloudifyBtn;     pCloudifyBtn     = NULL;
      delete pEstimNormalsBtn; pEstimNormalsBtn = NULL;
      delete pProjPlaneBtn;    pProjPlaneBtn    = NULL;
    }
  };

  t_widgets m_widgets; //!< Involved widgets.

};

#endif
