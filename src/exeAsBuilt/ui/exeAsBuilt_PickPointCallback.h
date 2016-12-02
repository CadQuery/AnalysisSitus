//-----------------------------------------------------------------------------
// Created on: 02 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_PickPointCallback_h
#define exeAsBuilt_PickPointCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// exeAsBuilt includes
#include <exeAsBuilt_Model.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class exeAsBuilt_FlannKdTree;

//! Callback for picking points in a point cloud.
class exeAsBuilt_PickPointCallback : public asiUI_ViewerCallback
{
public:

  static exeAsBuilt_PickPointCallback*
    New();

  vtkTypeMacro(exeAsBuilt_PickPointCallback, asiUI_ViewerCallback);

public:

  virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

public:

  //! Sets Data Model instance to access the geometry to pick.
  //! \param model [in] Data Model instance.
  void SetModel(const Handle(exeAsBuilt_Model)& model)
  {
    m_model = model;
  }

  //! Sets k-d tree to operate on.
  //! \param kdTree [in] k-d tree to use (must be pre-built).
  void SetKdTree(const Handle(exeAsBuilt_FlannKdTree)& kdTree)
  {
    m_kdTree = kdTree;
  }

private:

  exeAsBuilt_PickPointCallback(asiUI_Viewer* viewer);

  ~exeAsBuilt_PickPointCallback();

private:

  Handle(exeAsBuilt_Model)       m_model;  //!< Data Model instance.
  Handle(exeAsBuilt_FlannKdTree) m_kdTree; //!< K-d tree.

};

#endif
