//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncUnivariatePipeline_h
#define asiVisu_FuncUnivariatePipeline_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_FuncUnivariatePipeline, asiVisu_Pipeline)

//! Visualization pipeline for explicit univariate functions.
class asiVisu_FuncUnivariatePipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncUnivariatePipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_FuncUnivariatePipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_FuncUnivariatePipeline(const asiVisu_FuncUnivariatePipeline&);

  //! Assignment prohibited.
  asiVisu_FuncUnivariatePipeline& operator=(const asiVisu_FuncUnivariatePipeline&);

};

#endif
