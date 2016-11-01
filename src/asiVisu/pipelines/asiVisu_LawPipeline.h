//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_law_func_pipeline_h
#define visu_law_func_pipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_LawPipeline, asiVisu_Pipeline)

//! Visualization pipeline for explicit design laws.
class asiVisu_LawPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_LawPipeline, asiVisu_Pipeline)

public:

  asiVisu_LawPipeline();

public:

  virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_LawPipeline(const asiVisu_LawPipeline&);

  //! Assignment prohibited.
  asiVisu_LawPipeline& operator=(const asiVisu_LawPipeline&);

};

#endif
