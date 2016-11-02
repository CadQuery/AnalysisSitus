//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_RESurfacesPipeline_h
#define asiVisu_RESurfacesPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_RESurfacesPipeline, asiVisu_Pipeline)

//! Visualization pipeline for re-engineering surfaces.
class asiVisu_RESurfacesPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RESurfacesPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_RESurfacesPipeline();

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
  asiVisu_RESurfacesPipeline(const asiVisu_RESurfacesPipeline&);

  //! Assignment prohibited.
  asiVisu_RESurfacesPipeline& operator=(const asiVisu_RESurfacesPipeline&);

protected:

  int m_iStepsNumber; //!< Number of steps for sampling the parametric space.

};

#endif
