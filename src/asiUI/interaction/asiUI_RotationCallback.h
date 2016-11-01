//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_RotationCallback_h
#define asiUI_RotationCallback_h

// A-Situs (visualization) includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>
#include <asiVisu_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Mesh (Active Data) includes
#include <Mesh.h>

//-----------------------------------------------------------------------------
// Rotation center provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_RotationCenterCallback, asiVisu_MeshDataProvider)

//! Data Provider for rotation center.
class asiVisu_RotationCenterCallback : public asiVisu_MeshDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RotationCenterCallback, asiVisu_MeshDataProvider)

public:

  asiVisu_RotationCenterCallback();

public:

  void Init(const double x, const double y, const double z, const double side);

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
class asiUI_RotationCallback : public asiVisu_ViewerCallback
{
public:

  static asiUI_RotationCallback* New();
  static asiUI_RotationCallback* New(asiUI_Viewer* theViewer);
  vtkTypeMacro(asiUI_RotationCallback, asiVisu_ViewerCallback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

protected:

  void   setVisualProps ();
  double getScaledSize  (vtkRenderer* theRenderer);

private:

  asiUI_RotationCallback  (asiUI_Viewer* theViewer);
  ~asiUI_RotationCallback ();

protected:

  Handle(asiVisu_MeshPipeline)           m_pl;         //!< Pipeline for rotation center.
  Handle(asiVisu_RotationCenterCallback) m_prv;        //!< Data Provider.
  bool                                   m_bIsStarted; //!< Used for optimization only.

};

#endif
