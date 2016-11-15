//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_RotationCallback_h
#define asiUI_RotationCallback_h

// A-Situs (UI) includes
#include <asiUI_ViewerCallback.h>

// A-Situs (visualization) includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Mesh (Active Data) includes
#include <Mesh.h>

//-----------------------------------------------------------------------------
// Rotation center provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiUI_RotationCenterCallback, asiVisu_MeshDataProvider)

//! Data Provider for rotation center.
class asiUI_RotationCenterCallback : public asiVisu_MeshDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_RotationCenterCallback, asiVisu_MeshDataProvider)

public:

  asiUI_EXPORT
    asiUI_RotationCenterCallback();

public:

  asiUI_EXPORT void
    Init(const double x,
         const double y,
         const double z,
         const double side);

public:

  //! \return mesh data structure.
  virtual Handle(Mesh) GetMeshDS() const
  {
    return m_mesh;
  }

  //! Returns ID of the associated Node. Not used in this case.
  //! \return ID of the associated Node.
  virtual ActAPI_DataObjectId GetNodeID() const
  {
    return ActAPI_DataObjectId();
  }

  //! Not used.
  virtual bool MustExecute(const Handle(ActAux_TimeStamp)&) const
  {
    return true;
  }

protected:

  //! Returns sources for domain-VTK translation. Not used in this case.
  //! \return translation sources.
  virtual Handle(ActAPI_HParameterList) translationSources() const
  {
    return NULL;
  }

protected:

  Handle(Mesh) m_mesh; //!< Mesh representing the cube to draw.

};

//-----------------------------------------------------------------------------
// Rotation callback
//-----------------------------------------------------------------------------

//! Callback for rotation operation. Cooperates with VTK window via VTK
//! command pattern. Manages two events: rotation start and rotation end.
class asiUI_RotationCallback : public asiUI_ViewerCallback
{
public:

  asiUI_EXPORT static asiUI_RotationCallback*
    New();

  asiUI_EXPORT static asiUI_RotationCallback*
    New(asiUI_Viewer* viewer);

  vtkTypeMacro(asiUI_RotationCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

protected:

  asiUI_EXPORT void
    setVisualProps();

  asiUI_EXPORT double
    getScaledSize(vtkRenderer* pRenderer);

protected:

  asiUI_EXPORT
    asiUI_RotationCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_RotationCallback();

protected:

  Handle(asiVisu_MeshPipeline)         m_pl;         //!< Pipeline for rotation center.
  Handle(asiUI_RotationCenterCallback) m_prv;        //!< Data Provider.
  bool                                 m_bIsStarted; //!< Used for optimization only.

};

#endif
