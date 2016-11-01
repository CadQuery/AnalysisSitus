//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_SectionPipeline_h
#define asiVisu_SectionPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_SectionPipeline, asiVisu_Pipeline)

//! Visualization pipeline for a single skinning section.
class asiVisu_SectionPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_SectionPipeline, asiVisu_Pipeline)

public:

  asiVisu_SectionPipeline();

public:

  virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_SectionPipeline(const asiVisu_SectionPipeline&);

  //! Assignment prohibited.
  asiVisu_SectionPipeline& operator=(const asiVisu_SectionPipeline&);

private:

  bool m_bMapperColorsSet; //!< Boolean flag indicating whether lookup table is set.

};

#endif
