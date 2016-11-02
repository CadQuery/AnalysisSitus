//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_EdgeCurvePipeline_h
#define asiVisu_EdgeCurvePipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_EdgeCurvePipeline, asiVisu_Pipeline)

//! Visualization pipeline for a host curve of an edge.
class asiVisu_EdgeCurvePipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_EdgeCurvePipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_EdgeCurvePipeline();

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
  asiVisu_EdgeCurvePipeline(const asiVisu_EdgeCurvePipeline&);

  //! Assignment prohibited.
  asiVisu_EdgeCurvePipeline& operator=(const asiVisu_EdgeCurvePipeline&);

protected:

  int m_iStepsNumber; //!< Number of steps for sampling the parametric space.

};

#endif
