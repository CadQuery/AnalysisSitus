//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_rotation_callback_h
#define visu_rotation_callback_h

// A-Situs (visualization) includes
#include <visu_mesh_data_provider.h>
#include <visu_mesh_pipeline.h>
#include <visu_viewer_callback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// OMFDS includes
#include <OMFDS_Mesh.hxx>

//-----------------------------------------------------------------------------
// Rotation center provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_rotation_center_provider, visu_mesh_data_provider)

//! Data Provider for rotation center.
class visu_rotation_center_provider : public visu_mesh_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_rotation_center_provider, visu_mesh_data_provider)

public:

  visu_rotation_center_provider();

public:

  void Init(const double x, const double y, const double z, const double side);

public:

  //! \return mesh data structure.
  virtual Handle(OMFDS_Mesh) GetMeshDS() const
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

  Handle(OMFDS_Mesh) m_mesh; //!< Mesh representing the cube to draw.

};

//-----------------------------------------------------------------------------
// Rotation callback
//-----------------------------------------------------------------------------

//! Callback for rotation operation. Cooperates with VTK window via VTK
//! command pattern. Manages two events: rotation start and rotation end.
class visu_rotation_callback : public visu_viewer_callback
{
public:

  static visu_rotation_callback* New();
  static visu_rotation_callback* New(gui_viewer* theViewer);
  vtkTypeMacro(visu_rotation_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

protected:

  void   setVisualProps ();
  double getScaledSize  (vtkRenderer* theRenderer);

private:

  visu_rotation_callback  (gui_viewer* theViewer);
  ~visu_rotation_callback ();

protected:

  Handle(visu_mesh_pipeline)             m_pl;         //!< Pipeline for rotation center.
  Handle(visu_rotation_center_provider)  m_prv;        //!< Data Provider.
  bool                                   m_bIsStarted; //!< Used for optimization only.

};

#endif
