//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_face_domain_pipeline_h
#define visu_face_domain_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_face_domain_pipeline, visu_pipeline)

//! Visualization pipeline for face domain.
class visu_face_domain_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_face_domain_pipeline, visu_pipeline)

public:

  ASitus_EXPORT
    visu_face_domain_pipeline();

public:

  ASitus_EXPORT virtual void
    SetInput(const Handle(visu_data_provider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_face_domain_pipeline(const visu_face_domain_pipeline&);

  //! Assignment prohibited.
  visu_face_domain_pipeline& operator=(const visu_face_domain_pipeline&);

private:

  bool m_bMapperColorsSet; //!< Boolean flag indicating whether lookup table is set.

};

#endif
