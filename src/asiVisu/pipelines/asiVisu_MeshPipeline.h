//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshPipeline_h
#define asiVisu_MeshPipeline_h

// Visualization includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

DEFINE_STANDARD_HANDLE(asiVisu_MeshPipeline, asiVisu_Pipeline)

//! Visualization pipeline for facets and meshes.
class asiVisu_MeshPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

public:

  asiVisu_EXPORT void
    EmptyGroupForAllModeOn();

  asiVisu_EXPORT void
    EmptyGroupForAllModeOff();

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_MeshPipeline(const asiVisu_MeshPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshPipeline& operator=(const asiVisu_MeshPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_Last = 1
  };

protected:

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Indicates whether to treat empty mesh group as a signal to keep all
  //! mesh elements visualized. Otherwise all them are just skipped.
  bool m_bIsEmptyGroupForAll;

};

#endif
