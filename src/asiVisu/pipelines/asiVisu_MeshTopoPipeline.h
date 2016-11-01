//-----------------------------------------------------------------------------
// Created on: 31 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshTopoPipeline_h
#define asiVisu_MeshTopoPipeline_h

// Visualization includes
#include <asiVisu_Pipeline.h>

DEFINE_STANDARD_HANDLE(asiVisu_MeshTopoPipeline, asiVisu_Pipeline)

//! Visualization pipeline for topological constraints on the mesh.
class asiVisu_MeshTopoPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshTopoPipeline, asiVisu_Pipeline)

public:

  asiVisu_MeshTopoPipeline();

public:

  virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_MeshTopoPipeline(const asiVisu_MeshTopoPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshTopoPipeline& operator=(const asiVisu_MeshTopoPipeline&);

};

#endif
