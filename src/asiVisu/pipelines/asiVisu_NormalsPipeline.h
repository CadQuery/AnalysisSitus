//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_NormalsPipeline_h
#define asiVisu_NormalsPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_NormalsPipeline, asiVisu_Pipeline)

//! Visualization pipeline for normals.
class asiVisu_NormalsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_NormalsPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_NormalsPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_NormalsPipeline(const asiVisu_NormalsPipeline&);

  //! Assignment prohibited.
  asiVisu_NormalsPipeline& operator=(const asiVisu_NormalsPipeline&);

};

#endif
