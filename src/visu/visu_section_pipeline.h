//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_section_pipeline_h
#define visu_section_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_section_pipeline, visu_pipeline)

//! Visualization pipeline for a single skinning section.
class visu_section_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_section_pipeline, visu_pipeline)

public:

  visu_section_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_section_pipeline(const visu_section_pipeline&);

  //! Assignment prohibited.
  visu_section_pipeline& operator=(const visu_section_pipeline&);

private:

  bool m_bMapperColorsSet; //!< Boolean flag indicating whether lookup table is set.

};

#endif
