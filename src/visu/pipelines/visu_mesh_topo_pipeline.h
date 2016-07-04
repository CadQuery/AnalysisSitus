//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_topo_pipeline_h
#define visu_mesh_topo_pipeline_h

// Visualization includes
#include <visu_pipeline.h>

DEFINE_STANDARD_HANDLE(visu_mesh_topo_pipeline, visu_pipeline)

//! Visualization pipeline for topological constraints on the mesh.
class visu_mesh_topo_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_mesh_topo_pipeline, visu_pipeline)

public:

  visu_mesh_topo_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& dataProvider);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_mesh_topo_pipeline(const visu_mesh_topo_pipeline&);

  //! Assignment prohibited.
  visu_mesh_topo_pipeline& operator=(const visu_mesh_topo_pipeline&);

};

#endif
