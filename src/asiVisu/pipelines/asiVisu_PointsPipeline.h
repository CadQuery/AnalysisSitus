//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PointsPipeline_h
#define asiVisu_PointsPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PointsPipeline, asiVisu_Pipeline)

//! Visualization pipeline for point cloud.
class asiVisu_PointsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PointsPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_PointsPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_PointsPipeline(const asiVisu_PointsPipeline&);

  //! Assignment prohibited.
  asiVisu_PointsPipeline& operator=(const asiVisu_PointsPipeline&);

};

#endif
