//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_EdgeDomainPipeline_h
#define asiVisu_EdgeDomainPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_EdgeDomainPipeline, asiVisu_Pipeline)

//! Visualization pipeline for edge domain.
class asiVisu_EdgeDomainPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_EdgeDomainPipeline, asiVisu_Pipeline)

public:

  asiVisu_EdgeDomainPipeline();

public:

  virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_EdgeDomainPipeline(const asiVisu_EdgeDomainPipeline&);

  //! Assignment prohibited.
  asiVisu_EdgeDomainPipeline& operator=(const asiVisu_EdgeDomainPipeline&);

};

#endif
