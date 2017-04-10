//-----------------------------------------------------------------------------
// Created on: 01 December 2016 (*)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_PickEdgeCallback_h
#define asiUI_PickEdgeCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// asiEngine includes
#include <asiEngine_Model.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking an edge in 3D viewer.
class asiUI_PickEdgeCallback : public asiUI_ViewerCallback
{
public:

  asiUI_EXPORT static asiUI_PickEdgeCallback*
    New();

  vtkTypeMacro(asiUI_PickEdgeCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

public:

  //! Sets Data Model instance to access the geometry to pick.
  //! \param model [in] Data Model instance.
  void SetModel(const Handle(asiEngine_Model)& model)
  {
    m_model = model;
  }

private:

  asiUI_EXPORT
    asiUI_PickEdgeCallback(asiUI_Viewer* viewer);

  asiUI_EXPORT
    ~asiUI_PickEdgeCallback();

private:

  Handle(asiEngine_Model) m_model; //!< Data Model instance.

};

#endif
