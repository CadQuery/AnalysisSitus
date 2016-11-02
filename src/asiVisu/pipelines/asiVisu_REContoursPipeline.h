//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_REContoursPipeline_h
#define asiVisu_REContoursPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_REContoursPipeline, asiVisu_Pipeline)

//! Visualization pipeline for re-engineering contours.
class asiVisu_REContoursPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REContoursPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_REContoursPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_REContoursPipeline(const asiVisu_REContoursPipeline&);

  //! Assignment prohibited.
  asiVisu_REContoursPipeline& operator=(const asiVisu_REContoursPipeline&);

};

#endif
