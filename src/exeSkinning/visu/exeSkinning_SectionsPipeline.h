//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_SectionsPipeline_h
#define asiVisu_SectionsPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_SectionsPipeline, asiVisu_Pipeline)

//! Visualization pipeline for a set of skinning sections.
class asiVisu_SectionsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_SectionsPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_SectionsPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_SectionsPipeline(const asiVisu_SectionsPipeline&);

  //! Assignment prohibited.
  asiVisu_SectionsPipeline& operator=(const asiVisu_SectionsPipeline&);

};

#endif
