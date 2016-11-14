//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_CreateContourCallback_h
#define exeCC_CreateContourCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for contour creation.
class exeCC_CreateContourCallback : public QObject,
                                    public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  static exeCC_CreateContourCallback* New();
  //
  vtkTypeMacro(exeCC_CreateContourCallback, asiUI_ViewerCallback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

public:

  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh_facets)
  {
    m_bvh = bvh_facets;
  }

private:

  exeCC_CreateContourCallback  (asiUI_Viewer* theViewer);
  ~exeCC_CreateContourCallback ();

private:

  Handle(asiAlgo_BVHFacets) m_bvh; //!< Accelerating structure for picking.

};

#endif
