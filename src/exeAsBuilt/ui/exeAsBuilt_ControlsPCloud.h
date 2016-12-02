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

// OCCT includes
#include <Standard_Type.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

// VTK includes
#include <vtkPlaneWidget.h>
#include <vtkSmartPointer.h>

class exeAsBuilt_FlannKdTree;

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
  void onPickPoint    ();

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
    QPushButton*                    pPickPointBtn;    //!< Button to pick a point.

    t_widgets() : pLoadPointsBtn   (NULL),
                  pCloudifyBtn     (NULL),
                  pEstimNormalsBtn (NULL),
                  pProjPlaneBtn    (NULL),
                  pPickPointBtn    (NULL)
    {}

    void Release()
    {
      delete pLoadPointsBtn;   pLoadPointsBtn   = NULL;
      delete pCloudifyBtn;     pCloudifyBtn     = NULL;
      delete pEstimNormalsBtn; pEstimNormalsBtn = NULL;
      delete pProjPlaneBtn;    pProjPlaneBtn    = NULL;
      delete pPickPointBtn;    pPickPointBtn    = NULL;
    }
  };

  t_widgets                      m_widgets;      //!< Involved widgets.
  Handle(exeAsBuilt_FlannKdTree) m_kdTree;       //!< K-d tree.
  int                            m_iPrevSelMode; //!< Cached selection mode.

};

#endif
