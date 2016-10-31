//-----------------------------------------------------------------------------
// Created on: 01 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_shaded_surface_pipeline_h
#define visu_shaded_surface_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>
#include <visu_surface_source.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_shaded_surface_pipeline, visu_pipeline)

//! Visualization pipeline for a parametric surface in shaded view.
class visu_shaded_surface_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_shaded_surface_pipeline, visu_pipeline)

public:

  visu_shaded_surface_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

public:

  inline void SetStepsNumber (const int nbSteps)       { m_iStepsNumber = nbSteps; }
  inline int  GetStepsNumber ()                  const { return m_iStepsNumber; }
  inline void ForceUpdate    ()                        { m_bForced = true; }
//
  bool IsCurvature         () const;
  bool IsGaussianCurvature () const;
  bool IsMeanCurvature     () const;
  void GaussianCurvatureOn ();
  void MeanCurvatureOn     ();
  void CurvatureOff        ();

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_shaded_surface_pipeline(const visu_shaded_surface_pipeline&);

  //! Assignment prohibited.
  visu_shaded_surface_pipeline& operator=(const visu_shaded_surface_pipeline&);

protected:

  int  m_iStepsNumber;       //!< Number of steps for sampling the parametric space.
  bool m_bForced;            //!< Not very graceful flag to force execution even if
                             //!< no persistent data has changed.
  //
  vtkSmartPointer<visu_surface_source> m_source; //!< Polygonal data source.

};

#endif
