//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_points_pipeline_h
#define visu_points_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_points_pipeline, visu_pipeline)

//! Visualization pipeline for point cloud.
class visu_points_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_points_pipeline, visu_pipeline)

public:

  visu_points_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_points_pipeline(const visu_points_pipeline&);

  //! Assignment prohibited.
  visu_points_pipeline& operator=(const visu_points_pipeline&);

};

#endif
