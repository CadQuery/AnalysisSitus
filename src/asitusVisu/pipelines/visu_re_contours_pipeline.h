//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_re_contours_pipeline_h
#define visu_re_contours_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_re_contours_pipeline, visu_pipeline)

//! Visualization pipeline for re-engineering contours.
class visu_re_contours_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_re_contours_pipeline, visu_pipeline)

public:

  visu_re_contours_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_re_contours_pipeline(const visu_re_contours_pipeline&);

  //! Assignment prohibited.
  visu_re_contours_pipeline& operator=(const visu_re_contours_pipeline&);

};

#endif
