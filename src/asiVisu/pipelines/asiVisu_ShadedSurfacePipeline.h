//-----------------------------------------------------------------------------
// Created on: 01 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShadedSurfacePipeline_h
#define asiVisu_ShadedSurfacePipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_SurfaceSource.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_ShadedSurfacePipeline, asiVisu_Pipeline)

//! Visualization pipeline for a parametric surface in shaded view.
class asiVisu_ShadedSurfacePipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShadedSurfacePipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_ShadedSurfacePipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  void SetStepsNumber (const int nbSteps)       { m_iStepsNumber = nbSteps; }
  int  GetStepsNumber ()                  const { return m_iStepsNumber; }
  void ForceUpdate    ()                        { m_bForced = true; }
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
  asiVisu_ShadedSurfacePipeline(const asiVisu_ShadedSurfacePipeline&);

  //! Assignment prohibited.
  asiVisu_ShadedSurfacePipeline& operator=(const asiVisu_ShadedSurfacePipeline&);

protected:

  int  m_iStepsNumber; //!< Number of steps for sampling the parametric space.
  bool m_bForced;      //!< Not very graceful flag to force execution even if
                       //!< no persistent data has changed.
  //
  vtkSmartPointer<asiVisu_SurfaceSource> m_source; //!< Polygonal data source.

};

#endif
