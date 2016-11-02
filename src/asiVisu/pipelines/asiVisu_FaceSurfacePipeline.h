//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_FaceSurfacePipeline_h
#define asiVisu_FaceSurfacePipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_FaceSurfacePipeline, asiVisu_Pipeline)

//! Visualization pipeline for a host surface of a face.
class asiVisu_FaceSurfacePipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FaceSurfacePipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_FaceSurfacePipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  void SetStepsNumber(const int nbSteps)       { m_iStepsNumber = nbSteps; }
  int  GetStepsNumber()                  const { return m_iStepsNumber; }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_FaceSurfacePipeline(const asiVisu_FaceSurfacePipeline&);

  //! Assignment prohibited.
  asiVisu_FaceSurfacePipeline& operator=(const asiVisu_FaceSurfacePipeline&);

protected:

  int m_iStepsNumber; //!< Number of steps for sampling the parametric space.

};

#endif
