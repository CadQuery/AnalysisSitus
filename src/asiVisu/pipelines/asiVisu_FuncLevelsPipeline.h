//-----------------------------------------------------------------------------
// Created on: 28 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncLevelsPipeline_h
#define asiVisu_FuncLevelsPipeline_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for function level sets.
template <typename T_VARIABLE>
class asiVisu_FuncLevelsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncLevelsPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_FuncLevelsPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_FuncLevelsPipeline(const asiVisu_FuncLevelsPipeline&);

  //! Assignment prohibited.
  asiVisu_FuncLevelsPipeline& operator=(const asiVisu_FuncLevelsPipeline&);

};

#endif
