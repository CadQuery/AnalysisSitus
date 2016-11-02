//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_CurvePipeline_h
#define asiVisu_CurvePipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_CurvePipeline, asiVisu_Pipeline)

//! Visualization pipeline for a parametric curve.
class asiVisu_CurvePipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_CurvePipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_CurvePipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_CurvePipeline(const asiVisu_CurvePipeline&);

  //! Assignment prohibited.
  asiVisu_CurvePipeline& operator=(const asiVisu_CurvePipeline&);

};

#endif
