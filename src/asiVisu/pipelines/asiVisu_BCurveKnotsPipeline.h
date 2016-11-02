//-----------------------------------------------------------------------------
// Created on: 02 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_BCurveKnotsPipeline_h
#define asiVisu_BCurveKnotsPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_BCurveKnotsPipeline, asiVisu_Pipeline)

//! Visualization pipeline for b-curve knots.
class asiVisu_BCurveKnotsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_BCurveKnotsPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_BCurveKnotsPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_BCurveKnotsPipeline(const asiVisu_BCurveKnotsPipeline&);

  //! Assignment prohibited.
  asiVisu_BCurveKnotsPipeline& operator=(const asiVisu_BCurveKnotsPipeline&);

};

#endif
