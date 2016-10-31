//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_re_surfaces_pipeline_h
#define visu_re_surfaces_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_re_surfaces_pipeline, visu_pipeline)

//! Visualization pipeline for re-engineering surfaces.
class visu_re_surfaces_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_re_surfaces_pipeline, visu_pipeline)

public:

  visu_re_surfaces_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

public:

  inline void SetStepsNumber(const int nbSteps)       { m_iStepsNumber = nbSteps; }
  inline int  GetStepsNumber()                  const { return m_iStepsNumber; }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_re_surfaces_pipeline(const visu_re_surfaces_pipeline&);

  //! Assignment prohibited.
  visu_re_surfaces_pipeline& operator=(const visu_re_surfaces_pipeline&);

protected:

  int m_iStepsNumber; //!< Number of steps for sampling the parametric space.

};

#endif
