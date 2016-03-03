//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_curve_pipeline_h
#define visu_curve_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_curve_pipeline, visu_pipeline)

//! Visualization pipeline for a parametric curve.
class visu_curve_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_curve_pipeline, visu_pipeline)

public:

  visu_curve_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_curve_pipeline(const visu_curve_pipeline&);

  //! Assignment prohibited.
  visu_curve_pipeline& operator=(const visu_curve_pipeline&);

};

#endif
